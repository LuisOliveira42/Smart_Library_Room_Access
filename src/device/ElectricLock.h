#ifndef DEVICE_ELECTRICKLOCK_H
#define DEVICE_ELECTRICKLOCK_H

#include <string.h>

class ElectricLock{
public:
    explicit ElectricLock(const std::string& path);   // opens the device
    ~ElectricLock();                                  // locks + closes the device

    ElectricLock(const ElectricLock&) = delete;
    ElectricLock& operator=(const ElectricLock&) = delete;

    bool isReady() const;
    void unlock();
    void lock();

private:
    int fd;
    bool open(const std::string& path);
};

#endif