#include "leds.h"
#include "pico/cyw43_arch.h" // Potrzebne do cyw43_arch_gpio_put(), jeśli korzystasz z wbudowanej LED Wi-Fi

void led_toggle() {
  static int toggle = 0;
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, toggle);
  toggle = !toggle;
}

void init_leds() {
  // Przykład inicjalizacji GPIO dla diod fizycznych na pinach 16, 17, 18:
  gpio_init(16);
  gpio_set_dir(16, GPIO_OUT);
  gpio_put(16, 0);

  gpio_init(17);
  gpio_set_dir(17, GPIO_OUT);
  gpio_put(17, 0);

  gpio_init(18);
  gpio_set_dir(18, GPIO_OUT);
  gpio_put(18, 0);
}
