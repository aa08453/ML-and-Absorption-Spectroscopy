import pandas as pd
import matplotlib.pyplot as plt
import glob
import os
import re

def load_filtered_file(filepath):
    try:
        with open(filepath, 'r') as f:
            lines = f.readlines()

        # Detect where data starts
        data_start = None
        delimiter = None
        column_header_found = False

        for i, line in enumerate(lines):
            if re.search(r'WL.*Abs', line):  # Case: "WL(nm), Abs"
                data_start = i + 1
                delimiter = ','
                column_header_found = True
                break
            elif re.search(r'wavelength.*absorption', line, re.IGNORECASE):  # Case: "Wavelength,Absorption"
                data_start = i + 1
                delimiter = ','
                column_header_found = True
                break
            elif re.match(r'^\s*\d+(\.\d+)?[,\s]+(\d+(\.\d+)?)([,\s]+\d+(\.\d+)?)?', line):  # Pure numeric lines
                data_start = i
                delimiter = ',' if ',' in line else r'\s+'
                column_header_found = False
                break

        if data_start is None:
            print(f"Unrecognized format in: {filepath}")
            return None

        # Load the DataFrame
        df = pd.read_csv(
            filepath,
            skiprows=data_start,
            delimiter=delimiter,
            header=None if not column_header_found else 'infer',
            engine='python'
        )

        if df.shape[1] < 2:
            print(f"Skipping {filepath}: Not enough columns")
            return None

        df = df.iloc[:, :2]
        df.columns = ['Wavelength (nm)', 'Absorbance']

        # Convert to float to avoid errors during math
        df['Wavelength (nm)'] = pd.to_numeric(df['Wavelength (nm)'], errors='coerce')
        df['Absorbance'] = pd.to_numeric(df['Absorbance'], errors='coerce')

        # Drop any rows with invalid data
        df = df.dropna()

        return df

    except Exception as e:
        print(f"Error parsing {filepath}: {e}")
        return None

# === Main Script ===
folder_path = './final_graphs'  # Change this to your folder path
file_list = glob.glob(os.path.join(folder_path, '*.csv'))

plt.figure(figsize=(12, 6))

colors = [
    "#99f800",  # dark blue
    '#ff7f0e',  # dark orange
    '#2ca02c',  # dark green
    "#81F383",  # dark red
    "#032255",  # dark purple
    "#ed4422",  # brown
    "#9f91f1",  # pink
    "#F0A9A9",  # grey
    "#f7f753",  # olive
    "#b3e7ec"   # teal
]


for idx, file in enumerate(file_list):
    df = load_filtered_file(file)
    if df is not None and not df.empty:
        label = os.path.basename(file)
        color = colors[idx % len(colors)]
        plt.plot(df['Wavelength (nm)'], df['Absorbance'], label=label, color=color)

plt.title('Absorption Spectra')
plt.xlabel('Wavelength (nm)')
plt.ylabel('Absorbance (≤ 5)')
plt.xlim(0, 1200)
plt.ylim(0, 5)

plt.legend()
plt.grid(True)
plt.tight_layout()


plt.savefig('absorption_spectra.svg', format='svg')
plt.show()