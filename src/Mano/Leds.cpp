#include "Leds.h"

LEDS::LEDS(uint8_t pin, uint16_t numPixels) : pixels(numPixels, pin, NEO_GRB + NEO_KHZ800)
{
    pixels.begin();
}

void LEDS::clear()
{
    pixels.clear();
}

void LEDS::setPixelColor(uint16_t n, uint32_t color)
{
    pixels.setPixelColor(n, color);
}

void LEDS::show()
{
    pixels.show();
}
