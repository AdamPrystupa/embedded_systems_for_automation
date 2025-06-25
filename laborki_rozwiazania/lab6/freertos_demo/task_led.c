#include "task_led.h"
#include "task_buttons.h"

static uint current_led = 0;
static const uint led_pins[] = { LED0_PIN, LED1_PIN, LED2_PIN };

static void set_current_led(bool state) {
    gpio_put(led_pins[current_led], state);
}

static void set_leds(bool state) {
    for(int i=0; i<NUM_LEDS; ++i)
        gpio_put(led_pins[i], state);
}

static void leds_init(void) {
    for (uint i = 0; i < NUM_LEDS; ++i) {
        gpio_init(led_pins[i]);
        gpio_set_dir(led_pins[i], GPIO_OUT);
        gpio_put(led_pins[i], LED_OFF);
    }
}

void blink_task(__unused void *params) {
    bool state = false;
    printf("blink_task starts\n");
    leds_init();
    const TickType_t xTicksToWait = LED_DELAY_MS / portTICK_PERIOD_MS;

    while (true) {
        EventBits_t	evBits =  xEventGroupWaitBits( xISREventGroup, 
            EV_BTN0|EV_BTN1|EV_BTN2,    //the bits within the event group to wait for
            pdTRUE,                     //clear bits before returning
            pdFALSE,                    //don't wait for all bits, any bit will do
            xTicksToWait );

        if (evBits & (EV_BTN0|EV_BTN1|EV_BTN2)) //btn 0, 1, or 2 has changed state
        {
            int i = 0; 
            if (evBits & EV_BTN0) i = 0;
            if (evBits & EV_BTN1) i = 1;
            if (evBits & EV_BTN2) i = 2;
            if (btn_state[i] == BTN_PRESSED) {
                current_led = i;
                set_leds(LED_OFF);
                state = LED_ON;
            }
        }

        set_current_led(state);
        state = !state;
    }
}