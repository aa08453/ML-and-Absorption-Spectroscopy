#include <stdio.h>
#include <zephyr/kernel.h>


#include "addr.h"


typedef enum { OFF, ON } state_t;

typedef enum 
{
  F1_415nm,
  F2_445nm,
  F3_480nm,
  F4_515nm,
  CLEAR_0,
  NIR_0,
  F5_555nm,
  F6_590nm,
  F7_630nm,
  F8_680nm,
  CLEAR,
  NIR,
} color_channel_t;

typedef enum {
  GAIN_0_5X,
  GAIN_1X,
  GAIN_2X,
  GAIN_4X,
  GAIN_8X,
  GAIN_16X,
  GAIN_32X,
  GAIN_64X,
  GAIN_128X,
  GAIN_256X,
  GAIN_512X,
} gain_t;

int init(const struct device* as7341);


void set_ATIME(uint8_t value);
uint8_t get_ATIME();

void set_ASTEP(uint16_t value);
uint16_t get_ASTEP();

void set_GAIN(gain_t gain);
gain_t get_GAIN();

void enable_LED(state_t state);

void set_LED_current(int current);
