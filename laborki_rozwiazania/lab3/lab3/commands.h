#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Do strtok, strcmp itp.
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "pico/util/datetime.h"


// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart0
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// --- Definicje, które były w głównym pliku ---
#define N_TERM_BUF 32
extern char term_buf[N_TERM_BUF]; 
extern uint32_t term_idx;

// Możesz też przenieść tutaj definicje, np. RS485_* itp., jeśli używasz ich w commands.c

// Struktura polecenia:
typedef struct {
    const char *name;                // Nazwa polecenia
    int arg_count;                   // Minimalna liczba argumentów
    void (*handler)(char **args);    // Wskaźnik na funkcję obsługującą polecenie
    const char *description;         // Opis
} terminal_command;

// Deklaracje funkcji obsługujących poszczególne polecenia
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
void cmd_oled(char **args);

// Tablica zarejestrowanych poleceń (będzie zdefiniowana w commands.c)
extern const terminal_command commands[];
// extern char rs485_buf[N_TERM_BUF];
// extern uint32_t rs485_idx;

/**
 * Funkcja analizuje tekst wpisany w `term_buf`, wyszukuje odpowiadające mu polecenie 
 * w tablicy `commands[]` i wywołuje odpowiedni handler.
 */
void process_command(void);

#endif
