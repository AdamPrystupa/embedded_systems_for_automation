#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"

#define RS485_UART_ID uart1
#define RS485_BAUD_RATE 115200
#define RS485_TX_PIN 4
#define RS485_RX_PIN 5
#define RS485_DATA_BITS 8
#define RS485_STOP_BITS 2
#define RS485_PARITY UART_PARITY_NONE
#define RS485_DIR_PIN 6
#define RS485_DIR_TX 1
#define RS485_DIR_RX 0  

#define N_TERM_BUF 32 // Intentionally that short
char term_buf[N_TERM_BUF]; // Here we collect the received data
uint32_t term_idx = 0; // term_buf cursor - current position



// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

void led_toggle() {
    static int toggle = 0;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, toggle);
    toggle = !toggle;
}    

void process_command() {
}


// RX interrupt handler

void on_uart_rx() {
    while (uart_is_readable(RS485_UART_ID)) {
        led_toggle(); // Let’s have some fun
        uint8_t ch = uart_getc(RS485_UART_ID);
        if(ch == '\r') {
            process_command();
            term_idx = 0; // Prepare for another command reception
            term_buf[term_idx] = '\0';  // even empty C string must end with zero (\0)
        } else {
            if(term_idx<N_TERM_BUF-1) {
                term_buf[term_idx++] = ch;
                term_buf[term_idx] = '\0'; // ASCIIZ - C string must end with zero (\0)
            } else {
                ch = '~'; // Notify user: term buffer is full
            }
            // if (term_echo && uart_is_writable(RS485_UART_ID)) {
                //     uart_write_blocking(RS485_UART_ID, (const uint8_t *) &ch, 1); // Send it back
                //     uart_tx_wait_blocking(RS485_UART_ID); //wait fifo empty (even we don’t use it)
                // }
            }
        }
    }
    // Set up UART
    void rs485_init(uart_inst_t *uart, uint baudrate, 
        uint tx_pin, uint rx_pin, 
        uint data_bits, uint stop_bits, uart_parity_t parity) {
            uart_init(uart, baudrate);
        gpio_set_function(tx_pin, GPIO_FUNC_UART);
        gpio_set_function(rx_pin, GPIO_FUNC_UART);
        uart_set_hw_flow(uart, false, false); // flow control CTS/RTS, we don't want these
        uart_set_format(uart, data_bits, stop_bits, parity); // Set our data format
        uart_set_fifo_enabled(uart, false); // Turn off FIFO's - we want to do this char by char
        int UARTx_IRQ = uart == uart0 ? UART0_IRQ : UART1_IRQ; // Select interrupt for the UART
        irq_set_exclusive_handler(UARTx_IRQ, on_uart_rx); // Set up and enable the interrupt handlers
        irq_set_enabled(UARTx_IRQ, true);
        uart_set_irq_enables(uart, true, false); // Now enable the UART to send interrupts - RX only
    }
    
    
    
    
    
    void rs485_send_str(const char *src) {
        size_t len = strlen(src);
        gpio_put(RS485_DIR_PIN, RS485_DIR_TX); // Set RS485 transceiver to transmit mode
        uart_write_blocking(RS485_UART_ID, (const uint8_t *)src, len);
        uart_tx_wait_blocking(RS485_UART_ID); //wait for fifo empty (even though we don’t use fifo)
        gpio_put(RS485_DIR_PIN, RS485_DIR_RX); // Set RS485 transceiver to receive mode
    }

        
        int main() {
            stdio_init_all();
            if (cyw43_arch_init()) { // Initialise the Wi-Fi chip
        printf("Wi-Fi init failed\n");
        return -1;
    }
    rs485_init(RS485_UART_ID, RS485_BAUD_RATE, 
		  RS485_TX_PIN, RS485_RX_PIN, 
		  RS485_DATA_BITS, RS485_STOP_BITS, RS485_PARITY);   

          char uart_buffer[128];
          int index = 0;
    int32_t cnt = 0;
    while (true) {
        static char buf[100];
        sprintf(buf, "%d 275496\n", cnt++);
        led_toggle();
        rs485_send_str(buf);
        sleep_ms(5000);
    }


    
}
