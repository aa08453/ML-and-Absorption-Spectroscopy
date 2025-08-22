import time
import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Channel labels (X-axis categories)
channel_labels = [
    "415nm", "445nm", "480nm", "515nm", "555nm",
    "590nm", "630nm", "680nm", "Clear", "NIR"
]
x_values = list(range(len(channel_labels)))  # Numeric x-axis positions

# Create figure and axis
fig, ax = plt.subplots()
line, = ax.plot(x_values, [0]*10, marker='o', linestyle='-', color='blue')

def animate(i, line, ser):
    try:
        line_data = ser.readline().decode('ascii', errors='ignore').strip()

        # Only process lines that start with our marker
        if not line_data.startswith("Channel Readings: "):
            return

        # Remove the prefix and split the numbers
        parts = line_data.replace("Channel Readings: ", "").strip().split('\t')
        # print(parts)

        # Convert to ints safely
        values = [int(p) for p in parts if p.strip().isdigit()]
        print(values)

        if len(values) == 10:
            line.set_ydata(values)
            # print("Current ydata:", line.get_ydata())
        else:
            print("Bad data:", values, "Raw:", repr(line_data))

    except Exception as e:
        print("Error:", e)



    ax.set_xticks(x_values)
    ax.set_xticklabels(channel_labels, rotation=45)
    ax.set_ylim(0, 65000)
    ax.set_title("AS7341 Real-Time Spectral Plot")
    ax.set_ylabel("Sensor Intensity")
    ax.set_xlabel("Wavelength")
    ax.grid(True)

# Setup seri
ser = serial.Serial("/dev/ttyACM0", 115200, timeout=1)

time.sleep(0.1)

# Create animation
ani = animation.FuncAnimation(fig, animate, fargs=(line, ser), interval=500)
plt.tight_layout()
plt.show()
ser.close()
