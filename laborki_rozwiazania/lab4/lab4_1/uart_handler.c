#include "uart_handler.h"
#include "commands.h"    
#include "leds.h"       



void on_uart_rx() {
    while (uart_is_readable(uart0)) {
        char ch = uart_getc(uart0); 
        if (ch == '\n' || ch == '\r') {
            term_buf[term_idx] = '\0'; 
            uart_flag = true;         
        } else {

            if (term_idx < sizeof(term_buf) - 1) {
                term_buf[term_idx++] = ch;
            } else {
                uart_puts(UART_ID, "Error: Input too long.\n");
                memset(term_buf, 0, sizeof(term_buf));
                term_idx = 0;
            }
        }
    }
}
void uart_irq_init() {
    int UARTx_IRQ = (UART_ID == uart0) ? UART0_IRQ : UART1_IRQ;

    irq_set_exclusive_handler(UARTx_IRQ, on_uart_rx);
    irq_set_enabled(UARTx_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false); // RX = on, TX = off
}



// void rs485_send_str(const char *src) {
//     size_t len = strlen(src);
//     gpio_put(RS485_DIR_PIN, RS485_DIR_TX);
//     uart_write_blocking(RS485_UART_ID, (const uint8_t *)src, len);
//     uart_tx_wait_blocking(RS485_UART_ID);
//     gpio_put(RS485_DIR_PIN, RS485_DIR_RX);
// }

// // Bufor dla RS485
// char rs485_buf[N_TERM_BUF];
// uint32_t rs485_idx = 0;

// // Obsługa przerwań RS485
// void on_rs485_rx() {
//     while (uart_is_readable(RS485_UART_ID)) {
//         uint8_t ch = uart_getc(RS485_UART_ID);
//         if (ch == '\r') {
//             // Skopiuj bufor do głównego term_buf i przetwórz
//             memcpy(term_buf, rs485_buf, N_TERM_BUF);
//             term_idx = rs485_idx;
//             process_command_rs485();
//             memset(rs485_buf, 0, sizeof(rs485_buf));
//             rs485_idx = 0;
//         } else {
//             if (rs485_idx < N_TERM_BUF - 1) {
//                 rs485_buf[rs485_idx++] = ch;
//                 rs485_buf[rs485_idx] = '\0';
//             }
//         }
//     }
// }

// void rs485_irq_init() {
//     irq_set_exclusive_handler(UART1_IRQ, on_rs485_rx);
//     irq_set_enabled(UART1_IRQ, true);
//     uart_set_irq_enables(RS485_UART_ID, true, false);
// }
