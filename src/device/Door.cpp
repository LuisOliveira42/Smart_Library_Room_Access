#include "Door.h"

Door::Door(const std::string& ledPath, const std::string& lockPath) : led(ledPath), lock(lockPath)
{
    pthreads_mutex_init(&hwMutex, nullptr);
}

Door::~Door()
{
    pthreads_mutex_destroy(&hwMutex);
}

bool Door::isReady() const
{
    return led.isReady() && lock.isReady();
}

void Door::apply(AccessResult r)
{
    pthreads_mutex_lock(&hwMutex);
    switch(r){
        case ACCESS_GRANTED:
            led.changeColor(COLOR_GREEN);
            lock.unlock();
            break;
        case ACCESS_NO_RESERVATION:
            led.changeColor(COLOR_BLUE);
            lock.lock();
            break;
        case ACCESS_DENIED:
            led.changeColor(COLOR_RED);
            lock.lock();
            break;
        default:
            led.changeColor(COLOR_OFF);
            lock.lock();
            break;
    }
    pthreads_mutex_unlock(&hwMutex);
}

void Door::reset()
{
    pthreads_mutex_lock(&hwMutex);
    led.changeColor(COLOR_OFF);
    lock.lock();
    pthreads_mutex_unlock(&hwMutex);
}