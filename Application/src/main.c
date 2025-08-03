#include <stdio.h>
#include <zephyr/kernel.h>


#include "../inc/func.h"

const struct device *as7341 = DEVICE_DT_GET(DT_ALIAS(i2c));


int main(void)
{
	k_msleep(3);

	if (init(as7341) < 0)
		return 0;


	// set_LED_current(40);

	while (1)
	{
        // enable_LED(ON);
		read_all_channels();  // fills global buffer

		// printk("Channel Readings: ");
		// for (int i = 0; i < 12; ++i)
		// 	printk("%u\t", get_channel(i));  // or use channel_readings[i]
		// printk("\n");

		k_msleep(100);  // adjust based on your measurement interval
	}
}
