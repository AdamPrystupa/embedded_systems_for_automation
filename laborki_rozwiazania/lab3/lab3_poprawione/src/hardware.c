#include "hardware.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

void led_toggle() {
    static int toggle = 0;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, toggle);
    toggle = !toggle;
}    

void init_leds() {
    gpio_init(20);
    gpio_set_dir(20, GPIO_OUT);
    gpio_put(20, 0); 

    gpio_init(21);
    gpio_set_dir(21, GPIO_OUT);
    gpio_put(21, 0); 

    gpio_init(22);
    gpio_set_dir(22, GPIO_OUT);
    gpio_put(22, 0); 
}