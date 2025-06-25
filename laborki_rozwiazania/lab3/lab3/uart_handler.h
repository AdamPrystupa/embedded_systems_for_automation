#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#define UART_ID   uart0  // Ten sam, co w main
#define N_TERM_BUF 32    // Jeżeli chcesz w tym pliku używać

#define RS485_UART_ID     uart1
#define RS485_BAUD_RATE   115200
#define RS485_TX_PIN      4
#define RS485_RX_PIN      5
#define RS485_DATA_BITS 8
#define RS485_STOP_BITS 2
#define RS485_PARITY UART_PARITY_NONE
#define RS485_DIR_PIN 6
#define RS485_DIR_TX 1
#define RS485_DIR_RX 0  

#include "pico/stdlib.h"
#include "hardware/uart.h"

// Funkcja obsługi przerwania UART (RX)
void on_uart_rx(void);

// Inicjalizacja przerwania UART
void uart_irq_init(void);

// Ewentualnie funkcja do obsługi RS485
// void rs485_send_str(const char *src);

// void rs485_irq_init();
// void on_rs485_rx();

#endif
