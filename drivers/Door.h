#ifndef DEVICE_DOOR_H
#define DEVICE_DOOR_H

#include <pthread.h>
#include <string>
#include "Common/Types.h"
#include "Device/RGBLed.h"
#include "Device/ElectricLock.h"

class Door {
public:
    Door(const std::string& ledPath, const std::string& lockPath);
    ~Door();                                        //Destroys hwMutex

    Door(const Door&) = delete;
    Door& operator=(const Door&) = delete;

    bool isReady() const;                           //LED ON and Lock opened

    void apply(AccessResult r);

    void reset();                                   //LED OFF and Lock closed (after 5s)

private:
    RGBLed led;
    ElectricLock lock;
    pthread_mutex_t hwMutex;                       //Mutex for hardware access
};

#endif