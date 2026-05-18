import serial
import csv
import os
from datetime import datetime

SERIAL_PORT   = "/dev/ttyUSB0"
BAUD_RATE     = 115200
FILE_PREFIX   = "adafruit1_test"
OUTPUT_DIR    = "."
READINGS      = 2          # Number of averaged readings to capture PER SENSOR

CHANNELS = ["415nm", "445nm", "480nm", "515nm", "555nm", "590nm", "630nm", "680nm", "Clear", "NIR"]
HEADER   = ["timestamp"] + CHANNELS


def parse_sensor_line(line: str):
    line = line.strip()
    for label in ("Averaged sensor#1:", "Averaged sensor#2:"):
        if line.startswith(label):
            sensor_id = 1 if "sensor#1" in label else 2
            data_part = line[len(label):].strip()
            row = {"sensor": sensor_id}
            for token in data_part.split(","):
                token = token.strip()
                if ":" in token:
                    key, _, val = token.partition(":")
                    try:
                        row[key.strip()] = int(val.strip())
                    except ValueError:
                        pass
            return row
    return None


def make_csv_path(sensor_id: int) -> str:
    filename = f"{FILE_PREFIX}_sensor{sensor_id}.csv"
    return os.path.join(OUTPUT_DIR, filename)


def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    path1, path2 = make_csv_path(1), make_csv_path(2)
    file_exists = {1: os.path.exists(path1), 2: os.path.exists(path2)}

    f1 = open(path1, "a", newline="")
    f2 = open(path2, "a", newline="")
    writers = {
        1: csv.DictWriter(f1, fieldnames=HEADER),
        2: csv.DictWriter(f2, fieldnames=HEADER),
    }

    for sid, writer in writers.items():
        if not file_exists[sid]:
            writer.writeheader()

    print(f"Logging to:\n  {path1}\n  {path2}")
    print(f"Capturing {READINGS} readings per sensor. Press Ctrl+C to stop early.\n")

    # Track how many valid readings captured per sensor
    counts = {1: 0, 2: 0}

    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)

        while counts[1] < READINGS or counts[2] < READINGS:
            raw = ser.readline()
            try:
                line = raw.decode("utf-8", errors="replace")
            except Exception:
                continue

            parsed = parse_sensor_line(line)
            if parsed is None:
                continue

            sid = parsed["sensor"]

            # Skip if this sensor already has enough readings
            if counts[sid] >= READINGS:
                continue

            ts  = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
            row = {"timestamp": ts}
            for ch in CHANNELS:
                row[ch] = parsed.get(ch, "")

            writers[sid].writerow(row)
            (f1 if sid == 1 else f2).flush()

            counts[sid] += 1
            print(f"[Sensor {sid}] ({counts[sid]}/{READINGS}) {ts}  Clear={row['Clear']}  NIR={row['NIR']}")

        print(f"\nDone. {READINGS} readings saved for each sensor.")

    except KeyboardInterrupt:
        print(f"\nStopped early. Sensor1: {counts[1]} readings, Sensor2: {counts[2]} readings.")
    except serial.SerialException as e:
        print(f"Serial error: {e}")
    finally:
        f1.close()
        f2.close()


if __name__ == "__main__":
    main()
