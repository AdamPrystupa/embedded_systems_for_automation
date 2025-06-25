#include "uart_terminal.h"
#include "commands.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <string.h>

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

char term_buf[N_TERM_BUF];
uint32_t term_idx = 0;

void process_command() {
    char *args[10];
    int arg_count = 0;

    char *token = strtok(term_buf, " ");
    while (token != NULL && arg_count < 10) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }

    if (arg_count == 0) {
        uart_puts(UART_ID, "No command received.\n");
    } else {
        for (int i = 0; commands[i].name != NULL; i++) {
            if (strcmp(args[0], commands[i].name) == 0) {
                if (arg_count - 1 >= commands[i].arg_count) {
                    commands[i].handler(&args[1]);
                } else {
                    uart_puts(UART_ID, "Error: Insufficient arguments.\n");
                }
                memset(term_buf, 0, sizeof(term_buf));
                term_idx = 0;
                return;
            }
        }
        uart_puts(UART_ID, "Unknown command.\n");
    }
    memset(term_buf, 0, sizeof(term_buf));
    term_idx = 0;
}

static void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        led_toggle();
        uint8_t ch = uart_getc(UART_ID);
        if(ch == '\r') {
            process_command();
            term_idx = 0;
            term_buf[term_idx] = '\0';
        } else {
            if(term_idx<N_TERM_BUF-1) {
                term_buf[term_idx++] = ch;
                term_buf[term_idx] = '\0';
            } else {
                ch = '~';
            }
        }
    }
}

void uart_terminal_init() {
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    int UARTx_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UARTx_IRQ, on_uart_rx);
    irq_set_enabled(UARTx_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);
}

void uart_puts(int uart_id, const char *str) {
    uart_write_blocking(uart_id, (const uint8_t *)str, strlen(str));
}