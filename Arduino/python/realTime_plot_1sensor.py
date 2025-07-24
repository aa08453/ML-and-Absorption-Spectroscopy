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
        ser.write(b'g')  # Request new data
        line_data = ser.readline().decode('ascii').strip()
        values = list(map(int, line_data.split(',')))
        # z = max(values) + 10
        if len(values) == 10:
            line.set_ydata(values)
            
        else:
            print("Bad data:", line_data)
    except Exception as e:
        print("Error:", e)

    ax.set_xticks(x_values)
    ax.set_xticklabels(channel_labels, rotation=45)
    ax.set_ylim(0, 1000)
    ax.set_title("AS7341 Real-Time Spectral Plot")
    ax.set_ylabel("Sensor Intensity")
    ax.set_xlabel("Wavelength")
    ax.grid(True)

# Setup serial port (change COM port as needed)
ser = serial.Serial("COM3", 115200, timeout=1)
time.sleep(2)  # Allow Arduino to reset

# Create animation
ani = animation.FuncAnimation(fig, animate, fargs=(line, ser), interval=500)
plt.tight_layout()
plt.show()
ser.close()
