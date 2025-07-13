import serial
import time

# Adjust COM port and baud rate
ser = serial.Serial('COM12', 115200)  # e.g., COM12 on Windows
filename = "arduino_log_withoutmux.txt"

with open(filename, "w") as f:
    while True:
        try:
            line = ser.readline().decode().strip()
            print(line)
            f.write(line + "\n")
            f.flush()  # Write to file in real-time
        except KeyboardInterrupt:
            print("Logging stopped.")
            break
