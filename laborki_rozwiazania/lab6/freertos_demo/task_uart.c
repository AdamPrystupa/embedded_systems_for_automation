#include "task_uart.h"
#include "FreeRTOS.h"
#include "common.h"
#include "pico/stdlib.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define UART_LINE_MAX_LEN 64
#define UART_QUEUE_LEN 4

static QueueHandle_t uart_line_queue = NULL;

static char bufA[UART_LINE_MAX_LEN];
static char bufB[UART_LINE_MAX_LEN];
static volatile char *active_buf = bufA;
static volatile char *ready_buf = bufB;
static volatile int buf_pos = 0;

static void swap_buffers(void) {
  char *tmp = (char *)active_buf;
  active_buf = ready_buf;
  ready_buf = tmp;
  buf_pos = 0;
}

void on_uart_rx() {
  static char isr_line[UART_LINE_MAX_LEN];
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  while (uart_is_readable(UART_ID)) {
    char c = uart_getc(UART_ID);
    if (c == '\r' || c == '\n' || buf_pos >= UART_LINE_MAX_LEN - 1) {
      if (buf_pos > 0) { // Only send non-empty lines
        active_buf[buf_pos] = '\0';
        taskENTER_CRITICAL();
        strncpy(isr_line, (const char *)active_buf, UART_LINE_MAX_LEN);
        swap_buffers();
        taskEXIT_CRITICAL();
        xQueueSendFromISR(uart_line_queue, isr_line, NULL);
      }
      xEventGroupSetBitsFromISR(xISREventGroup, EV_UART_CMD,
                                &xHigherPriorityTaskWoken);
      buf_pos = 0;
    } else {
      active_buf[buf_pos++] = c;
    }
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void uart_irq_handler() { on_uart_rx(); }

void uart_hw_init() {
  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
  uart_set_hw_flow(UART_ID, false, false);
  uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
  uart_set_fifo_enabled(UART_ID, true);
  irq_set_exclusive_handler(UART0_IRQ, uart_irq_handler);
  irq_set_enabled(UART0_IRQ, true);
  uart_set_irq_enables(UART_ID, true, false);
}

void task_uart(__unused void *params) {
  uart_hw_init();
  printf("UART task started\n");
  char line[UART_LINE_MAX_LEN];
  while (1) {
    if (xQueueReceive(uart_line_queue, line, portMAX_DELAY) == pdTRUE) {
      printf("Received line: %s\n", line);
    }
  }
}

QueueHandle_t get_uart_line_queue() { return uart_line_queue; }

void uart_queue_init() {
  uart_line_queue = xQueueCreate(UART_QUEUE_LEN, UART_LINE_MAX_LEN);
}
