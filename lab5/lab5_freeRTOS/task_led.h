#ifndef TASK_LED_H
#define TASK_LED_H

#include "FreeRTOS.h"
#include "event_groups.h"

#define LED0_PIN 20
#define LED1_PIN 21
#define LED2_PIN 22

void init_leds(void);
void led_task(void *params);

#endif
