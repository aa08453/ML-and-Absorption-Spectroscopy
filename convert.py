import pandas as pd

# Replace with your actual file paths
file1 = 'C:/Users/PC/Desktop/milk_graphs/milk_graphs/milkpak11.csv'
file2 = 'C:/Users/PC/Desktop/milk_graphs/milk_graphs/milkpak22.csv'
output_file = 'milkpak_avgg.csv'

def load_clean_csv(filepath):
    with open(filepath, 'r') as f:
        lines = f.readlines()

    # Find where the actual data starts
    for i, line in enumerate(lines):
        if 'WL' in line and 'Abs' in line:
            data_start = i + 1
            break
    else:
        # Assume data starts at line 0 if header not found
        data_start = 0

    # Read only first 2 columns after skipping metadata
    df = pd.read_csv(filepath, skiprows=data_start, usecols=[0, 1], header=None)
    df.columns = ["Wavelength", "Absorption"]
    return df.apply(pd.to_numeric, errors='coerce').dropna()

# Load and clean both CSVs
df1 = load_clean_csv(file1)
df2 = load_clean_csv(file2)

# Make sure both DataFrames are aligned
min_len = min(len(df1), len(df2))
df1 = df1.iloc[:min_len]
df2 = df2.iloc[:min_len]

# Average values
avg_df = pd.DataFrame({
    "Wavelength": (df1["Wavelength"] + df2["Wavelength"]) / 2,
    "Absorption": (df1["Absorption"] + df2["Absorption"]) / 2
})

# Save with header
avg_df.to_csv(output_file, index=False)
print(f"Averaged file saved as: milkpakkkk")
