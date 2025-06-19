import serial
import pandas as pd
import time
import pprint
import curses

# Configure the serial connection
ser = serial.Serial('/dev/ttyUSB0', baudrate=115200, timeout=1)

labels = ["time", "415nm_F1", "445nm_F2", "480nm_F3", "515nm_F4",
          "555nm_F5", "590nm_F6", "630nm_F7", "680nm_F8", "CLEAR", "NIR"]


data = []


def main(stdscr):
    stdscr.nodelay(True)  # Don't block waiting for input
    stdscr.addstr("Press 'q' to exit...\n")

    while True:
        key = stdscr.getch()
        if key == ord("q"):  # Check for 'q' key press
            print("\nExiting...")
            df = pd.DataFrame(data, columns=labels)
            df.to_csv("test.csv", index=False)
            ser.close()
            break

        raw_readings = [time.time()] + ser.readline().decode('utf-8',
                                                             errors='ignore').strip().split(",")
        # readings = zip(labels, raw_readings)
        # print(raw_readings)
        readings = dict(zip(labels, raw_readings))
        if readings[labels[1]] != '':
            data.append(raw_readings)
        print(readings)
        print("\r\n")


curses.wrapper(main)