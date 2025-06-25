#include "ws2812.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "ws2812.pio.h"
#include "ws2812_reversed.pio.h"
#include <stdio.h>
#include <stdlib.h>

rgb_t pixels[4][4] = {{0, 0, 0}};
volatile uint8_t flag_ws2812_display_pixels = 0;

void set_pixel(PIO pio, int8_t state_machine, rgb_t color) {
  uint32_t grb = ((color.green & 0xFF) << 16) | ((color.red & 0xFF) << 8) |
                 (color.blue & 0xFF);
  pio_sm_put_blocking(pio, state_machine, grb);
}

void set_pixels(PIO pio, uint8_t state_machine, rgb_t colors[4][4]) {
  for (int i = 0; i < 16; ++i) {
    int y = i / 4;
    int x = i % 4;
    set_pixel(pio, state_machine, colors[y][x]);
  }
  sleep_us(80); // reset sygnału WS2812B
}

void ws2812_init(PIO pio, int8_t *state_machine, uint8_t data_pin,
                 bool reverse_signal) {
  *state_machine =
      pio_claim_unused_sm(pio, true); // Pobierz wolną maszynę stanową

  // Załaduj program PIO do pamięci PIO
  int32_t offset;
  pio_sm_config c;

  if (reverse_signal) {
    offset = pio_add_program(pio, &ws2812_reversed_program);
    c = ws2812_reversed_program_get_default_config(offset);
  } else {
    offset = pio_add_program(pio, &ws2812_program);
    c = ws2812_program_get_default_config(offset);
  }

  sm_config_set_sideset_pins(&c, data_pin); // Ustawienie pinu wyjściowego
  sm_config_set_out_shift(
      &c, false, true,
      24); // Przesuwanie danych bit po bicie, autopull włączony
  // sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX); // Połączenie FIFO TX
  sm_config_set_clkdiv(&c, 7.5f);

  pio_gpio_init(pio, data_pin); // Inicjalizacja pinu GPIO
  pio_sm_set_consecutive_pindirs(pio, *state_machine, data_pin, 1,
                                 true); // Ustawienie jako wyjście

  pio_sm_init(pio, *state_machine, offset,
              &c); // Inicjalizacja maszyny stanowej
  pio_sm_set_enabled(pio, *state_machine, true); // Uruchomienie maszyny PIO
}

void ws2812_dim(rgb_t colors[4][4], uint8_t ratio) {
  for (uint8_t row = 0; row < 4; ++row)
    for (uint8_t col = 0; col < 4; ++col) {
      colors[row][col].red = colors[row][col].red / ratio;
      colors[row][col].green = colors[row][col].green / ratio;
      colors[row][col].blue = colors[row][col].blue / ratio;
    }
}