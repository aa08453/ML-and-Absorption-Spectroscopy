# Milk Quality Analysis Repository

This repository contains scripts for collecting, storing, and visualizing spectral data related to milk quality. The dataset includes readings from different sensors, including:
- **Spectrophotometer Data**
- **RGB Sensor Data**
- **AS7341 10-Channel Color Sensor Data**

The scripts allow users to:
- **Store sensor readings into an HDF5 database.**
- **Retrieve and visualize stored spectral data.**
- **Automatically pull and push updated data to a GitHub repository.**

---

## 📂 Repository Structure
```
repo/
├── milk_quality.h5    # HDF5 database (stores all sensor data)
├── data/              # Folder to store raw CSV files
├── script.py          # Script to collect and store data in HDF5
├── plotting.py        # Script to retrieve and visualize stored data
└── README.md          # Documentation (this file)
```

---

## 🔧 Installation & Setup

1. **Clone the repository**:
```bash
 git clone https://github.com/aa08453/ML-and-Absorption-Spectroscopy.git
 cd ML-and-Absorption-Spectroscopy
```

2. **Install dependencies**:
```bash
 pip install -r requirements.txt
```

3. **Ensure you have Git installed and configured**
```bash
 git config --global user.name "Your Name"
 git config --global user.email "youremail@example.com"
```
---

## 📝 Usage Guide

### **1️⃣ Collect & Store Sensor Data**
To add sensor data from CSV files into the HDF5 database, use `script.py`:
```bash
 python script.py
```
- The script will prompt you to enter the **CSV filenames or folder paths**.
- You can specify the **sensor type**:
  - `1` for RGB Sensor
  - `2` for Spectrophotometer
  - `3` for AS7341 10-Channel Color Sensor
- The script will process the data, add it to `milk_quality.h5`, and push the updated database back to GitHub.

### **2️⃣ Retrieve & Visualize Data**
To view and plot stored sensor data, use `plotting.py`:
```bash
 python plotting.py
```
- The script will list available timestamps from `milk_quality.h5`.
- You can **select multiple timestamps** using an interactive menu.
- The script will generate **Wavelength vs. Absorption plots** with an interactive legend.

---

## 🔄 Git Integration
- **Data is stored in `milk_quality.h5` inside the `repo/` directory.**
- When you **add new data**, it is automatically committed and pushed to GitHub with a detailed commit message.
- Commit messages include:
  - **Number of new samples added**
  - **Sensor type used**
  - **Whether metadata was updated**
  - 
---

## ⚠️ Troubleshooting
### "HDF5 file missing"
Ensure `milk_quality.h5` is inside the `repo/` folder. If missing, rerun `script.py`, which will create a new file automatically.

### "Git push fails"
Ensure you have configured Git with your credentials:
```bash
git config --global user.name "Your Name"
git config --global user.email "youremail@example.com"
```
If authentication fails, try:
```bash
git pull origin main  # Sync latest changes
git push origin main  # Push your updates
```

---
