#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include "../inc/func.h"


K_THREAD_STACK_DEFINE (led_stack, 1024);


K_THREAD_STACK_DEFINE(adc_stack, 1024);
struct k_thread adc_thread;


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


uint8_t read_reg_field(const struct device* sensor, uint8_t reg_addr, bool field, int position, int bit_count)
{
    uint8_t value;
    int ret = i2c_reg_read_byte(sensor, ADDR, reg_addr, &value);
    if (ret) {
        printk("Failed to read register 0x%02X\t ret = %d\n", reg_addr, ret);
        return 0;
    }
    return (field) ? read_bits(value, position, bit_count) : value;
}

uint16_t read_two_reg(const struct device* sensor, uint8_t reg_addr)
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

void write_bits(const struct device* sensor, uint8_t reg, uint8_t field_value, int position, int bits)
{
    uint8_t mask = ((1U << bits) - 1) << position;
    uint8_t old_val;
    i2c_reg_read_byte(sensor, ADDR, reg, &old_val);
    uint8_t new_val = (old_val & ~mask) | ((field_value << position) & mask);
    int ret = i2c_reg_write_byte(sensor, ADDR, reg, new_val);
    if (ret) printk("Unable to write 0x%02X\n", reg);
}

void write_state(const struct device* sensor, uint8_t reg, state_t state, int position, int bits) 
{ 
    if (state == ON)
        write_bits(sensor, reg, 1, position, bits); 
    else if (state == OFF)
        write_bits(sensor, reg, 0, position, bits);
}


void write_reg(const struct device* sensor, uint8_t reg, uint8_t value)
{
    int ret = i2c_reg_write_byte(sensor, ADDR, reg, value);
    if (ret) {
        printk("Unable to write register 0x%02X, ret = %d\n", reg, ret);
    }  
}


//==========================================================================================

void bank(const struct device* sensor, state_t state)
{
    write_state(sensor, CFG0, state, 4, 1);
}



// void blink_LED(void* p1, void* p2, void* p3)
// {
//     while (1)
//     {
//         bank(ON);
//         write_state(CONFIG, led_state, 3, 1);
//         write_state(LED, led_state, 7, 1);
//         bank(OFF);
//         k_usleep(10);  
//     }
// }

void blink_LED(const struct device* sensor, state_t state)
{
    bank(sensor, ON);
    write_state(sensor, CONFIG, state, 3, 1);
    write_state(sensor, LED, state, 7, 1);
    bank(sensor, OFF);
    k_usleep(10);   
}

//==========================================================================================

void set_ATIME(as7341_t* as7341, uint8_t value)
{
    write_reg(as7341->sensor, ATIME, value); 
}

uint8_t get_ATIME(as7341_t* as7341)
{
    return read_reg_field(as7341->sensor, ATIME, true, 0, 8);
}

//==========================================================================================


void set_ASTEP(as7341_t* as7341, uint16_t value)
{
    if (value == 65535) value = 65534;
    uint8_t data[2];
    data[0] = value & 0xFF;        // LSB
    data[1] = (value >> 8) & 0xFF; // MSB
    int ret = i2c_burst_write(as7341->sensor, ADDR, ASTEP_L, data, 2);
    if (ret)
    {
        printk("Failed to write ASTEP,\nret = %d\n", ret);
        return;
    }
}

uint16_t get_ASTEP(as7341_t* as7341)
{

    uint8_t data[2];

    int ret = i2c_burst_read(as7341->sensor, ADDR, ASTEP_L, data, 2);
    if (ret)
    {
        printk("Failed to read ASTEP,\nret = %d\n", ret);
        return 0;
    }
    uint16_t value = data[0] | (data[1] << 8);
    return value;
}

//==========================================================================================


void set_GAIN(as7341_t* as7341, gain_t gain)
{
    write_bits(as7341->sensor, CFG1, (uint8_t)gain, 0, 5);
}

gain_t get_GAIN(as7341_t* as7341)
{
    return (gain_t) read_reg_field(as7341->sensor, CFG1, true, 0, 5);
}

//==========================================================================================


int init(as7341_t* as7341)
{
	if (as7341->sensor == NULL || !device_is_ready(as7341->sensor))
	{
		printk("No device\n");
		return -1;
	} 
    if (bus_scan(as7341->sensor) < 0) return -1;
	printk("Sensor initialized\n");
    return 0;
}


void enable_LED(as7341_t* as7341, state_t state)
{
    static bool led_thread_created = false;
    // const char *s = (state) ? "ON" : "OFF";
    // printk("LED turned %s\n", s);
    as7341->led_state = state;
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
    blink_LED(as7341->sensor, state);
}

void set_LED_current(as7341_t* as7341, int current)
{
    if (current > 258)
        current = 258;
    else if (current < 4)
        current = 4;

    uint8_t led_drive = (current - 4) >> 1;
    write_bits(as7341->sensor, LED, led_drive, 0, 7);
    printk("LED current set to %dmA\n", current);  
}


void enable_SINT_SMUX(const struct device* sensor)
{
    write_bits(sensor, CFG9, 1, 4, 1);
}

void enable_SIEN(const struct device* sensor)
{
    write_bits(sensor, INTENAB, 1, 0, 1);
}

void enable_PON(const struct device* sensor)
{
    write_bits(sensor, ENABLE, 1, 0, 1);
}

// enable spectral measurement
void enable_SP(const struct device* sensor, uint8_t num)
{
    write_bits(sensor, ENABLE, num, 1, 1);   
}

// enable SMUX
bool enable_SMUX(const struct device* sensor)
{
    write_bits(sensor, ENABLE, 1, 4, 1);  // set SMUXEN=1
    int timeout = 1000;
    while (timeout-- > 0) {
        if (read_reg_field(sensor, ENABLE, true, 4, 1) == 0)
            return true; // success
        k_msleep(1);
    }
    return false; // timeout
}


// set SMUX command
void SMUX_Config_RAM(const struct device* sensor)
{
    write_bits(sensor, CFG6, 2, 3, 2);
}


uint8_t get_AVALID(const struct device* sensor)
{    
    return read_reg_field(sensor, STATUS2, true, 6, 1);
    
}


void map_to_ADCs(const struct device* sensor, bool f1_to_f4)
{
    if (f1_to_f4)
    {
        write_reg(sensor, 0x00, 0x30); // F3 left set to ADC2
        write_reg(sensor, 0x01, 0x01); // F1 left set to ADC0
        write_reg(sensor, 0x02, 0x00); // Reserved or disabled
        write_reg(sensor, 0x03, 0x00); // F8 left disabled
        write_reg(sensor, 0x04, 0x00); // F6 left disabled
        write_reg(sensor, 0x05, 0x42); // F4 left connected to ADC3/f2 left connected to ADC1
        write_reg(sensor, 0x06, 0x00); // F5 left disbled
        write_reg(sensor, 0x07, 0x00); // F7 left disbled
        write_reg(sensor, 0x08, 0x50); // CLEAR connected to ADC4
        write_reg(sensor, 0x09, 0x00); // F5 right disabled
        write_reg(sensor, 0x0A, 0x00); // F7 right disabled
        write_reg(sensor, 0x0B, 0x00); // Reserved or disabled
        write_reg(sensor, 0x0C, 0x20); // F2 right connected to ADC1
        write_reg(sensor, 0x0D, 0x04); // F4 right connected to ADC3
        write_reg(sensor, 0x0E, 0x00); // F6/F8 right disabled
        write_reg(sensor, 0x0F, 0x30); // F3 right connected to AD2
        write_reg(sensor, 0x10, 0x01); // F1 right connected to AD0
        write_reg(sensor, 0x11, 0x50); // CLEAR right connected to AD4
        write_reg(sensor, 0x12, 0x00); // Reserved or disabled
        write_reg(sensor, 0x13, 0x06); // NIR connected to ADC5
    }
    else // f5 - f8
    {
        write_reg(sensor, 0x00, 0x00); // F3 left disable
        write_reg(sensor, 0x01, 0x00); // F1 left disable
        write_reg(sensor, 0x02, 0x00); // reserved/disable
        write_reg(sensor, 0x03, 0x40); // F8 left connected to ADC3
        write_reg(sensor, 0x04, 0x02); // F6 left connected to ADC1
        write_reg(sensor, 0x05, 0x00); // F4/ F2 disabled
        write_reg(sensor, 0x06, 0x10); // F5 left connected to ADC0
        write_reg(sensor, 0x07, 0x03); // F7 left connected to ADC2
        write_reg(sensor, 0x08, 0x50); // CLEAR Connected to ADC4
        write_reg(sensor, 0x09, 0x10); // F5 right connected to ADC0
        write_reg(sensor, 0x0A, 0x03); // F7 right connected to ADC2
        write_reg(sensor, 0x0B, 0x00); // Reserved or disabled
        write_reg(sensor, 0x0C, 0x00); // F2 right disabled
        write_reg(sensor, 0x0D, 0x00); // F4 right disabled
        write_reg(sensor, 0x0E, 0x24); // F8 right connected to ADC2/ F6 right connected to ADC1
        write_reg(sensor, 0x0F, 0x00); // F3 right disabled
        write_reg(sensor, 0x10, 0x00); // F1 right disabled
        write_reg(sensor, 0x11, 0x50); // CLEAR right connected to AD4
        write_reg(sensor, 0x12, 0x00); // Reserved or disabled
        write_reg(sensor, 0x13, 0x06); // NIR connected to ADC5
    }
}

void read_ADCs(const struct device* sensor, uint16_t* buffer, int start)
{
    buffer[start + 0] = read_two_reg(sensor, CH0_DATA);
    buffer[start + 1] = read_two_reg(sensor, CH1_DATA);
    buffer[start + 2] = read_two_reg(sensor, CH2_DATA);
    buffer[start + 3] = read_two_reg(sensor, CH3_DATA);
    buffer[start + 4] = read_two_reg(sensor, CH4_DATA);
    buffer[start + 5] = read_two_reg(sensor, CH5_DATA);
		
}


void read_channels(const struct device* sensor, uint16_t* buffer,  bool f1_to_f4)
{
    enable_PON(sensor);
    enable_SP(sensor, 0);
    enable_SINT_SMUX(sensor);
    enable_SIEN(sensor);


    SMUX_Config_RAM(sensor);
    map_to_ADCs(sensor, f1_to_f4);

    if (!enable_SMUX(sensor)) {
        printk("SMUX enable failed\n");
        return;
    }
    

    enable_SP(sensor, 1);
    bool DataReady = false;
    while(!DataReady)
    {
        DataReady = get_AVALID(sensor);
    }
    int start = f1_to_f4 ? 0 : 6;
    read_ADCs(sensor, buffer, start);
}

uint16_t get_channel(as7341_t* as7341, color_channel_t channel)
{
    return as7341->channel_readings[channel];
}

void read_all_channels(as7341_t* as7341)
{
    read_channels(as7341->sensor, as7341->channel_readings, true);
    read_channels(as7341->sensor, as7341->channel_readings, false);
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






