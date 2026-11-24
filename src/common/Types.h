#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <ctime>
#include <string>
using namespace std;

/* One row of the "users" table: a student and the card he/she uses. */
struct User {
    int studentNumber = 0;
    string cardID;          // card UID in hex, e.g. "A1B2C3D4"
};

/* One row of the "reservations" table. A reservation lasts one full hour. */
struct Reservation {
    int studentNumber = 0;
    time_t begin = 0;            // Unix time, always a multiple of 3600
};

/* One row of the "access_records" table (only granted entries are saved). */
struct AccessRecord {
    int studentNumber = 0;
    time_t accessTime = 0;       // Unix time
};

/* Result of every DatabaseEngine / ReservationManager operation. */
enum DbResult {
    DB_OK,
    DB_NOT_FOUND,
    DB_OVERLAP,
    DB_INVALID_SLOT,
    DB_ERROR
};

/* Colors of the RGB LED. The numeric value is the character written to
 * /dev/rgbled ("0".."3"), so it must match the rgbled.ko driver. */
enum Color {
    COLOR_OFF   = 0,
    COLOR_RED   = 1,
    COLOR_GREEN = 2,
    COLOR_BLUE  = 3
};

/* Decision taken when a card is read at the door. */
enum AccessResult {
    ACCESS_GRANTED,          // green LED, door unlocks
    ACCESS_DENIED,           // red LED, door stays locked
    ACCESS_NO_RESERVATION    // blue LED, door stays locked
};

/* Duration of one reservation slot, in seconds (1 hour). */
const time_t SLOT_SECONDS = 3600;

#endif // COMMON_TYPES_H