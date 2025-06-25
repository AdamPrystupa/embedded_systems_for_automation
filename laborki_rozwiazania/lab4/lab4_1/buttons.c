#include "buttons.h"

void init_buttons() {
  // Przykład inicjalizacji GPIO dla przycisków fizycznych na pinach 19, 20, 21:
  gpio_init(19);
  gpio_set_dir(19, GPIO_IN);
  gpio_pull_up(19);

  gpio_init(20);
  gpio_set_dir(20, GPIO_IN);
  gpio_pull_up(20);

  gpio_init(21);
  gpio_set_dir(21, GPIO_IN);
  gpio_pull_up(21);
}
