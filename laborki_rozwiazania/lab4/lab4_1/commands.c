#include "commands.h"
#include "hardware/pio.h"
#include "hardware/watchdog.h"
#include "leds.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "ssd.h"
#include "ws2812.h"

char term_buf[N_TERM_BUF];
uint32_t term_idx = 0;
volatile bool uart_flag = false;
char oled_input[256] = {0};

// Tablica dostępnych komend
const terminal_command commands[] = {
    {"help", 0, cmd_help, "Display this help message"},
    {"reboot", 0, cmd_reboot, "Reboot the system"},
    {"uptime", 0, cmd_uptime, "Display system uptime"},
    {"toggle", 0, cmd_toggle, "Toggle the LED"},
    {"led", 2, cmd_led, "Set LED state: led <led_num> <0/1>"},
    {"leds", 1, cmd_leds, "Set LEDs state using bitmask: leds <bitmask>"},
    {"pwm", 2, cmd_pwm, "Set PWM LED and filling: pwm <led> <filling>"},
    {"oled", 1, cmd_oled,
     "Display text on OLED: oled <text1|text2|text3|text4>"},
    {"blink", 3, cmd_blink, "Blink the LED: blink <times> <on_ms> <off_ms>"},
    {"ws", 2, cmd_ws, "Set WS2812 LED: ws <led_num 0..15> <RRGGBB> (hex)"},
    {"df", 1, cmd_df,
     "Set all WS2812 LEDs: df <RRGGBB...> (16xRRGGBB hex string)"},
    {NULL, 0, NULL, NULL}};

// ------ Implementacje komend ------
void cmd_help(char **args) {
  uart_puts(UART_ID, "Available commands:\n");
  for (int i = 0; commands[i].name != NULL; i++) {
    char buf[100];
    sprintf(buf, "- %s: %s\n", commands[i].name, commands[i].description);
    uart_puts(UART_ID, buf);
  }
}

void cmd_reboot(char **args) {
  uart_puts(UART_ID, "Rebooting...\n");
  sleep_ms(1000);
  watchdog_reboot(0, 0, 0);
}

void cmd_uptime(char **args) {
  uint32_t uptime = to_ms_since_boot(get_absolute_time());
  char buf[100];
  sprintf(buf, "System uptime: %u ms\n", uptime);
  uart_puts(UART_ID, buf);
}

void cmd_toggle(char **args) {
  led_toggle();
  uart_puts(UART_ID, "LED toggled.\n");
}

void cmd_blink(char **args) {
  if (args[0] && args[1] && args[2]) {
    int times = atoi(args[0]);
    int on_ms = atoi(args[1]);
    int off_ms = atoi(args[2]);
    if (times > 0 && on_ms > 0 && off_ms > 0) {
      char buf[100];
      sprintf(buf, "Blinking LED %d times: %dms ON, %dms OFF\n", times, on_ms,
              off_ms);
      uart_puts(UART_ID, buf);
      for (int i = 0; i < times; i++) {
        led_toggle();
        sleep_ms(on_ms);
        led_toggle();
        sleep_ms(off_ms);
      }
      uart_puts(UART_ID, "Blinking complete.\n");
    } else {
      uart_puts(UART_ID, "Error: Arguments must be positive integers.\n");
    }
  } else {
    uart_puts(
        UART_ID,
        "Error: Missing arguments. Usage: blink <times> <on_ms> <off_ms>\n");
  }
}

void cmd_led(char **args) {
  if (args[0] && args[1]) {
    int led_num = atoi(args[0]);
    int state = atoi(args[1]);
    if (led_num >= 0 && led_num < 3) {
      gpio_put(16 + led_num, state);
      char buf[100];
      sprintf(buf, "LED %d turned %s\n", led_num, state ? "on" : "off");
      uart_puts(UART_ID, buf);
    } else {
      uart_puts(UART_ID, "Error: LED number should be 0..2\n");
    }
  } else {
    uart_puts(UART_ID,
              "Error: Missing arguments. Usage: led <led_num> <0/1>\n");
  }
}

void cmd_leds(char **args) {
  if (args[0]) {
    int bitmask = atoi(args[0]);
    for (int i = 0; i < 3; i++) {
      gpio_put(16 + i, (bitmask >> i) & 1);
    }
    char buf[100];
    sprintf(buf, "LEDs set to bitmask: %d\n", bitmask);
    uart_puts(UART_ID, buf);
  } else {
    uart_puts(UART_ID, "Error: Missing argument. Usage: leds <bitmask>\n");
  }
}

void cmd_pwm(char **args) {
  if (args[0] && args[1]) {
    int led_num = atoi(args[0]);
    int filling = atoi(args[1]);
    if (filling < 0 || filling > 100) {
      uart_puts(UART_ID, "Error: Filling must be between 0 and 100.\n");
      return;
    }
    for (int i = 0; i < 1000; i++) {
      gpio_put(16 + led_num, 1);
      sleep_us(filling * 10);
      gpio_put(16 + led_num, 0);
      sleep_us((100 - filling) * 10);
    }
    char buf[100];
    sprintf(buf, "LED %d software PWM set to %d\n", led_num, filling);
    uart_puts(UART_ID, buf);
  }
}

void cmd_oled(char **args) {
  if (args[0] == NULL) {
    uart_puts(UART_ID, "Użycie: oled tekst1|tekst2|tekst3|tekst4\n");
    return;
  }
  char *tokens[4] = {NULL, NULL, NULL, NULL};
  int token_count = 0;
  static char input_copy[256];
  strncpy(input_copy, args[0], sizeof(input_copy) - 1);
  input_copy[sizeof(input_copy) - 1] = '\0';
  char *token = strtok(input_copy, "|");
  while (token != NULL && token_count < 4) {
    tokens[token_count++] = token;
    token = strtok(NULL, "|");
  }
  display_text(tokens);
}

#define PIO_USED pio0
extern int8_t ws2812_sm;
extern rgb_t pixels[4][4];
extern volatile uint8_t flag_ws2812_display_pixels;

void cmd_ws(char **args) {
  // Nowa wersja zgodna z wymaganiami użytkownika
  if (!args[0] || !args[1]) {
    uart_puts(UART_ID, "\r\nERROR: ws takes 2 arguments\r\n");
    return;
  }
  int n = atoi(args[0]);
  if (n < 0 || n > 15) {
    uart_puts(UART_ID, "\r\nERROR: ws invalid LED_NUM\r\n");
    return;
  }
  if (strlen(args[1]) != 6) {
    uart_puts(UART_ID, "\r\nERROR: ws invalid RRGGBB value\r\n");
    return;
  }
  int r = 0, g = 0, b = 0;
  if (sscanf(args[1], "%2x%2x%2x", &r, &g, &b) != 3) {
    uart_puts(UART_ID, "\r\nERROR: ws invalid RRGGBB format\r\n");
    return;
  }
  pixels[n / 4][n % 4].red = r;
  pixels[n / 4][n % 4].green = g;
  pixels[n / 4][n % 4].blue = b;
  flag_ws2812_display_pixels = 1;
}

void cmd_df(char **args) {
  // Nowa wersja zgodna z wymaganiami użytkownika
  if (!args[0]) {
    uart_puts(UART_ID, "\r\nERROR: df takes 1 argument\r\n");
    return;
  }
  if (strlen(args[0]) != 96) {
    uart_puts(UART_ID, "\r\nERROR: df invalid RRGGBB val\r\n");
    return;
  }
  char holder[7];
  holder[6] = '\0';
  for (uint8_t i = 0; i < 16; ++i) {
    memcpy(holder, &args[0][i * 6], 6);
    int r = 0, g = 0, b = 0;
    if (sscanf(holder, "%2x%2x%2x", &r, &g, &b) != 3) {
      uart_puts(UART_ID, "\r\nERROR: df invalid RRGGBB format\r\n");
      return;
    }
    pixels[i / 4][i % 4].red = r;
    pixels[i / 4][i % 4].green = g;
    pixels[i / 4][i % 4].blue = b;
  }
  flag_ws2812_display_pixels = 1;
}

// ------ Główna funkcja przetwarzająca wpisany tekst ------
void process_command() {
  char *args[32];
  int arg_count = 0;
  char *token = strtok(term_buf, " ");
  while (token != NULL && arg_count < 32) {
    args[arg_count++] = token;
    token = strtok(NULL, " ");
  }
  if (arg_count == 0) {
    uart_puts(UART_ID, "No command received.\n");
  } else {
    for (int i = 0; commands[i].name != NULL; i++) {
      if (strcmp(args[0], commands[i].name) == 0) {
        if (arg_count - 1 >= commands[i].arg_count) {
          commands[i].handler(&args[1]);
        } else {
          uart_puts(UART_ID, "Error: Insufficient arguments.\n");
        }
        memset(term_buf, 0, sizeof(term_buf));
        term_idx = 0;
        return;
      }
    }
    uart_puts(UART_ID, "Unknown command.\n");
  }
  memset(term_buf, 0, sizeof(term_buf));
  term_idx = 0;
}