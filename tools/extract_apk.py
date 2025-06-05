#!/usr/bin/env python3
import setup_venv

import grabsounds
import sys
import zipfile
import os

# Constants
TITLE = 'APK Extractor'
ASSETS_DIR = 'assets/'
SOUND_DATA_FILE = 'libminecraftpe.so'

# Determine Mode
use_gui = len(sys.argv) < 2
if use_gui:
    # Setup GUI
    import tkinter as tk
    from tkinter import filedialog, messagebox
    root = tk.Tk()
    root.withdraw()

# Show Information
def info(msg):
    print(msg)
    if use_gui:
        messagebox.showinfo(title=TITLE, message=msg)

# Extract Sounds
def extract_sounds(apk_zip):
    # Search For File
    for name in apk_zip.namelist():
        if os.path.basename(name) == SOUND_DATA_FILE:
            with apk_zip.open(name) as sound_data:
                grabsounds.main(sound_data)
            return
    raise Exception('Unable to find sound data!')

# Extract Assets
def extract_assets(apk_zip):
    assets = os.path.dirname(os.path.abspath(__file__))
    assets = os.path.join(assets, '..', 'game', 'assets')
    # Search For Assets
    for name in apk_zip.namelist():
        if name.startswith(ASSETS_DIR):
            # Found Asset
            short_name = name[len(ASSETS_DIR):]
            print(f'Extracting Asset {short_name}...')
            out = os.path.join(assets, short_name)
            os.makedirs(os.path.dirname(out), exist_ok=True)
            # Extract
            with open(out, 'wb') as file:
                file.write(apk_zip.read(name))

# Open APK
def open_apk():
    if use_gui:
        out = filedialog.askopenfile('rb', title=TITLE, filetypes=[('APK', '*.apk')])
        if out is None:
            sys.exit()
        return out
    else:
        return open(sys.argv[1], 'rb')

# Main
def main():
    with open_apk() as apk:
        # Extract
        with zipfile.ZipFile(apk) as apk_zip:
            extract_sounds(apk_zip)
            extract_assets(apk_zip)
    # Done
    info('Done!')

# Handle Errors
try:
    main()
except Exception as e:
    if use_gui:
        info('An error has occurred!')
    raise
