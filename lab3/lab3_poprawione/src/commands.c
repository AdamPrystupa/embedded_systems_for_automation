#include "commands.h"
#include "hardware.h"
#include "uart_terminal.h"
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

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

// ... (pozostałe funkcje komend)