import h5py
import pandas as pd
import os
import numpy as np
import subprocess
import shlex
import glob

# GitHub repo details
GITHUB_REPO = "https://github.com/aa08453/ML-and-Absorption-Spectroscopy.git"
HDF5_FILE = "repo/milk_quality.h5"  

# AS7341 Wavelength Mapping
AS7341_WAVELENGTHS = np.array([415, 445, 480, 515, 555, 590, 630, 680, 850, 940])  # Estimated NIR bands

def pull_latest_hdf5():
    """Pull the latest HDF5 file from GitHub and ensure it stays inside `repo/`."""
    if os.path.exists("repo"):
        subprocess.run(["git", "-C", "repo", "pull"])
    else:
        subprocess.run(["git", "clone", GITHUB_REPO, "repo"])

    # Ensure HDF5 file exists inside repo
    if not os.path.exists(HDF5_FILE):
        print("No existing HDF5 file found in the repo. Creating a new one.")
        with h5py.File(HDF5_FILE, "w") as f:
            f.create_group("samples")  # Initialize empty HDF5 file

def collect_csv_files(inputs):
    """Collects CSV files from folders or filenames."""
    all_csv_files = []
    for item in inputs:
        if os.path.isdir(item):  # If input is a folder, get all CSV files inside
            csv_files = glob.glob(os.path.join(item, "*.csv"))
            all_csv_files.extend(csv_files)
        elif os.path.isfile(item) and item.endswith(".csv"):  # If input is a single CSV file
            all_csv_files.append(item)
        else:
            print(f"Skipping: {item} (Not a valid folder or CSV file)")
    return all_csv_files

def add_samples_from_csv(csv_files, hdf5_file, data_type):
    """Processes and adds CSV data to the HDF5 file."""
    added_samples = []  # Track timestamps for commit message
    metadata_updates = False

    with h5py.File(hdf5_file, "a") as hdf5_file:
        if "samples" not in hdf5_file:
            hdf5_file.create_group("samples")  # Ensure samples group exists

        for csv_file in csv_files:
            print(f"Processing: {csv_file}")

            if data_type == "3":  # AS7341 10-Channel Sensor Data
                df = pd.read_csv(csv_file)
                for _, row in df.iterrows():
                    timestamp = str(row["time"])  # Extract timestamp
                    group_name = f"/samples/{timestamp}"

                    if group_name in hdf5_file:
                        print(f"Skipping duplicate sample: {timestamp}")
                        continue

                    # Extract all 10 channels
                    intensities = np.array([
                        row["415nm_F1"], row["445nm_F2"], row["480nm_F3"], row["515nm_F4"],
                        row["555nm_F5"], row["590nm_F6"], row["630nm_F7"], row["680nm_F8"],
                        row["CLEAR"], row["NIR"]
                    ]).astype(float)  # Convert to float for storage

                    sample_group = hdf5_file.create_group(group_name)
                    sample_group.create_dataset("Wavelengths", data=AS7341_WAVELENGTHS)
                    sample_group.create_dataset("Intensities", data=intensities)

                    sample_group.attrs["Timestamp"] = timestamp
                    metadata_updates |= add_metadata(sample_group)  # Track if metadata was added
                    added_samples.append(timestamp)

                    print(f"Added AS7341 sample: {timestamp}")

    return added_samples, metadata_updates

def add_metadata(sample_group):
    """Asks the user for optional metadata and adds it to the sample."""
    add_meta = input(f"Do you want to add metadata for {sample_group.name}? (y/n): ").strip().lower()
    
    if add_meta == "y":
        while True:
            key = input("Enter metadata key (or press Enter to stop): ").strip()
            if key == "":
                break  # Stop if user presses Enter

            value = input(f"Enter value for {key}: ").strip()
            sample_group.attrs[key] = value
            print(f"Added metadata: {key} = {value}")
        return True  # Metadata was updated
    return False  # No metadata update

def view_samples(hdf5_file):
    """Displays stored samples and metadata, handling different data sources."""
    if not os.path.exists(hdf5_file):
        print("No database found.")
        return

    with h5py.File(hdf5_file, "r") as hdf5_file:
        if "samples" not in hdf5_file:
            print("No samples found in HDF5 file.")
            return
        
        print("\nAvailable Timestamps:")
        for sample in hdf5_file["samples"]:
            print(f" - {sample}")

        for sample in hdf5_file["samples"]:
            group = hdf5_file[f"samples/{sample}"]
            metadata = {key: group.attrs[key] for key in group.attrs}

            print(f"\nTimestamp: {metadata['Timestamp']}")

            # Check if Wavelengths exist before accessing
            if "Wavelengths" in group:
                wavelengths = group["Wavelengths"][:]
                print(f"Wavelengths: {wavelengths[:5]} ...")

            # Check if Absorption and Transmission exist (Spectrophotometer data)
            if "Absorption" in group and "Transmission" in group:
                absorption = group["Absorption"][:]
                transmission = group["Transmission"][:]
                print(f"Absorption: {absorption[:5]} ...")
                print(f"Transmission: {transmission[:5]} ...")

            # Check if Intensities exist (AS7341 Color Sensor data)
            if "Intensities" in group:
                intensities = group["Intensities"][:]
                print(f"Intensities: {intensities[:5]} ...")

            print(f"Metadata: {metadata}")

def push_updated_hdf5(added_samples, data_type, metadata_updates):
    """Pushes the updated HDF5 file back to GitHub with a detailed commit message."""
    
    if not os.path.exists(HDF5_FILE):
        print("Error: HDF5 file missing. Aborting push.")
        return
    
    os.chdir("repo")
    subprocess.run(["git", "add", "milk_quality.h5"])

    # **Format commit message**
    source_map = {"1": "RGB Sensor", "2": "Spectrophotometer", "3": "AS7341 Sensor"}
    source_type = source_map.get(data_type, "Unknown Source")
    
    metadata_status = "with metadata update" if metadata_updates else "without metadata update"
    commit_message = (
        f"Updated HDF5: Added {len(added_samples)} samples from {source_type} {metadata_status}\n"
        f"Timestamps: {', '.join(added_samples) if added_samples else 'None'}"
    )

    subprocess.run(["git", "commit", "-m", commit_message])
    subprocess.run(["git", "push", "origin", "main"])
    os.chdir("..")  # Return to original directory

if __name__ == "__main__":
    pull_latest_hdf5()

    # Input multiple folders or filenames
    INPUTS = shlex.split(input("Enter folder names or CSV filenames (wrap in quotes if they have spaces): "))
    CSV_FILES = collect_csv_files(INPUTS)

    if not CSV_FILES:
        print("No valid CSV files found. Exiting.")
    else:
        TYPE = input("Enter CSV source (1 for RGB, 2 for spectrophotometer, 3 for AS7341 color sensor): ")
        added_samples, metadata_updates = add_samples_from_csv(CSV_FILES, HDF5_FILE, TYPE)

        print("\nViewing updated database:")
        view_samples(HDF5_FILE)

        push_updated_hdf5(added_samples, TYPE, metadata_updates)
