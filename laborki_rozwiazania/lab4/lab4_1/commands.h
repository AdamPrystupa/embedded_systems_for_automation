#ifndef COMMANDS_H
#define COMMANDS_H

#include "hardware/uart.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define N_TERM_BUF 128
extern char term_buf[N_TERM_BUF];
extern uint32_t term_idx;
extern volatile bool uart_flag;
extern char oled_input[256];

// Struktura polecenia
typedef struct {
  const char *name;
  int arg_count;
  void (*handler)(char **args);
  const char *description;
} terminal_command;

// Deklaracje funkcji obsługujących polecenia
void cmd_help(char **args);
void cmd_reboot(char **args);
void cmd_uptime(char **args);
void cmd_toggle(char **args);
void cmd_blink(char **args);
void cmd_led(char **args);
void cmd_leds(char **args);
void cmd_pwm(char **args);
void cmd_oled(char **args);
void cmd_ws(char **args);
void cmd_df(char **args);

extern const terminal_command commands[];

void process_command(void);

#endif
