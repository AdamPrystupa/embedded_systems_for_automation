#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "pico/binary_info.h"

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

// Set an LED_TYPE variable - 0 is default, 1 is connected to WIFI chip
// Note that LED_TYPE == 1 is only supported when initially compiled for
// a board with PICO_CYW43_SUPPORTED (eg pico_w), else the required
// libraries won't be present
bi_decl(bi_program_feature_group(0x1111, 0, "LED Configuration"));
#if defined(PICO_DEFAULT_LED_PIN)
    // the tag and id are not important as picotool filters based on the
    // variable name, so just set them to 0
    bi_decl(bi_ptr_int32(0x1111, 0, LED_TYPE, 0));
    bi_decl(bi_ptr_int32(0x1111, 0, LED_PIN, PICO_DEFAULT_LED_PIN));
#elif defined(CYW43_WL_GPIO_LED_PIN)
    bi_decl(bi_ptr_int32(0x1111, 0, LED_TYPE, 1));
    bi_decl(bi_ptr_int32(0x1111, 0, LED_PIN, CYW43_WL_GPIO_LED_PIN));
#else
    bi_decl(bi_ptr_int32(0x1111, 0, LED_TYPE, 0));
    bi_decl(bi_ptr_int32(0x1111, 0, LED_PIN, 25));
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 100
#endif

// Perform initialisation
int pico_led_init(void) {
    if (LED_TYPE == 0) {
        // A device like Pico that uses a GPIO for the LED so we can
        // use normal GPIO functionality to turn the led on and off
        gpio_init(LED_PIN);
        gpio_set_dir(LED_PIN, GPIO_OUT);
        return PICO_OK;
#ifdef CYW43_WL_GPIO_LED_PIN
    } else if (LED_TYPE == 1) {
        // For Pico W devices we need to initialise the driver etc
        return cyw43_arch_init();
#endif
    } else {
        return PICO_ERROR_INVALID_DATA;
    }
}

// Turn the led on or off
void pico_set_led(bool led_on) {
    if (LED_TYPE == 0) {
        // Just set the GPIO on or off
        gpio_put(LED_PIN, led_on);
#ifdef CYW43_WL_GPIO_LED_PIN
    } else if (LED_TYPE == 1) {
        // Ask the wifi "driver" to set the GPIO on or off
        cyw43_arch_gpio_put(LED_PIN, led_on);
#endif
    }
}



// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart0
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 0
#define UART_RX_PIN 1

int when_number(char buffer, char message[100]){
    int time = 0;
        time = (int) buffer - 48;
        int j=0;
        for(j=time; j>0; j--)
            {
                sprintf(message, "%d...\n", j);
                uart_puts(UART_ID, message);
                
                if(j>3)
                {
                    pico_set_led(true);
                    sleep_ms(500);
                    pico_set_led(false);
                    sleep_ms(500);
                }
                else
                {
                    int k=0;
                    for(k=0; k<10; k++)
                    {
                        pico_set_led(true);
                        sleep_ms(50);
                        pico_set_led(false);
                        sleep_ms(50);
                    }
            }
        }
        sprintf(message, "BOOM!\n");
        uart_puts(UART_ID, message);
return 0;
}


int when_letter(char buffer){
    buffer=buffer-32;
    uart_putc(UART_ID, buffer);
    return 0;
}


int main()
{
    stdio_init_all();
    char buffer=0;
    char message[100];
    int counter;
    int mode;
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    
    
    
    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART
    
    
    
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart
    
    while (true) {
        // Send out a string, with CR/LF conversions
        if (uart_is_readable(UART_ID)) {
            buffer = uart_getc(UART_ID);
            if(buffer>48 && buffer<58)
            {
                hw_clear_bits(&uart_get_hw(UART_ID)->cr, UART_UARTCR_RXE_BITS);
                when_number(buffer, message);
                hw_set_bits(&uart_get_hw(UART_ID)->cr, UART_UARTCR_RXE_BITS);
            }
            
            else           
            if(buffer>96 && buffer<123)
            {
                when_letter(buffer);
                   
            }
        }
        
                }
}