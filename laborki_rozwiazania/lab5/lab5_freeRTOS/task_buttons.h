#ifndef TASK_BUTTONS_H
#define TASK_BUTTONS_H

#include "FreeRTOS.h"
#include "event_groups.h"


#define BTN0_PIN 16
#define BTN1_PIN 17
#define BTN2_PIN 18

#define BTN0_EVENT (1 << 0)
#define BTN1_EVENT (1 << 1)
#define BTN2_EVENT (1 << 2)


extern EventGroupHandle_t button_event_group;

void button_task(void *params);
void gpio_callback(uint gpio, uint32_t events);
void init_buttons(void);

#endif
