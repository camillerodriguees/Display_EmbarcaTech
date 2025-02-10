#ifndef WS2812_H
#define WS2812_H

#include "pico/stdlib.h"

void ws2812_init(uint pin);
void ws2812_display_number(uint pin, int number);
void ws2812_set_pixel(uint pin, uint index, uint8_t red, uint8_t green, uint8_t blue);
void ws2812_show();

#endif // WS2812_H
