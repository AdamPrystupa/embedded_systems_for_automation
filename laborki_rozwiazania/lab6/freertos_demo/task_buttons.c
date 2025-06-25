#include "task_buttons.h"
#include "task_uart.h"
#include <stdbool.h>
#include <stdio.h>


volatile bool btn_state[NUM_BUTTONS];

static volatile bool btn_changed_state = false;
const static uint btn_pins[] = {BTN0_PIN, BTN1_PIN, BTN2_PIN};
const uint btn_event_bits[] = {EV_BTN0, EV_BTN1, EV_BTN2};

static void btnIrqCallback(uint _gpioNum, uint32_t _events) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  gpio_acknowledge_irq(_gpioNum, _events);
  int i = -1;
  switch (_gpioNum) {
  case BTN0_PIN:
    i = 0;
    break;
  case BTN1_PIN:
    i = 1;
    break;
  case BTN2_PIN:
    i = 2;
    break;
  }
  if (i >= 0) {
    xEventGroupSetBitsFromISR(xISREventGroup, btn_event_bits[i],
                              &xHigherPriorityTaskWoken);
    btn_changed_state = true;
  }
  for (uint i = 0; i < NUM_BUTTONS; ++i) {
    btn_state[i] = gpio_get(btn_pins[i]) == 0 ? BTN_PRESSED : BTN_RELEASED;
  }
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void buttons_init(void) {
  for (uint i = 0; i < NUM_BUTTONS; ++i) {
    gpio_init(btn_pins[i]);
    gpio_set_dir(btn_pins[i], GPIO_IN);
    gpio_pull_up(btn_pins[i]);
    btn_state[i] = gpio_get(btn_pins[i]);
    gpio_set_input_enabled(btn_pins[i], true);
    gpio_set_irq_enabled_with_callback(btn_pins[i],
                                       GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
                                       true, btnIrqCallback);
  }
}

void buttons_task(__unused void *params) {
  printf("buttons_task starts\n");
  buttons_init();

  // naive approach:
  /*while (true) {
      if (btn_changed_state) {
          btn_changed_state = false; //possible problem!
          printf("button states: %d %d %d\n", btn_state[0], btn_state[1],
  btn_state[2]);
      }
      sleep_ms(1); //1: the loop will execute 1000 times a second,
      sleep_ms(100); //100: Many things can happen during sleep – any button can
  be pressed or released many times, but we don’t notice it.
  }*/

  while (true) {
    // This function stops and waits for an event to occur.
    EventBits_t evBits = xEventGroupWaitBits(
        xISREventGroup,
        EV_BTN0 | EV_BTN1 | EV_BTN2 |
            EV_UART_CMD, // the bits within the event group to wait for
        pdTRUE,          // clear bits before returning
        pdFALSE,         // don't wait for all bits, any bit will do
        portMAX_DELAY);  // wait forever

    if (evBits &
        (EV_BTN0 | EV_BTN1 | EV_BTN2)) // btn 0, 1, or 2 has changed its state
    {
      printf("GPIO IRQ has fired. Button states during IRQ: %d %d %d\n",
             btn_state[0], btn_state[1], btn_state[2]);
    }
    if (evBits & EV_UART_CMD) {
      // Only notify, do not consume the queue here
      printf("UART CMD event!\n");
    }
  }
}
