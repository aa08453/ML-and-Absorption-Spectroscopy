#include <stdio.h>
#include <zephyr/kernel.h>


#include "addr.h"

typedef enum { OFF, ON } state_t;

typedef struct {
    const struct device* sensor;   // which I2C bus
    uint16_t channel_readings[12];          // raw channel values
    volatile state_t led_state;
} as7341_t;

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

int init(as7341_t* as7341);

void set_ATIME(as7341_t* as7341, uint8_t value);
uint8_t get_ATIME(as7341_t* as7341);

void set_ASTEP(as7341_t* as7341,uint16_t value);
uint16_t get_ASTEP(as7341_t* as7341);

void set_GAIN(as7341_t* as7341, gain_t gain);
gain_t get_GAIN(as7341_t* as7341);

void enable_LED(as7341_t* as7341, state_t state);

void set_LED_current(as7341_t* as7341, int current);

void read_all_channels(as7341_t* as7341);
uint16_t get_channel(as7341_t* as7341, color_channel_t channel);

