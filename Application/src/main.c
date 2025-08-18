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

		printk("Channel Readings: ");
		for (int i = 0; i < 12; ++i)
			printk("%u\t", get_channel(i));  // or use channel_readings[i]
		printk("\n");

		k_msleep(100);  // adjust based on your measurement interval
	}
}

//=========================================================================

// #include <stdio.h>
// #include <zephyr/kernel.h>
// #include <zephyr/drivers/i2c.h>

// #include "../inc/addr.h"
// // #include "../inc/func.h"

// const struct device *as7341 = DEVICE_DT_GET(DT_ALIAS(i2c));

// uint8_t bitmask(int bits, int position) 
// {
//     return ((1U << bits) - 1) << position;
// }

// // uint8_t bitvalue(int )

// int init(const struct device* as7341)
// {
//     sensor = as7341;
// 	if (sensor == NULL || !device_is_ready(sensor))
// 	{
// 		printk("No device\n");
// 		return -1;
// 	} 
//     if (bus_scan(sensor) < 0) return -1;
// 	printk("Sensor initialized\n");
//     return 0;
// }



// int bus_scan(const struct device* sensor)
// {
//     // printk("Scanning I2C bus...\n");
//     for (uint8_t addr = 0x03; addr < 0x78; addr++) 
//     {
//         uint8_t dummy;
//         int ret = i2c_read(sensor, &dummy, 1, addr);
//         if (ret == 0)
//         {
//             // printk("Device found at 0x%02X\n", addr);
//             return 0;
//         }
//     }
//     printk("Device not found.\n");
//     return -1;
// }

// // void enable_PON()
// // {
// // 	// 0000 0001 
// // 	uint8_t PON = bitmask(1,0);
// // 	int ret = i2c_reg_update_byte(as7341, ADDR, ENABLE, PON, 0x01);
// // 	printk("Ret = %d\tPON = 0x%02X\n", ret, PON);
// // }

// void SINT_SMUX()
// {
// 	uint8_t SIEN_SMUX = bitmask(1,4); // 0001 0000, to write = 0001 0000
// 	int ret = i2c_reg_update_byte(as7341, ADDR, CFG9, SIEN_SMUX, 0x10);
// 	printk("Ret = %d\tSIEN_SMUX = 0x%02X\n", ret, SIEN_SMUX);
// }

// void SINT_SIEN()
// {
// 	uint8_t SIEN = bitmask(1,0); // 0000 0001, to write = 0000 0001
// 	int ret = i2c_reg_update_byte(as7341, ADDR, INTENAB, SIEN, 0x01);
// 	printk("Ret = %d\tSIEN = 0x%02X\n", ret, SIEN);	
// }

// void SMUX_Config_RAM()
// {
// 	uint8_t SMUX_CMD = bitmask(2,3); // 0001 1000, to write = 0001 0000
// 	int ret = i2c_reg_update_byte(as7341, ADDR, CFG6, SMUX_CMD, 0x10);
// 	printk("Ret = %d\tSMUX_CMD = 0x%02X\n", ret, SMUX_CMD);
// }

// void write_reg(uint8_t reg, uint8_t value)
// {
// 	int ret = i2c_reg_write_byte(as7341, ADDR, reg, value);
// 	printk("Ret = %d\treg = 0x%02X\tvalue = 0x%02X\n", ret, reg, value);
// }


// void SMUX_Map1()
// {
// 	write_reg(0x00, 0x30);
// 	write_reg(0x01, 0x01);
// 	write_reg(0x02, 0x00);
// 	write_reg(0x03, 0x00);
// 	write_reg(0x04, 0x00);
// 	write_reg(0x05, 0x42);
// 	write_reg(0x06, 0x00);
// 	write_reg(0x07, 0x00);
// 	write_reg(0x08, 0x50);
// 	write_reg(0x09, 0x00);
// 	write_reg(0x0A, 0x00);
// 	write_reg(0x0B, 0x00);
// 	write_reg(0x0C, 0x20);
// 	write_reg(0x0D, 0x04);
// 	write_reg(0x0E, 0x00);
// 	write_reg(0x0F, 0x30);
// 	write_reg(0x10, 0x01);
// 	write_reg(0x11, 0x50);
// 	write_reg(0x12, 0x00);
// 	write_reg(0x13, 0x06);
// }

// void SMUX_Map2()
// {
// 	write_reg(0x00, 0x00);
// 	write_reg(0x01, 0x00);
// 	write_reg(0x02, 0x00);
// 	write_reg(0x03, 0x40);
// 	write_reg(0x04, 0x02);
// 	write_reg(0x05, 0x00);
// 	write_reg(0x06, 0x10);
// 	write_reg(0x07, 0x03);
// 	write_reg(0x08, 0x50);
// 	write_reg(0x09, 0x10);
// 	write_reg(0x0A, 0x03);
// 	write_reg(0x0B, 0x00);
// 	write_reg(0x0C, 0x20);
// 	write_reg(0x0D, 0x00);
// 	write_reg(0x0E, 0x24);
// 	write_reg(0x0F, 0x00);
// 	write_reg(0x10, 0x00);
// 	write_reg(0x11, 0x50);
// 	write_reg(0x12, 0x00);
// 	write_reg(0x13, 0x06);
// }

// void SMUXEN()
// {
// 	uint8_t SP_EN = bitmask(2,0); // 0000 0011, to write = 0000 0011
// 	int ret = i2c_reg_update_byte(as7341, ADDR, ENABLE, SP_EN, 0x11);
// 	printk("Ret = %d\tSP_EN = 0x%02X\n", ret, SP_EN);
// }

// void SMUXEN_poll()
// {
//     int timeOut = 1000; // Arbitrary value, but if it takes 1000 milliseconds then
//                       // something is wrong
//     int count = 0;
// 	uint8_t value;
//     int ret = i2c_reg_read_byte(as7341, ADDR, ENABLE, &value);
//     while ((value == 0x03) && (count < timeOut)) 
//     {
//         // printk("In this loop\n");
//         k_msleep(1);
//         count++;
//     }
//     printk("count = %d\n", count);
//     if (count >= timeOut)
//         return false;
//     else
//         return true;
// }

// void SP_EN()
// {
// 	// uint8_t SP_EN = bitmask(2,0); // 0001 0001, to write = 0001 0001
// 	int ret = i2c_reg_update_byte(as7341, ADDR, ENABLE, 0x11, 0x11);
// 	printk("Ret = %d\tSP_EN = 0x%02X\n", ret, SP_EN);	
// }

// // void disable_PON()
// // {
// // 	uint8_t PON = bitmask(1,0);
// // 	int ret = i2c_reg_update_byte(as7341, ADDR, ENABLE, PON, 0x00);
// // 	printk("Ret = %d\tPON = 0x%02X\n", ret, PON);	
// // }



// int main(void)
// {
// 	k_msleep(3);

// 	if (init(as7341) < 0)
// 		return 0;


// 	// set_LED_current(40);
// 	while (1)
// 	{

// 	//PON 0x01
// 	//SINT_SMUX
// 	//SINT_SIEN
// 	//SMUX_Config_RAM
// 	//SMUX map1 
// 	// SMUXEN
// 	// wait for int, SMUX_poll()
// 	// PON 0x00

// 	//PON 0x01
// 	//SINT_SMUX
// 	//SINT_SIEN
// 	//SMUX_Config_RAM
// 	//SMUX map2 
// 	// SMUXEN
// 	// wait for int, SMUX_poll()
// 	// PON 0x00

// 	}
// }

//=========================================================================================

/*
 * Zephyr C port of AMS AS7241 spectral sensor demo (Arduino version by Sijo John, 2018).
 * Uses plain Zephyr I2C APIs (no devicetree helper macros) + printk logging.
 */

// #include <zephyr/kernel.h>
// #include <zephyr/drivers/i2c.h>
// #include <zephyr/device.h>
// #include <zephyr/sys/printk.h>
// #include <stdbool.h>
// #include <stdint.h>

// #define AS7241_I2C_ADDR 0x39

// static const struct device *i2c_dev = DEVICE_DT_GET(DT_ALIAS(i2c));

// /* ---------- I2C helpers ---------- */

// static int writeRegister(uint8_t reg, uint8_t val)
// {
//     return i2c_reg_write_byte(i2c_dev, AS7241_I2C_ADDR, reg, val);
// }

// static int readRegister(uint8_t reg, uint8_t *val)
// {
//     return i2c_reg_read_byte(i2c_dev, AS7241_I2C_ADDR, reg, val);
// }

// static int readTwoRegister(uint8_t reg, uint16_t *val)
// {
//     uint8_t buf[2];
//     int ret = i2c_burst_read(i2c_dev, AS7241_I2C_ADDR, reg, buf, 2);
//     if (ret < 0) return ret;
//     *val = ((uint16_t)buf[1] << 8) | buf[0];
//     return 0;
// }

// /* ---------- Basic register controls ---------- */

// static void PON(void)
// {
//     uint8_t r = 0;
//     readRegister(0x80, &r);
//     r = (r & 0xFE) | 0x01;
//     writeRegister(0x80, r);
// }

// static void SpEn(bool enable)
// {
//     uint8_t r = 0;
//     readRegister(0x80, &r);
//     if (enable) r |= 0x02; else r &= ~0x02;
//     writeRegister(0x80, r);
// }

// static void SmuxConfigRAM(void) { writeRegister(0xAF, 0x10); }

// static void SMUXEN(void)
// {
//     uint8_t r = 0;
//     readRegister(0x80, &r);
//     r |= 0x10;
//     writeRegister(0x80, r);
// }

// static bool getSmuxEnabled(void)
// {
//     uint8_t r = 0;
//     readRegister(0x80, &r);
//     return (r & 0x10) == 0x10;
// }

// static bool getIsDataReady(void)
// {
//     uint8_t r = 0;
//     readRegister(0xA3, &r);
//     return (r & 0x40) == 0x40;
// }

// /* Integration time & gain (same as Arduino demo) */
// static void setATIME(uint8_t value) { writeRegister(0x81, value); }
// static void setASTEP(uint8_t low, uint8_t high) { writeRegister(0xCA, low); writeRegister(0xCB, high); }
// static void setGAIN(uint8_t value) { writeRegister(0xAA, value); }

// /* ---------- SMUX Configurations ---------- */

// static void F1F4_Clear_NIR(void)
// {
//     writeRegister(0x00, 0x30);
//     writeRegister(0x01, 0x01);
//     writeRegister(0x02, 0x00);
//     writeRegister(0x03, 0x00);
//     writeRegister(0x04, 0x00);
//     writeRegister(0x05, 0x42);
//     writeRegister(0x06, 0x00);
//     writeRegister(0x07, 0x00);
//     writeRegister(0x08, 0x50);
//     writeRegister(0x09, 0x00);
//     writeRegister(0x0A, 0x00);
//     writeRegister(0x0B, 0x00);
//     writeRegister(0x0C, 0x20);
//     writeRegister(0x0D, 0x04);
//     writeRegister(0x0E, 0x00);
//     writeRegister(0x0F, 0x30);
//     writeRegister(0x10, 0x01);
//     writeRegister(0x11, 0x50);
//     writeRegister(0x12, 0x00);
//     writeRegister(0x13, 0x06);
// }

// static void F5F8_Clear_NIR(void)
// {
//     writeRegister(0x00, 0x00);
//     writeRegister(0x01, 0x00);
//     writeRegister(0x02, 0x00);
//     writeRegister(0x03, 0x40);
//     writeRegister(0x04, 0x02);
//     writeRegister(0x05, 0x00);
//     writeRegister(0x06, 0x10);
//     writeRegister(0x07, 0x03);
//     writeRegister(0x08, 0x50);
//     writeRegister(0x09, 0x10);
//     writeRegister(0x0A, 0x03);
//     writeRegister(0x0B, 0x00);
//     writeRegister(0x0C, 0x00);
//     writeRegister(0x0D, 0x00);
//     writeRegister(0x0E, 0x24);
//     writeRegister(0x0F, 0x00);
//     writeRegister(0x10, 0x00);
//     writeRegister(0x11, 0x50);
//     writeRegister(0x12, 0x00);
//     writeRegister(0x13, 0x06);
// }


// /* ---------- Readout helpers ---------- */

// static void ReadRawValuesMode1(void)
// {
//     bool isEnabled = true;
//     bool isDataReady = false;
//     uint16_t v;

//     printk("Mode1: F1-F4, Clear, NIR\n");

//     PON();
//     SmuxConfigRAM();
//     F1F4_Clear_NIR();
//     SMUXEN();

//     while (isEnabled) { isEnabled = getSmuxEnabled(); }
//     SpEn(true);
//     while (!isDataReady) { isDataReady = getIsDataReady(); }

//     readTwoRegister(0x95, &v); printk("ADC0/F1:   %u\n", v);
//     readTwoRegister(0x97, &v); printk("ADC1/F2:   %u\n", v);
//     readTwoRegister(0x99, &v); printk("ADC2/F3:   %u\n", v);
//     readTwoRegister(0x9B, &v); printk("ADC3/F4:   %u\n", v);
//     readTwoRegister(0x9D, &v); printk("ADC4/Clear:%u\n", v);
//     readTwoRegister(0x9F, &v); printk("ADC5/NIR:  %u\n", v);
// }

// static void ReadRawValuesMode2(void)
// {
//     bool isEnabled = true;
//     bool isDataReady = false;
//     uint16_t v;

//     printk("Mode2: F5-F8, Clear, NIR\n");

//     PON();
//     SpEn(false);         /* match Arduino flow */
//     SmuxConfigRAM();
//     F5F8_Clear_NIR();
//     SMUXEN();

//     while (isEnabled) { isEnabled = getSmuxEnabled(); }
//     SpEn(true);
//     while (!isDataReady) { isDataReady = getIsDataReady(); }

//     readTwoRegister(0x95, &v); printk("ADC0/F5:   %u\n", v);
//     readTwoRegister(0x97, &v); printk("ADC1/F6:   %u\n", v);
//     readTwoRegister(0x99, &v); printk("ADC2/F7:   %u\n", v);
//     readTwoRegister(0x9B, &v); printk("ADC3/F8:   %u\n", v);
//     readTwoRegister(0x9D, &v); printk("ADC4/Clear:%u\n", v);
//     readTwoRegister(0x9F, &v); printk("ADC5/NIR:  %u\n", v);
// }

// /* ---------- Main ---------- */

// int main(void)
// {
//     if (!i2c_dev) {
//         printk("I2C bus not found\n");
//         return 0;
//     }

//     /* Optional: set bus speed if needed
//        i2c_configure(i2c_dev, I2C_SPEED_SET(I2C_SPEED_STANDARD));
//     */

//     printk("AS7341 Zephyr\n");

//     setATIME(0x64);
//     setASTEP(0xE7, 0x03);
//     setGAIN(0x09);

//     while (1) {
//         ReadRawValuesMode1();
//         k_msleep(200);
//         ReadRawValuesMode2();
//         k_msleep(1000);
//     }
// 	return 0;
// }
