#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "uart_handler.h"
#include "leds.h"
#include "commands.h"
#include "buttons.h"
#include "ssd.h"



int main() {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Zeruj bufor poleceń
    memset(term_buf, 0, sizeof(term_buf));
    term_idx = 0;

    // 1. Inicjalizacja UART do obsługi terminala
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART); // TX
    gpio_set_function(1, GPIO_FUNC_UART); // RX

    // 2. Inicjalizacja przerwania UART/RS4
    // uart_irq_init();
    // rs485_irq_init();

    // 3. Inicjalizacja LED-ów
    init_leds();

    // 4. Inicjalizacja przycisków
    init_buttons();

    // 5. nicjalizacja RS485, np.:
    // uart_init(RS485_UART_ID, RS485_BAUD_RATE);
    // gpio_set_function(RS485_TX_PIN, GPIO_FUNC_UART);
    // gpio_set_function(RS485_RX_PIN, GPIO_FUNC_UART);
    // gpio_init(6);
    // gpio_set_dir(6, GPIO_OUT);
    // gpio_put(6, RS485_DIR_RX);

    // Pętla główna
    while (1) {

    }
    return 0;
}
