#ifndef WS2812_H
#define WS2812_H

#include "hardware/pio.h"

#define WS2812_PIN 15

typedef struct rgb_t {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} rgb_t;

extern rgb_t pixels[4][4];

extern volatile uint8_t flag_ws2812_display_pixels;

void set_pixel(PIO pio, int8_t state_machine, rgb_t color);
void set_pixels(PIO pio, uint8_t state_machine, rgb_t colors[4][4]);
void ws2812_init(PIO pio, int8_t *state_machine, uint8_t data_pin,
                 bool reverse_signal);
void ws2812_dim(rgb_t colors[4][4], uint8_t ratio);

#endif