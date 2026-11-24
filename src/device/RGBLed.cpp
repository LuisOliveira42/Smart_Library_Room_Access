#include "RGBLed.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

RGBLed::RGBLed(const std::string& path) : fd(-1)
{
    open(path);
}

RGBLed::~RGBLed()
{
    if(fd >= 0){
        changeColor(COLOR_OFF);
        ::close(fd);
    }
}

/* "::open" is needed because this member function hides the global open(). */
bool RGBLed::open(const std::string& path)
{
    fd = ::open(path.c_str(), O_WRONLY);
    if (fd < 0) {
        std::fprintf(stderr, "RGBLed: cannot open %s: %s\n", path.c_str(), std::strerror(errno));
        return false;
    }
    return true;
}

bool RGBLed::isReady() const
{
    return fd >= 0;
}

void RGBLed::changeColor(Color c)
{
    if(fd < 0){
        return;
    }
    
    if(c < COLOR_OFF || c > COLOR_BLUE){    // unknown value -> turn off
        c = COLOR_OFF; 
    }

    char value = static_cast<char>('0' + c);
    if (::write(fd, &value, 1) != 1) {
        std::fprintf(stderr, "RGBLed: write failed: %s\n", std::strerror(errno));
    }
}