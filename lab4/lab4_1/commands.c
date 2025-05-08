#include "commands.h"
#include "hardware/watchdog.h"   // do watchdog_reboot
#include "pico/cyw43_arch.h"     // do cyw43_arch_gpio_put
#include "leds.h"   
#include "ssd.h"             // aby móc wywołać led_toggle(), init_leds() itp.

// Bufor i indeks muszą mieć 'definition' w .c, bo w .h jest tylko extern:
char term_buf[N_TERM_BUF];
uint32_t term_idx = 0;

volatile bool uart_flag = false; 
char oled_input[256]= {0};

// Tablica dostępnych komend
const terminal_command commands[] = {
    { "help",   0, cmd_help,   "Display this help message" },
    { "reboot", 0, cmd_reboot, "Reboot the system" },
    { "uptime", 0, cmd_uptime, "Display system uptime" },
    { "toggle", 0, cmd_toggle, "Toggle the LED" },
    { "led",    2, cmd_led,    "Set LED state: led <led_num> <0/1>" },
    { "leds",   1, cmd_leds,   "Set LEDs state using bitmask: leds <bitmask>" },
    // { "settime", 6, cmd_settime, "Set the system time: settime <year> <month> <day> <dotw> <hour> <min>" },
    // { "time", 0, cmd_time, "Display the current system time" },
    { "pwm", 2, cmd_pwm, "Set PWM LED and filling: pwm <led> <filling>" },
    { "oled", 1, cmd_oled, "Display text on OLED: oled <text1|text2|text3|text4>" },
    { "blink",  3, cmd_blink,  "Blink the LED: blink <times> <on_ms> <off_ms>" },
    // {"ws", 2, cmd_ws, "Set WS2812 LED: ws <led_num> <color>"},
    // {"df", 0, cmd_df, "Set all WS2812 LEDs: df <color> ... <color> (n=16)"},
    { NULL, 0, NULL, NULL }
};

// ------ Implementacje komend ------
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
    watchdog_reboot(0, 0, 0);
}

void cmd_uptime(char **args) {
    uint32_t uptime = to_ms_since_boot(get_absolute_time());
    char buf[100];
    sprintf(buf, "System uptime: %u ms\n", uptime);
    uart_puts(UART_ID, buf);
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

void cmd_led(char **args) {
    if (args[0] && args[1]) {
        int led_num = atoi(args[0]);
        int state   = atoi(args[1]);
        
        if (led_num >= 0 && led_num < 3) {
            // Przykładowo: piny 20,21,22 -> led_num = 1..3 => pin = 19 + led_num
            gpio_put(20 + led_num, state);
            char buf[100];
            sprintf(buf, "LED %d turned %s\n", led_num, state ? "on" : "off");
            uart_puts(UART_ID, buf);
        } else {
            uart_puts(UART_ID, "Error: LED number should be 1..3\n");
        }
    } else {
        uart_puts(UART_ID, "Error: Missing arguments. Usage: led <led_num> <0/1>\n");
    }
}

void cmd_leds(char **args) {
    if (args[0]) {
        int bitmask = atoi(args[0]);
        for (int i = 0; i < 3; i++) {
            // i = 1..3 => piny 20..22 => (bitmask >> (i-1)) & 1
            gpio_put(20 + i, (bitmask >> (i )) & 1);
        }
        char buf[100];
        sprintf(buf, "LEDs set to bitmask: %d\n", bitmask);
        uart_puts(UART_ID, buf);
    } else {
        uart_puts(UART_ID, "Error: Missing argument. Usage: leds <bitmask>\n");
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
        if (filling < 0 || filling > 100) { 
            uart_puts(UART_ID, "Error: Filling must be between 0 and 100.\n");
            return;
        }

        for (int i = 0; i < 1000; i++) { 
            gpio_put(20 + led_num, 1); 
            sleep_us(filling * 10);   
            gpio_put(20 + led_num, 0); 
            sleep_us((100 - filling) * 10);
        }


        char buf[100];
        sprintf(buf, "LED %d software PWM set to %d\n", led_num, filling);
        uart_puts(UART_ID, buf);
    }
}


void cmd_oled(char **args) {
    if (args[0] == NULL) {
        printf("Użycie: oled tekst1|tekst2|tekst3|tekst4\n");
        return;
    }

    char *tokens[4] = {NULL, NULL, NULL, NULL}; 
    int token_count = 0;

    
    static char input_copy[256]; 
    strncpy(input_copy, args[0], sizeof(input_copy) - 1);
    input_copy[sizeof(input_copy) - 1] = '\0'; 

    char *token = strtok(input_copy, "|");
    while (token != NULL && token_count < 4) {
        tokens[token_count++] = token; 
        token = strtok(NULL, "|");
    }
    
    display_text(tokens);
}




// ------ Główna funkcja przetwarzająca wpisany tekst ------
void process_command() {
    // Rozdziel tekst z term_buf na tokeny (komenda + argumenty)
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
        // Wyszukaj w tablicy commands
        for (int i = 0; commands[i].name != NULL; i++) {
            if (strcmp(args[0], commands[i].name) == 0) {
                // Sprawdź liczbę argumentów
                if (arg_count - 1 >= commands[i].arg_count) {
                    commands[i].handler(&args[1]);
                } else {
                    uart_puts(UART_ID, "Error: Insufficient arguments.\n");
                }
                // Wyzeruj bufor po przetworzeniu
                memset(term_buf, 0, sizeof(term_buf));
                term_idx = 0;
                return;
            }
        }
        uart_puts(UART_ID, "Unknown command.\n");
    }

    // Gdy brak trafionego polecenia:
    memset(term_buf, 0, sizeof(term_buf));
    term_idx = 0;
}

// ------ RS485 ------
// void process_command_rs485() {

//     char *args[10];
//     int arg_count = 0;
    
//     char *token = strtok(rs485_buf, " ");
//     while (token != NULL && arg_count < 10) {
//         args[arg_count++] = token;
//         token = strtok(NULL, " ");
//     }

// if (arg_count == 0) {
//         uart_puts(UART_ID, "No command received.\n");
//     } else {

//         for (int i = 0; commands[i].name != NULL; i++) {
//             if (strcmp(args[0], commands[i].name) == 0) {
    
//                 if (arg_count - 1 >= commands[i].arg_count) {
//                     commands[i].handler(&args[1]);
//                 } else {
//                     uart_puts(UART_ID, "Error: Insufficient arguments.\n");
//                 }
  
//                 memset(term_buf, 0, sizeof(term_buf));
//                 term_idx = 0;
//                 return;
//             }
        
//     }

//     rs485_send_str(response_buffer);
// }
// }