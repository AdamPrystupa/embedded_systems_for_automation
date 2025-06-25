#pragma once

#include "FreeRTOS.h"
#include "event_groups.h"
#include "pico/async_context_freertos.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "task.h"
#include <stdio.h>


extern int32_t global_x;
extern EventGroupHandle_t xISREventGroup;

#define EV_BTN0 (1 << 0)
#define EV_BTN1 (1 << 1)
#define EV_BTN2 (1 << 2)
#define EV_UART_CMD (1 << 3)

uint8_t dec2hex(uint8_t d);
uint8_t hex2dec(char h);
int8_t ishex(char c);
int8_t isdec(char c);
int8_t isletter(char c);
int16_t kju_atoi(char *s);
int16_t process_arg(char **s);
int16_t process_hex_arg(char **s);