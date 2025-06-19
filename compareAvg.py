import pandas as pd
import matplotlib.pyplot as plt

# Load CSVs
csv1 = pd.read_csv('sensor1.csv')
csv2 = pd.read_csv('sensor1.csv')
csv1 = csv1.drop(columns=["time"], errors='ignore')  # safely ignore if 'time' doesn't exist
csv2 = csv2.drop(columns=["time"], errors='ignore')  # safely ignore if 'time' doesn't exist

# Set channel names for better labels (adjust if your CSVs have more/less columns)
channel_names = [
    "415nm_F1", "445nm_F2", "480nm_F3", "515nm_F4",
    "555nm_F5", "590nm_F6", "630nm_F7", "680nm_F8",
    "CLEAR", "NIR"
]

# Make sure both have same number of columns
assert csv1.shape[1] == csv2.shape[1], "CSV column mismatch"

# Compute mean for each channel
mean1 = csv1.mean()
mean2 = csv2.mean()

# Compute difference
diff = mean1 - mean2

# Plot
plt.figure(figsize=(10, 6))
plt.bar(channel_names, diff)
plt.title("Average Channel Difference: Sensor 1 - Sensor 2")
plt.ylabel("Difference in Reading")
plt.xticks(rotation=45)
plt.grid(axis='y')
plt.tight_layout()
plt.show()
