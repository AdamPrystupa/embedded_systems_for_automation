#ifndef LEDS_H
#define LEDS_H

#include "pico/stdlib.h"

/**
 * Inicjuje wyjścia GPIO dla diod LED.
 */
void init_leds(void);

/**
 * Przełącza stan diody LED w module Wi-Fi (lub innej zdefiniowanej).
 * Przykładowo dioda wbudowana w moduł Wi-Fi na Raspberry Pi Pico W.
 */
void led_toggle(void);

#endif
