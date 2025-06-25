#include "task_buttons.h"
#include "task_led.h" // Dodano, aby używać active_led
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include <stdio.h>
#include "task_led.h"

// Deklaracja globalnej zmiennej active_led
extern volatile uint8_t active_led;

EventGroupHandle_t button_event_group;

void gpio_callback(uint gpio, uint32_t events) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Zmień aktywną diodę LED na podstawie wciśniętego przycisku
    if (gpio == BTN0_PIN) {
        active_led = 0; // Ustaw LED0 jako aktywną
    } else if (gpio == BTN1_PIN) {
        active_led = 1; // Ustaw LED1 jako aktywną
    } else if (gpio == BTN2_PIN) {
        active_led = 2; // Ustaw LED2 jako aktywną
    }

    // Wyczyść wszystkie flagi zdarzeń
    xEventGroupClearBitsFromISR(button_event_group, BTN0_EVENT | BTN1_EVENT | BTN2_EVENT);

    // Ustaw odpowiednią flagę zdarzenia
    if (gpio == BTN0_PIN) {
        xEventGroupSetBitsFromISR(button_event_group, BTN0_EVENT, &xHigherPriorityTaskWoken);
    } else if (gpio == BTN1_PIN) {
        xEventGroupSetBitsFromISR(button_event_group, BTN1_EVENT, &xHigherPriorityTaskWoken);
    } else if (gpio == BTN2_PIN) {
        xEventGroupSetBitsFromISR(button_event_group, BTN2_EVENT, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void button_task(void *params) {
    while (true) {
        EventBits_t events = xEventGroupWaitBits(
            button_event_group,
            BTN0_EVENT | BTN1_EVENT | BTN2_EVENT,
            pdTRUE,  
            pdFALSE, 
            portMAX_DELAY
        );

        if (events & BTN0_EVENT) {
            printf("BTN0 pressed or released\n");
        }
        if (events & BTN1_EVENT) {
            printf("BTN1 pressed or released\n");
        }
        if (events & BTN2_EVENT) {
            printf("BTN2 pressed or released\n");
        }
    }
}

void init_buttons(void) {
    button_event_group = xEventGroupCreate();

    gpio_init(BTN0_PIN);
    gpio_set_dir(BTN0_PIN, GPIO_IN);
    gpio_pull_up(BTN0_PIN);
    gpio_set_irq_enabled_with_callback(BTN0_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(BTN1_PIN);
    gpio_set_dir(BTN1_PIN, GPIO_IN);
    gpio_pull_up(BTN1_PIN);
    gpio_set_irq_enabled(BTN1_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    gpio_init(BTN2_PIN);
    gpio_set_dir(BTN2_PIN, GPIO_IN);
    gpio_pull_up(BTN2_PIN);
    gpio_set_irq_enabled(BTN2_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}
