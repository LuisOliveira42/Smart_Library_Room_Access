#ifndef DEVICE_DOOR_H
#define DEVICE_DOOR_H

#include <pthreads.h>
#include <string.h>
#include "Types.h"
#include "RGBLed.h"
#include "ElectricLock.h"

class Door{
public:
    explicit Door(const std::string& ledPath, const std::string& lockPath);

    bool isReady() const;                           //LED ON and Lock opened
    void apply(AccessResult r);
    void reset();                                   //LED OFF and Lock closed (after 5s)

private:
    RGBLed led;
    ElectricLock lock;
    pthread_mutex_t hwMutex;
};

#endif