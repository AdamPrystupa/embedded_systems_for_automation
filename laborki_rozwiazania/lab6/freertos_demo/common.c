#include "common.h"

int32_t global_x;

EventGroupHandle_t xISREventGroup = NULL;

uint8_t dec2hex(uint8_t d)
{
  if(d <= 9) return (uint8_t)(d + '0');
  if(d >= 10 && d <= 15) return (uint8_t)(d - 10 + 'A');
  return (uint8_t)'?';
}

uint8_t hex2dec(char h)
{
  if(h >= '0' && h <= '9') return (uint8_t)(h - '0');
  if(h >= 'A' && h <= 'F') return (uint8_t)(h - 'A' + 10);
  if(h >= 'a' && h <= 'f') return (uint8_t)(h - 'a' + 10);
  return 0;
}
int8_t ishex(char c)
{
  return (c>='0' && c<='9') || (c>='A' && c<='F') || (c>='a' && c<='f');
}
int8_t isdec(char c)
{
  return (c>='0' && c<='9');
}

int8_t isletter(char c)
{
  return ((c>='a' && c<='z') || (c>='A' && c<='Z'));
}

int16_t kju_atoi(char * s)
{
  int result = 0;
  while(isdec(*s))
  {
    result = result * 10 + *s - '0';
    s++;
  }
  return result;
}

int16_t process_arg(char ** s)
{
  int16_t result = 0;
  while(!isdec(**s))(*s)++;
  while(isdec(**s))
  {
    result = result * 10 + **s - '0';
    (*s)++;
  }
  return result;
}

int16_t process_hex_arg(char ** s)
{
  int16_t result = 0;
  while(!ishex(**s))(*s)++;
  while(ishex(**s))
  {
    result = (result <<4) | hex2dec(**s);
    (*s)++;
  }
  return result;
}
