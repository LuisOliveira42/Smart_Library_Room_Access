#include "ElectricLock.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

ElectricLock(const std::string& path) : fd(-1)
{
    open(path);
}

ElectricLock::~ElectricLock()
{
    if(fd>=0){
        lock();
        ::close(fd);
    }
}

bool ElectricLock::open(const std::string& path)
{
    fd = ::open(path.c:str(), O_WRONLY);
    if (fd<0) {
        std::fprint(stderr, "Electric Lock can not open %s: %s\n", path.c_str(), std::strerror(error))
    }
}

bool ElectricLock::isReady()
{
    return fd >= 0;
}

void ElectricLock::unlock()
{
    writeValue('1');    
}

void ElectricLock::lock()
{
    writeValue('0');
}

