import serial
import numpy as np
import matplotlib.pyplot as plt
from drawnow import *

# Initialize serial port
arduinoData = serial.Serial('COM8', 115200)

# Sensor data storage: each entry is a list of 10 wavelength values
data = {
    'Averaged sensor#1': [],
    'Averaged sensor#2': []
}

wavelength = ["415nm", "445nm", "480nm", "515nm", "555nm", "590nm", "630nm", "680nm", "Clear", "NIR"]
wavelength_ticks = list(range(len(wavelength)))

plt.ion()  # Interactive mode on

def makeFig():
    plt.clf()
    plt.suptitle('Live Spectrogram')

    for i, label in enumerate(data):
        if data[label]:
            sensor_matrix = np.array(data[label])  # Shape: (time_steps, 10)
            ax = plt.subplot(1, 2, i+1)
            ax.set_title(label)
            im = ax.imshow(sensor_matrix.T, aspect='auto', cmap='viridis', origin='lower')
            ax.set_yticks(wavelength_ticks)
            ax.set_yticklabels(wavelength)
            ax.set_xlabel('Time')
            ax.set_ylabel('Wavelength')
            plt.colorbar(im, ax=ax, orientation='vertical', label='Absorption')

while True:
    while arduinoData.inWaiting() == 0:
        pass  # Wait for data

    try:
        arduinoString = arduinoData.readline().decode().strip()
        dataArray = arduinoString.split(',')

        if not dataArray or ':' not in dataArray[0]:
            continue  # Skip if data is malformed

        label_part = dataArray[0].split(':')
        label = label_part[0].strip()
        dataArray[0] = label_part[1].strip()

        if label not in data:
            continue  # Skip unknown labels

        sensor_values = list(map(float, dataArray))  # Convert to float
        data[label].append(sensor_values)

        if len(data[label]) > 100:  # Limit history for memory
            data[label].pop(0)

        drawnow(makeFig)
        plt.pause(0.001)

    except Exception as e:
        print("Error:", e)
        continue
