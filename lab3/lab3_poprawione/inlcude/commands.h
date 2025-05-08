#pragma once

typedef struct {
    const char *name;       
    int arg_count;          
    void (*handler)(char **args); 
    const char *description; 
} terminal_command;

extern const terminal_command commands[];

void cmd_help(char **args);
void cmd_reboot(char **args);
void cmd_uptime(char **args);
void cmd_toggle(char **args);
void cmd_blink(char **args);
void cmd_led(char **args);
void cmd_leds(char **args);