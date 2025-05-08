#pragma once

#define N_TERM_BUF 32

void uart_terminal_init(void);
void process_command(void);
void uart_puts(int uart_id, const char *str);