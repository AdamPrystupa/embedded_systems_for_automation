#pragma once

// Priorities of our threads - higher numbers are higher priority
#define MAIN_TASK_PRIORITY          ( tskIDLE_PRIORITY + 2UL )
#define BLINK_TASK_PRIORITY         ( tskIDLE_PRIORITY + 1UL )
#define WORKER_TASK_PRIORITY        ( tskIDLE_PRIORITY + 4UL )
#define DANCEFLOOR_TASK_PRIORITY    ( tskIDLE_PRIORITY + 3UL )
#define BUTTONS_TASK_PRIORITY       ( tskIDLE_PRIORITY + 3UL )
#define SENSORS_TASK_PRIORITY       ( tskIDLE_PRIORITY + 5UL )
#define UART_TASK_PRIORITY          ( tskIDLE_PRIORITY + 6UL )
#define CMD_TASK_PRIORITY           ( tskIDLE_PRIORITY + 1UL )
#define NRF24_TASK_PRIORITY         ( tskIDLE_PRIORITY + 2UL )

// Stack sizes of our threads in words (4 bytes)
#define MAIN_TASK_STACK_SIZE        configMINIMAL_STACK_SIZE
#define BLINK_TASK_STACK_SIZE       configMINIMAL_STACK_SIZE
#define WORKER_TASK_STACK_SIZE      configMINIMAL_STACK_SIZE
#define DANCEFLOOR_TASK_STACK_SIZE  configMINIMAL_STACK_SIZE
#define BUTTONS_TASK_STACK_SIZE     configMINIMAL_STACK_SIZE
#define SENSORS_TASK_STACK_SIZE     configMINIMAL_STACK_SIZE
#define UART_TASK_STACK_SIZE        configMINIMAL_STACK_SIZE
#define CMD_TASK_STACK_SIZE         configMINIMAL_STACK_SIZE
#define NRF24_TASK_STACK_SIZE       configMINIMAL_STACK_SIZE
