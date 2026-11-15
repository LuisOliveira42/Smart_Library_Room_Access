#ifndef DATABASE_DATABASEENGINE_H
#define DATABASE_DATABASEENGINE_H

#include <pthread.h>
#include <sqlite3.h>
#include <ctime>
#include <string>
#include "common/Types.h"
using namespace std;

class DatabaseEngine {
private:
    sqlite3* conn;              // connection to the database file
    pthread_mutex_t dbMutex;    // one thread at a time inside the database

    void open(const std::string& path);   // opens without creating the file

public:
    DatabaseEngine(const string& path);   // opens the file
    ~DatabaseEngine();                         // closes the file

    bool isReady();

    // Card -> student (used at the door)
    DbResult findUser(const string& cardID, User& user);

    // Student number -> student (used to check that the student exists)
    DbResult findStudent(int studentNumber, User& user);

    // Who reserved the slot that starts at "slot"
    DbResult findRoomOwner(time_t slot, User& owner);

    // Saves one granted entry. OK or ERROR
    DbResult saveAccessRecord(const AccessRecord& rec);

    DbResult insertReservation(const Reservation& res);

    // Deletes only if this student owns that slot: OK or NOT_FOUND
    DbResult deleteReservation(const Reservation& res);

};

#endif // DATABASE_DATABASEENGINE_H