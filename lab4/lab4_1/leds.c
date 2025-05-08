#include "leds.h"
#include "pico/cyw43_arch.h"  // Potrzebne do cyw43_arch_gpio_put(), jeśli korzystasz z wbudowanej LED Wi-Fi

void led_toggle() {
    static int toggle = 0;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, toggle);
    toggle = !toggle;
}

void init_leds() {
    // Przykład inicjalizacji GPIO dla diod fizycznych na pinach 20, 21, 22:
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
