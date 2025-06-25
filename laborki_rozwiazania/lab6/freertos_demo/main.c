#include "common.h"
#include "task_async.h"
#include "task_buttons.h"
#include "task_led.h"
#include "task_uart.h"
#include "tasks_config.h"

#include <stdlib.h>

void task_cmd(__unused void *params) {
  QueueHandle_t uart_queue = get_uart_line_queue();
  char line[64];
  while (1) {
    if (xQueueReceive(uart_queue, &line, portMAX_DELAY) == pdTRUE) {
      printf("CMD: %s\n", line);
    }
  }
}

void main_task(__unused void *params) {
  async_context_t *context = example_async_context();
  async_context_add_at_time_worker_in_ms(context, &worker_timeout, 0);

  xISREventGroup = xEventGroupCreate();

  xTaskCreate(blink_task, "Blink", BLINK_TASK_STACK_SIZE, NULL,
              BLINK_TASK_PRIORITY, NULL);
  xTaskCreate(buttons_task, "Buttons", BUTTONS_TASK_STACK_SIZE, NULL,
              BUTTONS_TASK_PRIORITY, NULL);
  uart_queue_init();
  xTaskCreate(task_uart, "UART", 512, NULL, 2, NULL);
  xTaskCreate(task_cmd, "CMD", 512, NULL, 2, NULL);

  int count = 0;
  while (true) {
    static int last_core_id = -1;
    if (portGET_CORE_ID() != last_core_id) {
      last_core_id = portGET_CORE_ID();
      printf("main task is on core %d\n", last_core_id);
    }
    printf("Hello from main task count=%u\n", count);
    vTaskDelay(1000);
    ++count;
  }
  async_context_deinit(context);
}

void vLaunch(void) {
  TaskHandle_t task;
  xTaskCreate(main_task, "MainThread", MAIN_TASK_STACK_SIZE, NULL,
              MAIN_TASK_PRIORITY, &task);

  // we must bind the main task to one core (well at least while the init is
  // called)
  vTaskCoreAffinitySet(task, 1);

  // Start the tasks and timer running.
  vTaskStartScheduler();
}

int main(void) {
  stdio_init_all();
  vLaunch();
  return 0;
}