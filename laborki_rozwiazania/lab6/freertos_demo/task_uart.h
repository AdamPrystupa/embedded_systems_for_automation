#pragma once
#include "FreeRTOS.h"
#include "queue.h"

void task_uart(void *params);
void uart_queue_init(void);
QueueHandle_t get_uart_line_queue(void);
