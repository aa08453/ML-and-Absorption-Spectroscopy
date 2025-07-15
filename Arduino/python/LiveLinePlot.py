import serial
import numpy as np
import matplotlib.pyplot as plt
from drawnow import *

# Initialize serial port
arduinoData = serial.Serial('COM8', 115200)

# Sensor data storage
data = {
    'Averaged sensor#1': [],
    'Averaged sensor#2': []
}

wavelength = ["415nm", "445nm", "480nm", "515nm", "555nm", "590nm", "630nm", "680nm", "Clear", "NIR"]

plt.ion()  # Interactive mode on

def makeFig():
    plt.clf()
    plt.title('Live Streaming Sensor Data')
    plt.grid(True)
    plt.ylabel('Absorption')

    for label in data:
        if data[label]:  # Only plot if there is data
            avg_data = np.mean(data[label], axis=0)
            plt.plot(wavelength, avg_data, 'o-', label=label)

    plt.legend(loc='upper left')

while True:
    while arduinoData.inWaiting() == 0:
      #  print('sksksksk')
        pass  # Wait for data

    try:
        arduinoString = arduinoData.readline().decode().strip()
        dataArray = arduinoString.split(',')

        if not dataArray or ':' not in dataArray[0]:
            
            continue  # Skip if data is malformed

        label_part = dataArray[0].split(':')
        label = label_part[0].strip()
        dataArray[0] = label_part[1].strip()
        print(label,dataArray)

        if label not in data:
            continue  # Skip unknown labels

        sensor_values = list(map(float, dataArray))  # Convert to float

        data[label].append(sensor_values)

        if len(data[label]) > 50:
            data[label].pop(0)  # Keep last 50 readings
        
        print(data[label])

        drawnow(makeFig)
        plt.pause(0.0001)

    except Exception as e:
        print("Error:", e)
        continue
