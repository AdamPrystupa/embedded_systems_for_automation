#include <stdio.h>
#include <stdlib.h> 
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
#define UART_ID uart0
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 0
#define UART_RX_PIN 1

typedef struct {
    const char *name;       
    int arg_count;          
    void (*handler)(char **args); 
    const char *description; 
} terminal_command;

void led_toggle() {
    static int toggle = 0;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, toggle);
    toggle = !toggle;
}    


void cmd_help(char **args);
void cmd_toggle(char **args);
void cmd_blink(char **args);

const terminal_command commands[] = {
    { "help", 0, cmd_help, "Display this help message" },
    { "toggle", 0, cmd_toggle, "Toggle the LED" },
    { "blink", 3, cmd_blink, "Blink the LED: blink <times> <on_ms> <off_ms>" },
    { NULL, 0, NULL, NULL } 
};

void cmd_help(char **args) {
    uart_puts(UART_ID, "Available commands:\n");
    for (int i = 0; commands[i].name != NULL; i++) {
        char buf[100];
        sprintf(buf, "- %s: %s\n", commands[i].name, commands[i].description);
        uart_puts(UART_ID, buf);
    }
}

void cmd_toggle(char **args) {
    led_toggle();
    uart_puts(UART_ID, "LED toggled.\n");
}

void cmd_blink(char **args) {
    if (args[0] && args[1] && args[2]) {
        int times = atoi(args[0]);
        int on_ms = atoi(args[1]);
        int off_ms = atoi(args[2]);
        
        if (times > 0 && on_ms > 0 && off_ms > 0) {
            char buf[100];
            sprintf(buf, "Blinking LED %d times: %dms ON, %dms OFF\n", times, on_ms, off_ms);
            uart_puts(UART_ID, buf);
            
            for (int i = 0; i < times; i++) {
                led_toggle();
                sleep_ms(on_ms);
                led_toggle();
                sleep_ms(off_ms);
            }
            
            uart_puts(UART_ID, "Blinking complete.\n");
        } else {
            uart_puts(UART_ID, "Error: Arguments must be positive integers.\n");
        }
    } else {
        uart_puts(UART_ID, "Error: Missing arguments. Usage: blink <times> <on_ms> <off_ms>\n");
    }
}

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
        return;
    }
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(args[0], commands[i].name) == 0) {
            if (arg_count - 1 >= commands[i].arg_count) {
                commands[i].handler(&args[1]);
            } else {
                uart_puts(UART_ID, "Error: Insufficient arguments.\n");
            }
            return;
        }
    }
    
    uart_puts(UART_ID, "Unknown command.\n");
}


// RX interrupt handler

// RS485
// void on_uart_rx() {
//     while (uart_is_readable(RS485_UART_ID)) {
//         led_toggle(); // Let’s have some fun
//         uint8_t ch = uart_getc(RS485_UART_ID);
//         if(ch == '\r') {
//             process_command();
//             term_idx = 0; // Prepare for another command reception
//             term_buf[term_idx] = '\0';  // even empty C string must end with zero (\0)
//         } else {
//             if(term_idx<N_TERM_BUF-1) {
//                 term_buf[term_idx++] = ch;
//                 term_buf[term_idx] = '\0'; // ASCIIZ - C string must end with zero (\0)
//             } else {
//                 ch = '~'; // Notify user: term buffer is full
//             }
//             // if (term_echo && uart_is_writable(RS485_UART_ID)) {
//                 //     uart_write_blocking(RS485_UART_ID, (const uint8_t *) &ch, 1); // Send it back
//                 //     uart_tx_wait_blocking(RS485_UART_ID); //wait fifo empty (even we don’t use it)
//                 // }
//             }
//         }
//     }

// Set up RS485
// void rs485_init(uart_inst_t *uart, uint baudrate, 
//     uint tx_pin, uint rx_pin, 
//     uint data_bits, uint stop_bits, uart_parity_t parity) {
//         uart_init(uart, baudrate);
//     gpio_set_function(tx_pin, GPIO_FUNC_UART);
//     gpio_set_function(rx_pin, GPIO_FUNC_UART);
//     uart_set_hw_flow(uart, false, false); // flow control CTS/RTS, we don't want these
//     uart_set_format(uart, data_bits, stop_bits, parity); // Set our data format
//     uart_set_fifo_enabled(uart, false); // Turn off FIFO's - we want to do this char by char
//     int UARTx_IRQ = uart == uart0 ? UART0_IRQ : UART1_IRQ; // Select interrupt for the UART
//     irq_set_exclusive_handler(UARTx_IRQ, on_uart_rx); // Set up and enable the interrupt handlers
//     irq_set_enabled(UARTx_IRQ, true);
//     uart_set_irq_enables(uart, true, false); // Now enable the UART to send interrupts - RX only
// }

// UART

    void on_uart_rx() {
        while (uart_is_readable(UART_ID)) {
            led_toggle(); // Let’s have some fun
            uint8_t ch = uart_getc(UART_ID);
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
                // if (term_echo && uart_is_writable(UART_ID)) {
                    //     uart_write_blocking(UART_ID, (const uint8_t *) &ch, 1); // Send it back
                    //     uart_tx_wait_blocking(UART_ID); //wait fifo empty (even we don’t use it)
                    // }
                }
            }
        }
        
        void uart_irq_init(){
            int UARTx_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
            irq_set_exclusive_handler(UARTx_IRQ, on_uart_rx); // Set the interrupt handler
            irq_set_enabled(UARTx_IRQ, true);                // Enable the interrupt
            uart_set_irq_enables(UART_ID, true, false);      // Enable RX interrupt only
    }
            
    
    
  // Set up UART  
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
    // rs485_init(RS485_UART_ID, RS485_BAUD_RATE, 
	// 	  RS485_TX_PIN, RS485_RX_PIN, 
	// 	  RS485_DATA_BITS, RS485_STOP_BITS, RS485_PARITY);   
    
    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    uart_irq_init();
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART
    


    
    
    while (1) {
        
    }

    
}
