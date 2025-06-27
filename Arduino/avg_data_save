import serial
import re
import time

SERIAL_PORT = 'COM12'      # Replace with your port (e.g., COM4, /dev/ttyUSB0)
BAUD_RATE = 115200
OUTPUT_FILE = 'sensor_readings.csv'
READINGS_LIMIT = 15

# Regular expression to match 10 numbers in the line
value_pattern = re.compile(r'415nm: (\d+), 445nm: (\d+), 480nm: (\d+), 515nm: (\d+), 555nm: (\d+), 590nm: (\d+), 630nm: (\d+), 680nm: (\d+), Clear: (\d+), NIR: (\d+)')

ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
time.sleep(2)  # Let Arduino reset

with open(OUTPUT_FILE, 'w') as file:
    # CSV header
    file.write('415nm,445nm,480nm,515nm,555nm,590nm,630nm,680nm,Clear,NIR\n')

    print("Reading from Arduino...")

    count = 0
    try:
        while count < READINGS_LIMIT:
            line = ser.readline().decode('utf-8').strip()
            
            if line.startswith("Averaged values:"):
                match = value_pattern.search(line)
                if match:
                    values = match.groups()
                    file.write(','.join(values) + '\n')
                    print(f"Reading {count + 1}/{READINGS_LIMIT}:", values)
                    count += 1
    except KeyboardInterrupt:
        print("\nStopped by user.")
    finally:
        ser.close()
        print("Serial connection closed.")
