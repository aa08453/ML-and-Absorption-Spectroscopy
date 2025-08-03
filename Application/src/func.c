#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include "../inc/func.h"

const struct device* sensor;
volatile state_t led_state = OFF;
K_THREAD_STACK_DEFINE (led_stack, 1024);


K_THREAD_STACK_DEFINE(adc_stack, 1024);
struct k_thread adc_thread;

static uint16_t channel_readings[12]; 

//==========================================================================================

int bus_scan(const struct device* sensor)
{
    // printk("Scanning I2C bus...\n");
    for (uint8_t addr = 0x03; addr < 0x78; addr++) 
    {
        uint8_t dummy;
        int ret = i2c_read(sensor, &dummy, 1, addr);
        if (ret == 0)
        {
            // printk("Device found at 0x%02X\n", addr);
            return 0;
        }
    }
    printk("Device not found.\n");
    return -1;
}

//==========================================================================================

uint8_t read_bits(uint8_t reg_value, int position, int bit_count)
{
    uint8_t mask = ((1U << bit_count) - 1) << position;
    return (reg_value & mask) >> position;
}


uint8_t read_reg_field(uint8_t reg_addr, bool field, int position, int bit_count)
{
    uint8_t value;
    int ret = i2c_reg_read_byte(sensor, ADDR, reg_addr, &value);
    if (ret) {
        printk("Failed to read register 0x%02X\t ret = %d\n", reg_addr, ret);
        return 0;
    }
    return (field) ? read_bits(value, position, bit_count) : value;
}

uint16_t read_two_reg(uint8_t reg_addr)
{
    uint8_t buffer[2] = {0};
    int ret = i2c_burst_read(sensor, ADDR, reg_addr, buffer, 2);
    if (ret)
    {
        printk("Failed to read from register 0x%02X\n", reg_addr);
        return 0; 
    }
    uint16_t value = ((uint16_t)buffer[1] << 8) | buffer[0];
    return value;
}

//=====================================================================================


// Inline utility to build a bitmask at a position
static inline uint8_t bitmask(int bits, int position) 
{
    return ((1U << bits) - 1) << position;
}

void write_bits(uint8_t reg, uint8_t field_value, int position, bool read, int bits)
{
    uint8_t mask = bitmask(bits, position);
    uint8_t value = (field_value << position) & mask;

    int ret;
    if (read) // only a specific portion
        ret = i2c_reg_update_byte(sensor, ADDR, reg, mask, value);
    else
        // Direct write with pre-masked value (used only if you're sure the rest of the bits are don't-care)
        ret = i2c_reg_write_byte(sensor, ADDR, reg, value);

    if (ret)
        printk("Unable to write register 0x%02X, ret = %d\n", reg, ret);
}

// wrapper for ON/OFF
void write_state(uint8_t reg, state_t state, int position, bool read, int bits)
{
    write_bits(reg, state == ON, position, read, bits);
}

void write_reg(uint8_t reg, uint8_t value)
{
    int ret = i2c_reg_write_byte(sensor, ADDR, reg, value);
    if (ret) {
        printk("Unable to write register 0x%02X, ret = %d\n", reg, ret);
    }  
}


//==========================================================================================

void bank(state_t state)
{
    write_state(CFG0, state, 4, true, 1);
}



// void blink_LED(void* p1, void* p2, void* p3)
// {
//     while (1)
//     {
//         bank(ON);
//         write_state(CONFIG, led_state, 3, false, 1);
//         write_state(LED, led_state, 7, true, 1);
//         bank(OFF);
//         k_usleep(10);  
//     }
// }

void blink_LED()
{
    bank(ON);
    write_state(CONFIG, led_state, 3, false, 1);
    write_state(LED, led_state, 7, true, 1);
    bank(OFF);
    k_usleep(10);   
}

//==========================================================================================

void set_ATIME(uint8_t value)
{
    write_reg(ATIME, value); 
}

uint8_t get_ATIME()
{
    return read_reg_field(ATIME, true, 0, 8);
}

//==========================================================================================


void set_ASTEP(uint16_t value)
{
    if (value == 65535) value = 65534;
    uint8_t data[2];
    data[0] = value & 0xFF;        // LSB
    data[1] = (value >> 8) & 0xFF; // MSB
    int ret = i2c_burst_write(sensor, ADDR, ASTEP_L, data, 2);
    if (ret)
    {
        printk("Failed to write ASTEP,\nret = %d\n", ret);
        return;
    }
}

uint16_t get_ASTEP()
{

    uint8_t data[2];

    int ret = i2c_burst_read(sensor, ADDR, ASTEP_L, data, 2);
    if (ret)
    {
        printk("Failed to read ASTEP,\nret = %d\n", ret);
        return 0;
    }
    uint16_t value = data[0] | (data[1] << 8);
    return value;
}

//==========================================================================================


void set_GAIN(gain_t gain)
{
    write_bits(CFG1, (uint8_t)gain, 0, false, 5);
}

gain_t get_GAIN()
{
    return (gain_t) read_bits(CFG1, 0, 5);
}

//==========================================================================================


int init(const struct device* as7341)
{
    sensor = as7341;
	if (sensor == NULL || !device_is_ready(sensor))
	{
		printk("No device\n");
		return -1;
	} 
    if (bus_scan(sensor) < 0) return -1;
	printk("Sensor initialized\n");
    return 0;
}


void enable_LED(state_t state)
{
    static bool led_thread_created = false;
    // const char *s = (state) ? "ON" : "OFF";
    // printk("LED turned %s\n", s);
    led_state = state;
    if (!led_thread_created)
    {
        const char *s = (state) ? "ON" : "OFF";
        printk("LED turned %s\n", s);
    //     static struct k_thread led_thread;

    //     k_thread_create(&led_thread, led_stack, K_THREAD_STACK_SIZEOF(led_stack),
    //         blink_LED, NULL, NULL, NULL,
    //         5, 0, K_NO_WAIT);
        led_thread_created = true;
    }
    blink_LED();
}

void set_LED_current(int current)
{
    if (current > 258)
        current = 258;
    else if (current < 4)
        current = 4;

    uint8_t led_drive = (current - 4) >> 1;
    write_bits(LED, led_drive, 0, false, 7);
    printk("LED current set to %dmA\n", current);  
}


void enable_SINT_SMUX()
{
    write_bits(CFG9, 1, 4, true, 1);
}

void enable_SIEN()
{
    write_bits(INTENAB, 1, 0, true, 1);
}

void enable_PON()
{
    write_bits(ENABLE, 1, 0, true, 1);
}

// enable spectral measurement
void enable_SP(uint8_t num)
{
    write_bits(ENABLE, num, 1, true, 1);   
}

// enable SMUX
bool enable_SMUX()
{
    write_bits(ENABLE, 1, 4, true, 1);
    // printk("Value of SMUX_enable = %d\n" , read_reg_field(ENABLE, true, 4, 1));
    int timeOut = 5000; // Arbitrary value, but if it takes 1000 milliseconds then
                      // something is wrong
    int count = 0;
    uint8_t value = read_reg_field(ENABLE, true, 4, 1);
    while (value && count < timeOut) 
    {
        k_msleep(1);
        count++;
    }
    // printk("Timeout \t count = %d\n", count);
    if (count >= timeOut)
        return false;
    else
        return true;
}

// polling to see if SMUX is enabled and cleared
bool get_SMUX_enable()
{
    int timeOut = 1000; // Arbitrary value, but if it takes 1000 milliseconds then
                      // something is wrong
    int count = 0;
    uint8_t value = read_reg_field(ENABLE, true, 4, 1);
    while (value && count < timeOut) 
    {
        k_msleep(1);
        count++;
    }
    // printk("Timeout \t count = %d\n", count);
    if (count >= timeOut)
        return false;
    else
        return true;
}

// set SMUX command
void SMUX_Config_RAM()
{
    write_bits(CFG6, 2, 3, true, 2);
}


uint8_t get_AVALID()
{    
    return read_reg_field(STATUS2, true, 6, 1);
}


void map_to_ADCs(bool f1_to_f4)
{
    if (f1_to_f4)
    {
        write_reg(0x00, 0x30); // F3 left set to ADC2
        write_reg(0x01, 0x01); // F1 left set to ADC0
        write_reg(0x02, 0x00); // Reserved or disabled
        write_reg(0x03, 0x00); // F8 left disabled
        write_reg(0x04, 0x00); // F6 left disabled
        write_reg(0x05, 0x42); // F4 left connected to ADC3/f2 left connected to ADC1
        write_reg(0x06, 0x00); // F5 left disbled
        write_reg(0x07, 0x00); // F7 left disbled
        write_reg(0x08, 0x50); // CLEAR connected to ADC4
        write_reg(0x09, 0x00); // F5 right disabled
        write_reg(0x0A, 0x00); // F7 right disabled
        write_reg(0x0B, 0x00); // Reserved or disabled
        write_reg(0x0C, 0x20); // F2 right connected to ADC1
        write_reg(0x0D, 0x04); // F4 right connected to ADC3
        write_reg(0x0E, 0x00); // F6/F8 right disabled
        write_reg(0x0F, 0x30); // F3 right connected to AD2
        write_reg(0x10, 0x01); // F1 right connected to AD0
        write_reg(0x11, 0x50); // CLEAR right connected to AD4
        write_reg(0x12, 0x00); // Reserved or disabled
        write_reg(0x13, 0x06); // NIR connected to ADC5
    }
    else // f5 - f8
    {
        write_reg(0x00, 0x00); // F3 left disable
        write_reg(0x01, 0x00); // F1 left disable
        write_reg(0x02, 0x00); // reserved/disable
        write_reg(0x03, 0x40); // F8 left connected to ADC3
        write_reg(0x04, 0x02); // F6 left connected to ADC1
        write_reg(0x05, 0x00); // F4/ F2 disabled
        write_reg(0x06, 0x10); // F5 left connected to ADC0
        write_reg(0x07, 0x03); // F7 left connected to ADC2
        write_reg(0x08, 0x50); // CLEAR Connected to ADC4
        write_reg(0x09, 0x10); // F5 right connected to ADC0
        write_reg(0x0A, 0x03); // F7 right connected to ADC2
        write_reg(0x0B, 0x00); // Reserved or disabled
        write_reg(0x0C, 0x00); // F2 right disabled
        write_reg(0x0D, 0x00); // F4 right disabled
        write_reg(0x0E, 0x24); // F8 right connected to ADC2/ F6 right connected to ADC1
        write_reg(0x0F, 0x00); // F3 right disabled
        write_reg(0x10, 0x00); // F1 right disabled
        write_reg(0x11, 0x50); // CLEAR right connected to AD4
        write_reg(0x12, 0x00); // Reserved or disabled
        write_reg(0x13, 0x06); // NIR connected to ADC5
    }
}

void read_ADCs(uint16_t* buffer, int start)
{
    buffer[start + 0] = read_two_reg(CH0_DATA);
    buffer[start + 1] = read_two_reg(CH1_DATA);
    buffer[start + 2] = read_two_reg(CH2_DATA);
    buffer[start + 3] = read_two_reg(CH3_DATA);
    buffer[start + 4] = read_two_reg(CH4_DATA);
    buffer[start + 5] = read_two_reg(CH5_DATA);
}


void read_channels(bool f1_to_f4)
{
    enable_PON();
    enable_SP(0);
    enable_SINT_SMUX();
    enable_SIEN();


    SMUX_Config_RAM();
    map_to_ADCs(f1_to_f4);
    while(enable_SMUX());
    
    // while (get_SMUX_enable());
    // printk("Value of SMUX_enable = %d\n" , read_reg_field(ENABLE, true, 4, 1));
    enable_SP(1);
    // printk("Value of SP_Enable = %d\n" , read_reg_field(ENABLE, true, 1, 1));
    while (!get_AVALID());
    int start = f1_to_f4 ? 0 : 6;
    read_ADCs(channel_readings, start);
}

uint16_t get_channel(color_channel_t channel)
{
    return channel_readings[channel];
}

void read_all_channels()
{
    read_channels(true);
    read_channels(false);
}

// void print_channels(void *arg1, void *arg2, void *arg3)
// {
//     ARG_UNUSED(arg1);
//     ARG_UNUSED(arg2);
//     ARG_UNUSED(arg3);

//     while (1)
//     {
//         read_all_channels();  // fills global channel_readings[12]

//         printk("Channel Readings: ");
//         for (int i = 0; i < 12; ++i)
//             printk("%u\t", channel_readings[i]);
//         printk("\n");

//         k_msleep(100);  // adjust delay as needed
//     }
// }

// void get_readings()
// {
//     static bool adc_thread_created = false;
//     if (!adc_thread_created)
//     {
//         k_thread_create(&adc_thread, adc_stack, K_THREAD_STACK_SIZEOF(adc_stack),
//                         print_channels, NULL, NULL, NULL, 5, 0, K_NO_WAIT);
//         adc_thread_created = true;
//     }
// }






