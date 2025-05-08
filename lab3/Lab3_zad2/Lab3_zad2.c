#include <stdio.h>
#include <stdlib.h> 
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"
#include "hardware/watchdog.h"
#include "pico/bootrom.h" 
#include "pico/util/datetime.h"
#include "hardware/i2c.h"
#include "ssd1306_font.h"






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

typedef struct {
    int16_t year;    ///< 0..4095
    int8_t month;    ///< 1..12, 1 is January
    int8_t day;      ///< 1..28,29,30,31 depending on month
    int8_t dotw;     ///< 0..6, 0 is Sunday
    int8_t hour;     ///< 0..23
    int8_t min;      ///< 0..59
    int8_t sec;      ///< 0..59
} datetime_t;


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


void cmd_help(char **args);
void cmd_reboot(char **args);
void cmd_uptime(char **args);
void cmd_toggle(char **args);
void cmd_blink(char **args);
void cmd_led(char **args);
void cmd_leds(char **args);
// void cmd_settime(char **args);
// void cmd_time(char **args);
void cmd_pwm(char **args);

const terminal_command commands[] = {
    { "help", 0, cmd_help, "Display this help message" },
    {"reboot", 0, cmd_reboot, "Reboot the system" },
    { "uptime", 0, cmd_uptime, "Display system uptime" },
    { "toggle", 0, cmd_toggle, "Toggle the LED" },
    { "led", 2, cmd_led, "Set LED state: led <on/off>" },
    { "leds", 1, cmd_leds, "Set LEDs state using bitmask: leds <bitmask>" },
    // { "settime", 6, cmd_settime, "Set the system time: settime <year> <month> <day> <dotw> <hour> <min>" },
    // { "time", 0, cmd_time, "Display the current system time" },
    { "pwm", 2, cmd_pwm, "Set PWM LED and filling: pwm <led> <filling>" },
    // { "oled", 1, cmd_oled, "Display message on OLED: oled <message>" },
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

void cmd_reboot(char **args) {
    uart_puts(UART_ID, "Rebooting...\n");
    sleep_ms(1000); 
    watchdog_reboot(0,0,0);
}

void cmd_uptime(char **args) {
    uint32_t uptime = to_ms_since_boot(get_absolute_time());
    char buf[100];
    sprintf(buf, "System uptime: %d ms\n", uptime);
    uart_puts(UART_ID, buf);
}

void cmd_led(char **args) {
    if (args[0] && args[1]) {
        int led_num = atoi(args[0]);
        int state = atoi(args[1]);
        
        if (led_num >= 1 && led_num < 4) {
            gpio_put(19 + led_num, state);
            char buf[100];
            sprintf(buf, "LED %d turned %s\n", led_num, state ? "on" : "off");
            uart_puts(UART_ID, buf);
        } 
        }    
    } 

void cmd_leds(char **args) {
    if (args[0]) {
        int bitmask = atoi(args[0]);
        for (int i = 1; i < 4; i++) {
            gpio_put(19 + i, (bitmask >> i) & 1);
        }
        char buf[100];
        sprintf(buf, "LEDs set to %d\n", bitmask);
        uart_puts(UART_ID, buf);
    } 
}

// void cmd_settime(char **args){
//     if (args[0] && args[1] && args[2] && args[3] && args[4] && args[5]) {
//         datetime_t dt;
//         dt.year = atoi(args[0]);
//         dt.month = atoi(args[1]);
//         dt.day = atoi(args[2]);
//         dt.dotw = atoi(args[3]);
//         dt.hour = atoi(args[4]);
//         dt.min = atoi(args[5]);
//         dt.sec = 0;

//         datetime_to_rtc(&dt);
//         char buf[100];
//         sprintf(buf, "Time set to: %04d-%02d-%02d %02d:%02d:%02d\n", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
//         uart_puts(UART_ID, buf);
//     } else {
//         uart_puts(UART_ID, "Error: Missing arguments. Usage: settime <year> <month> <day> <dotw> <hour> <min>\n");
//     }
// }

// void cmd_time(char **args){
//     datetime_t dt;
//     rtc_get_datetime(&dt);
//     char buf[100];
//     sprintf(buf, "Current time: %04d-%02d-%02d %02d:%02d:%02d\n", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
//     uart_puts(UART_ID, buf);
// }

void cmd_pwm(char **args) {
    if (args[0] && args[1]) {
        int led_num = atoi(args[0]);
        int filling = atoi(args[1]);

        if (led_num >= 1 && led_num < 4 && filling >= 0 && filling <= 100) {
            char buf[100];
            sprintf(buf, "Setting PWM on LED %d with %d%% filling\n", led_num, filling);
            uart_puts(UART_ID, buf);

            int on_time = filling * 10; // Convert percentage to milliseconds (assuming 10ms cycle)
            int off_time = 1000 - on_time;

            for (int i = 0; i < 100; i++) { // Run for 1 second (100 cycles of 10ms)
                gpio_put(19 + led_num, 1);
                sleep_us(on_time * 10); // Convert ms to us
                gpio_put(19 + led_num, 0);
                sleep_us(off_time * 10);
            }
        } else {
            uart_puts(UART_ID, "Error: Invalid LED number or filling percentage.\n");
        }
    } else {
        uart_puts(UART_ID, "Error: Missing arguments. Usage: pwm <led> <filling>\n");
    }
}

// void cmd_oled(char **args) {

// }

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
    } else {
        for (int i = 0; commands[i].name != NULL; i++) {
            if (strcmp(args[0], commands[i].name) == 0) {
                if (arg_count - 1 >= commands[i].arg_count) {
                    commands[i].handler(&args[1]);
                } else {
                    uart_puts(UART_ID, "Error: Insufficient arguments.\n");
                }    

                // Clear the buffer after processing
                memset(term_buf, 0, sizeof(term_buf));
                term_idx = 0;
                return;
            }    
        }    
        uart_puts(UART_ID, "Unknown command.\n");
    }    

    // Clear the buffer if no valid command was found
    memset(term_buf, 0, sizeof(term_buf));
    term_idx = 0;
}    

// RX interrupt handler

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
        
        // Clear the terminal buffer and reset the index
        memset(term_buf, 0, sizeof(term_buf));
        term_idx = 0;
    
        // UART initialization
        uart_init(UART_ID, BAUD_RATE);
        init_leds();
       // rtc_init();
        sleep_ms(1); 
        while (uart_is_readable(UART_ID)) {
            uart_getc(UART_ID); 
        }
        uart_irq_init();
        gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
        gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

        while (1) {
            // Main loop
        }
    }
