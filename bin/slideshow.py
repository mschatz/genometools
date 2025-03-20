#!/usr/bin/env python

import os
import glob
import sys
import tkinter as tk
import random
from PIL import Image, ImageTk, ExifTags

# Check if a folder path is provided
if len(sys.argv) < 2:
    print("Usage: python slideshow.py /path/to/your/images")
    sys.exit(1)

folder_path = sys.argv[1]

# Verify the path exists
if not os.path.isdir(folder_path):
    print(f"Error: The folder '{folder_path}' does not exist.")
    sys.exit(1)

# Get all image files in the folder (JPG, JPEG, PNG)
image_files = glob.glob(os.path.join(folder_path, "*.jpg")) + \
              glob.glob(os.path.join(folder_path, "*.jpeg")) + \
              glob.glob(os.path.join(folder_path, "*.png"))
random.shuffle(image_files)
if not image_files:
    print(f"No images found in {folder_path}")
    sys.exit(1)

index = 0
paused = False
MAX_WIDTH = 1024
MAX_HEIGHT = 768
SLIDE_DELAY = 15000

# Setup Tkinter window
root = tk.Tk()
root.configure(bg='black')
root.title("Image Slideshow")

# Function to correct image orientation based on EXIF data
def correct_orientation(image):
    try:
        orientation = next((k for k, v in ExifTags.TAGS.items() if v == 'Orientation'), None)
        exif = image._getexif()
        if exif is not None:
            orientation_value = exif.get(orientation, None)
            if orientation_value == 3:
                image = image.rotate(180, expand=True)
            elif orientation_value == 6:
                image = image.rotate(270, expand=True)
            elif orientation_value == 8:
                image = image.rotate(90, expand=True)
    except (AttributeError, KeyError, IndexError):
        pass
    return image

# Function to resize image while maintaining aspect ratio
def resize_image(image, max_width, max_height):
    width, height = image.size
    ratio = min(max_width / width, max_height / height)
    new_size = (int(width * ratio), int(height * ratio))
    return image.resize(new_size, Image.Resampling.LANCZOS)

# Function to update image
def update_image():
    try:
        img = Image.open(image_files[index])
        img = correct_orientation(img)
        img = resize_image(img, MAX_WIDTH, MAX_HEIGHT)
        img = ImageTk.PhotoImage(img)
    except Exception as e:
        print(f"Error loading image {image_files[index]}: {e}")
        return
    
    label.config(image=img)
    label.image = img
    filename_label.config(text=f"{os.path.basename(image_files[index])} ({index + 1}/{len(image_files)})")

# Function to show next image
def show_next_image():
    global index, paused
    if paused:
        return
    index = (index + 1) % len(image_files)
    update_image()

# Function to show previous image
def show_prev_image():
    global index, paused
    if paused:
        return
    index = (index - 1) % len(image_files)
    update_image()

# Create a label to display images
label = tk.Label(root, bg='black')
label.pack(expand=True)

# Create a label to display filename and index
filename_label = tk.Label(root, bg='black', fg='white', font=("Arial", 14))
filename_label.pack(side='bottom', fill='x')

update_image()
root.after(SLIDE_DELAY, show_next_image)

# Toggle pause function
def toggle_pause(event=None):
    global paused
    paused = not paused

# Bind keys
root.bind('<Escape>', lambda e: root.destroy())
root.bind('<q>', lambda e: root.destroy())
root.bind('<space>', toggle_pause)
root.bind('<Right>', lambda e: show_next_image())
root.bind('<Left>', lambda e: show_prev_image())
root.bind('<i>', lambda e: filename_label.config(text=f"{os.path.basename(image_files[index])} ({index + 1}/{len(image_files)})"))

root.mainloop()

