import pandas as pd
import matplotlib.pyplot as plt
import glob

csv_file = "filename.csv"

# Function to read CSV and extract wavelength and absorbance
def read_absorption_data(file):
    df = pd.read_csv(file, skiprows=12, usecols=[0, 1], names=["Wavelength", "Absorbance"])
    return df

plt.figure(figsize=(10, 6))

# Loop through files and plot data
data = read_absorption_data(csv_file)
plt.plot(data["Wavelength"], data["Absorbance"])

plt.xlabel("Wavelength (nm)")
plt.ylabel("Absorbance")
plt.title("Absorption Spectrum")
plt.legend()
plt.grid(True)
plt.show()
