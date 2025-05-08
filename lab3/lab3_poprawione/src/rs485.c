#include "rs485.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define RS485_UART_ID uart1
#define RS485_BAUD_RATE 115200
#define RS485_TX_PIN 4
#define RS485_RX_PIN 5
#define RS485_DIR_PIN 6
#define RS485_DIR_TX 1
#define RS485_DIR_RX 0  

void rs485_init() {
    uart_init(RS485_UART_ID, RS485_BAUD_RATE);
    gpio_set_function(RS485_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RS485_RX_PIN, GPIO_FUNC_UART);
    gpio_init(RS485_DIR_PIN);
    gpio_set_dir(RS485_DIR_PIN, GPIO_OUT);
    gpio_put(RS485_DIR_PIN, RS485_DIR_RX);
}

void rs485_send_str(const char *src) {
    size_t len = strlen(src);
    gpio_put(RS485_DIR_PIN, RS485_DIR_TX);
    uart_write_blocking(RS485_UART_ID, (const uint8_t *)src, len);
    uart_tx_wait_blocking(RS485_UART_ID);
    gpio_put(RS485_DIR_PIN, RS485_DIR_RX);
}