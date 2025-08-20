#include <stdio.h>
#include <zephyr/kernel.h>

#include "../inc/func.h"

#define SHIFT 4
#define NUM_SAMPLES SHIFT*SHIFT

static as7341_t sensor1 = {.sensor = DEVICE_DT_GET(DT_ALIAS(i2c))};

static uint16_t readings[12]; 

int main(void)
{
	// k_msleep(3);

	if (init(&sensor1) < 0)
		return 0;


	// set_LED_current(40);
	set_GAIN(&sensor1, GAIN_128X);
	set_ASTEP(&sensor1, 499);
	set_ATIME(&sensor1, 74);

	int num_samples = 0;

	while (1)
	{
        enable_LED(&sensor1, OFF);
		read_all_channels(&sensor1);  // fills global buffer

		// printk("Channel Readings: ");
		if (num_samples < NUM_SAMPLES)
		{
			for (int k = 0; k < 12; k++)
				readings[k] += get_channel(&sensor1, k);
			num_samples += 1;
		}

		else
		{
			for (int i = 0; i < 4; ++i)
			{
				printk("%u\t", (readings[i] >> SHIFT));
				readings[i] = 0;
			}
			for (int i = 6; i < 12; ++i)
			{
				printk("%u\t", (readings[i] >> SHIFT));
				readings[i] = 0;
			}
			

			printk("\n");
			num_samples = 0;

		}	

		k_msleep(100);  // adjust based on your measurement interval
	}
}
