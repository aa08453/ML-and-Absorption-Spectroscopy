import h5py
import matplotlib.pyplot as plt
import os
import numpy as np
import subprocess
import curses

# GitHub repo details
GITHUB_REPO = "https://github.com/aa08453/ML-and-Absorption-Spectroscopy.git"
HDF5_FILE = "repo/milk_quality.h5"

def pull_latest_hdf5():
    if os.path.exists("repo"):
        subprocess.run(["git", "-C", "repo", "pull"])
    else:
        subprocess.run(["git", "clone", GITHUB_REPO, "repo"])

    if not os.path.exists(HDF5_FILE):
        print("No existing HDF5 file found on GitHub. Exiting.")
        exit()

def list_samples(hdf5_file):
    if not os.path.exists(hdf5_file):
        print("No database found.")
        return []

    with h5py.File(hdf5_file, "r") as hdf5_file:
        if "samples" not in hdf5_file:
            print("No samples found in HDF5 file.")
            return []

        return list(hdf5_file["samples"].keys())

def plot_samples(hdf5_file, selected_samples):
    if not os.path.exists(hdf5_file):
        print("No database found.")
        return

    with h5py.File(hdf5_file, "r") as hdf5_file:
        fig, ax = plt.subplots(figsize=(10, 6))
        handles = []
        labels = []
        visibility = {}  # Track visibility of each plot

        for sample in selected_samples:
            group_name = f"samples/{sample}"
            if group_name not in hdf5_file:
                print(f"Warning: Sample {sample} not found. Skipping.")
                continue

            group = hdf5_file[group_name]
            wavelengths = group["Wavelengths"][:]
            absorption = group["Absorption"][:]

            line, = ax.plot(wavelengths, absorption, label=sample)  
            handles.append(line)
            labels.append(sample)
            visibility[line] = True  # Initially, all plots are visible

        ax.set_xlabel("Wavelength (nm)")
        ax.set_ylabel("Absorption")
        ax.set_title("Wavelength vs Absorption")
        ax.grid()

        # Create interactive legend
        leg = ax.legend(handles, labels, loc="upper right", fontsize=10, frameon=True)
        leg.set_draggable(True)

        # Make legend items clickable
        def toggle_visibility(event):
            """Handles legend click events to toggle visibility."""
            for line, leg_line in zip(handles, leg.get_lines()):
                if event.artist == leg_line:
                    visible = not line.get_visible()
                    line.set_visible(visible)
                    leg_line.set_alpha(1.0 if visible else 0.3)  # Dim legend text if hidden
                    fig.canvas.draw()

        fig.canvas.mpl_connect("pick_event", toggle_visibility)  # Enable clicking legend
        for leg_line in leg.get_lines():
            leg_line.set_picker(True)  # Make legend items clickable

        plt.show()

def select_samples_curses(stdscr, timestamps):
    curses.curs_set(0)
    selected = [False] * len(timestamps)
    current = 0

    while True:
        stdscr.clear()
        height, width = stdscr.getmaxyx()

        stdscr.addstr(0, 0, "Use ↑ ↓ to navigate, SPACE to select, ENTER to confirm", curses.A_BOLD)

        for i, ts in enumerate(timestamps):
            if i + 2 >= height:
                break

            mode = curses.A_REVERSE if i == current else curses.A_NORMAL
            mark = "[X] " if selected[i] else "[ ] "
            ts_display = ts[:width - 6] if len(ts) > width - 6 else ts
            stdscr.addstr(i + 2, 0, mark + ts_display, mode)

        key = stdscr.getch()
        if key == curses.KEY_UP and current > 0:
            current -= 1
        elif key == curses.KEY_DOWN and current < len(timestamps) - 1:
            current += 1
        elif key == ord(" "):
            selected[current] = not selected[current]
        elif key == 10:
            break

    return [ts for i, ts in enumerate(timestamps) if selected[i]]

def run_curses_selection(timestamps):
    try:
        return curses.wrapper(select_samples_curses, timestamps)
    except KeyboardInterrupt:
        print("\nCancelled. Resetting terminal...")
        curses.endwin()

if __name__ == "__main__":
    pull_latest_hdf5()
    available_samples = list_samples(HDF5_FILE)

    if available_samples:
        selected_samples = run_curses_selection(available_samples)

        if selected_samples:
            plot_samples(HDF5_FILE, selected_samples)
        else:
            print("No valid timestamps selected.")
