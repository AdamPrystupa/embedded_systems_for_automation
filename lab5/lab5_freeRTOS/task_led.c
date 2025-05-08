#include "task_led.h"
#include "task_buttons.h"
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

// Globalna zmienna przechowująca aktywną diodę LED
volatile uint8_t active_led = 0xFF; // 0xFF oznacza brak aktywnej diody

void led_task(void *params) {
    while (true) {
        // Sprawdź, która dioda LED jest aktywna
        if (active_led == 0) {
            gpio_put(LED0_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(500));
            gpio_put(LED0_PIN, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
        } else if (active_led == 1) {
            gpio_put(LED1_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(500));
            gpio_put(LED1_PIN, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
        } else if (active_led == 2) {
            gpio_put(LED2_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(500));
            gpio_put(LED2_PIN, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
        } else {
            // Jeśli żadna dioda nie jest aktywna, czekaj na zmianę
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

void init_leds(void) {
    gpio_init(LED0_PIN);
    gpio_set_dir(LED0_PIN, GPIO_OUT);
    gpio_put(LED0_PIN, 0);

    gpio_init(LED1_PIN);
    gpio_set_dir(LED1_PIN, GPIO_OUT);
    gpio_put(LED1_PIN, 0);

    gpio_init(LED2_PIN);
    gpio_set_dir(LED2_PIN, GPIO_OUT);
    gpio_put(LED2_PIN, 0);
}
