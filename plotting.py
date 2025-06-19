import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Load the CSV file
plot_name = input("What input do you want: ")
df = pd.read_csv(plot_name)

# Extract time and spectral data
time_values = df["time"] - df["time"].iloc[0]  # Normalize time
spectral_data = df.iloc[:, 1:].to_numpy().T  # Exclude time, transpose for spectrogram

# Create the spectrogram plot
fig, ax = plt.subplots(figsize=(10, 6))
im = ax.imshow(spectral_data, aspect="auto", cmap="plasma", interpolation="nearest",
               extent=[time_values.min(), time_values.max(), 0, len(df.columns) - 2])

# Labels
ax.set_yticks(range(len(df.columns) - 1))
ax.set_yticklabels(df.columns[1:])
ax.set_xlabel("Time (seconds)")
ax.set_title("Spectrogram of AS7341 Sensor Readings")
fig.colorbar(im, label="Sensor Value")

# Show the plot
plt.show()