#ifndef LEDS_H
#define LEDS_H

#include <Adafruit_NeoPixel.h>

class LEDS
{

public:
    Adafruit_NeoPixel pixels;
    LEDS(uint8_t pin, uint16_t numPixels);
    void clear();
    void setPixelColor(uint16_t n, uint32_t color);
    void show();

private:
};

#endif
