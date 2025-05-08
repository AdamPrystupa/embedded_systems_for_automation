#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware.h"
#include "uart_terminal.h"
#include "rs485.h"

int main() {
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    init_leds();
    uart_terminal_init();
    rs485_init();

    while (1) {
        // Main loop
    }
}