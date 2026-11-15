#include "database/DatabaseEngine.h"
using namespace std;

DatabaseEngine::DatabaseEngine(const string& path){
    conn = nullptr;
    pthread_mutex_init(&dbMutex, nullptr);
    open(path);
}

DatabaseEngine::~DatabaseEngine(){
    sqlite3_close(conn);                 // does nothing if conn is nullptr
    pthread_mutex_destroy(&dbMutex);
}

void DatabaseEngine::open(const string& path){
    /* READWRITE without CREATE: if the file does not exist, it fails. */
    int rc = sqlite3_open_v2(path.c_str(), &conn, SQLITE_OPEN_READWRITE, nullptr);

    // Foreign keys are off by default and must be turned on
    if (rc == SQLITE_OK) {
        rc = sqlite3_exec(conn, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    }

    if (rc != SQLITE_OK) {
        sqlite3_close(conn);             // open_v2 may create conn even when it fails
        conn = nullptr;                  // -> isReady() returns false
    }
}

bool DatabaseEngine::isReady(){
    return conn != nullptr;
}

DbResult DatabaseEngine::findUser(const string& cardID, User& user){
    DbResult result = DbResult::DB_ERROR;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT student_number, card_id FROM users WHERE card_id = ?;";

    pthread_mutex_lock(&dbMutex);

    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, cardID.c_str(), -1, SQLITE_TRANSIENT);

        int rc = sqlite3_step(stmt); 
        if (rc == SQLITE_ROW) {                          // a row was found
            user.studentNumber = sqlite3_column_int(stmt, 0);
            user.cardID = (const char*) sqlite3_column_text(stmt, 1);
            result = DbResult::DB_OK;
        } else if (rc == SQLITE_DONE) {                  // no rows
            result = DB_NOT_FOUND;
        }
    }

    sqlite3_finalize(stmt);              // does nothing if stmt is nullptr
    pthread_mutex_unlock(&dbMutex);
    return result;
}

DbResult DatabaseEngine::findStudent(int studentNumber, User& user)
{
    DbResult result = DbResult::DB_ERROR;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT student_number, card_id FROM users WHERE student_number = ?;";

    pthread_mutex_lock(&dbMutex);

    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, studentNumber);

        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            user.studentNumber = sqlite3_column_int(stmt, 0);
            user.cardID = (const char*) sqlite3_column_text(stmt, 1);
            result = DbResult::DB_OK;
        } else if (rc == SQLITE_DONE) {
            result = DbResult::DB_NOT_FOUND;
        }
    }

    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&dbMutex);
    return result;
}

DbResult DatabaseEngine::findRoomOwner(time_t slot, User& owner)
{
    DbResult result = DbResult::DB_ERROR;
    sqlite3_stmt* stmt = nullptr;

    /* JOIN: take the reservation of that hour and the matching user row. */
    const char* sql = "SELECT u.student_number, u.card_id "
                      "FROM reservations r JOIN users u "
                      "ON u.student_number = r.student_number "
                      "WHERE r.begin_time = ?;";

    pthread_mutex_lock(&dbMutex);

    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, slot);

        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            owner.studentNumber = sqlite3_column_int(stmt, 0);
            owner.cardID = (const char*) sqlite3_column_text(stmt, 1);
            result = DbResult::DB_OK;
        } else if (rc == SQLITE_DONE) {
            result = DbResult::DB_NOT_FOUND;
        }
    }

    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&dbMutex);
    return result;
}

DbResult DatabaseEngine::saveAccessRecord(const AccessRecord& rec)
{
    DbResult result = DbResult::DB_ERROR;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO access_records (student_number, access_time) VALUES (?, ?);";

    pthread_mutex_lock(&dbMutex);

    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, rec.studentNumber);
        sqlite3_bind_int64(stmt, 2, rec.accessTime);

        if (sqlite3_step(stmt) == SQLITE_DONE) {         // INSERT gives no rows
            result = DbResult::DB_OK;
        }
    }

    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&dbMutex);
    return result;
}

DbResult DatabaseEngine::insertReservation(const Reservation& res)
{
    DbResult result = DbResult::DB_ERROR;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO reservations (begin_time, student_number) VALUES (?, ?);";

    pthread_mutex_lock(&dbMutex);

    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, res.begin);
        sqlite3_bind_int(stmt, 2, res.studentNumber);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            result = DbResult::DB_OK;
        } else if (sqlite3_extended_errcode(conn) == SQLITE_CONSTRAINT_PRIMARYKEY) {
            result = DbResult::DB_OVERLAP;  // that hour is already reserved
        }                                // a bad foreign key stays ERROR
    }

    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&dbMutex);
    return result;
}

DbResult DatabaseEngine::deleteReservation(const Reservation& res)
{
    DbResult result = DbResult::DB_ERROR;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "DELETE FROM reservations WHERE begin_time = ? AND student_number = ?;";

    pthread_mutex_lock(&dbMutex);

    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, res.begin);
        sqlite3_bind_int(stmt, 2, res.studentNumber);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            /* 0 rows deleted: no reservation, or it belongs to someone else. */
            if (sqlite3_changes(conn) == 1) {
                result = DbResult::DB_OK;
            } else {
                result = DbResult::DB_NOT_FOUND;
            }
        }
    }

    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&dbMutex);
    return result;
}