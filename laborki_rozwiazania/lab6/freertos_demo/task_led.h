#pragma once
#include "common.h"

// Whether to busy wait in the led thread
#ifndef LED_BUSY_WAIT
#define LED_BUSY_WAIT 0
#endif
 
// Delay between led blinking
#define LED_DELAY_MS 500

#define NUM_LEDS    3
#define LED0_PIN    19
#define LED1_PIN    20
#define LED2_PIN    21

#define LED_ON      1
#define LED_OFF     0

void blink_task(__unused void *params);
