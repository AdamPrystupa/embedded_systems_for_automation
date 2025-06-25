#pragma once
#include "common.h"

#define NUM_BUTTONS 3

#define BTN0_PIN    16
#define BTN1_PIN    17
#define BTN2_PIN    18

#define BTN_PRESSED  1
#define BTN_RELEASED 0

extern volatile bool btn_state[NUM_BUTTONS];
extern const uint btn_event_bits[NUM_BUTTONS];

void buttons_task(__unused void *params);
