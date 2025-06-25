#include "buttons.h"



void init_buttons() {
    // Przykład inicjalizacji GPIO dla przycisków fizycznych na pinach 16, 17, 18:
    gpio_init(16);
    gpio_set_dir(16, GPIO_IN);
    gpio_pull_up(16);

    gpio_init(17);
    gpio_set_dir(17, GPIO_IN);
    gpio_pull_up(17); 

    gpio_init(18);
    gpio_set_dir(18, GPIO_IN);
    gpio_pull_up(18);
}
