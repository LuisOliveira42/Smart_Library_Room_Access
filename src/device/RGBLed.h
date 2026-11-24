#ifndef DEVICE_RGBLED_H
#define DEVICE_RGBLED_H
 
#include <string>
#include "Types.h"
 
/* Controls the RGB LED through the /dev/rgbled driver (rgbled.ko).
 * The driver takes one character: '0'=OFF '1'=RED '2'=GREEN '3'=BLUE,
 * which is exactly the numeric value of the Color enum. */
class RGBLed {
public:
    explicit RGBLed(const std::string& path);       // opens the device
    ~RGBLed();                                      // LED off + closes the device
 
    RGBLed(const RGBLed&) = delete;
    RGBLed& operator=(const RGBLed&) = delete;
 
    bool isReady() const;
    void changeColor(Color c);
 
private:
    int fd;
    bool open(const std::string& path);
};
 
#endif