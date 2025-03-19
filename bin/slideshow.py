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
image_files = glob.glob(os.path.join(folder_path, "*.[JjPp][PpNn][GgGg]"))  # Matches JPG, PNG, etc.
random.shuffle(image_files)
if not image_files:
    print(f"No images found in {folder_path}")
    sys.exit(1)

index = 0
paused = False
MAX_WIDTH = 1024
MAX_HEIGHT = 768

# Setup Tkinter window
root = tk.Tk()
root.configure(bg='black')
root.title("Image Slideshow")

# Function to correct image orientation based on EXIF data
def correct_orientation(image):
    try:
        for orientation in ExifTags.TAGS.keys():
            if ExifTags.TAGS[orientation] == 'Orientation':
                break
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

# Function to fade transition between images
def fade_transition(new_img, alpha=0.0):
    if alpha == 0:
        label.config(image=new_img)
        label.image = new_img
    if alpha < 1:
        root.attributes('-alpha', alpha)
        root.after(50, lambda: fade_transition(new_img, min(alpha + 0.1, 1)))
    else:
        root.attributes('-alpha', 1.0)
        label.config(image=new_img)
        label.image = new_img
        root.after(3000, show_next_image)

# Function to update image
def show_next_image():
    global index, paused
    if paused:
        return
    
    try:
        img = Image.open(image_files[index])
        img = correct_orientation(img)
        img = resize_image(img, MAX_WIDTH, MAX_HEIGHT)
        img = ImageTk.PhotoImage(img)
    except Exception as e:
        print(f"Error loading image {image_files[index]}: {e}")
        index = (index + 1) % len(image_files)
        root.after(3000, show_next_image)
        return
    
    fade_transition(img)
    index = (index + 1) % len(image_files)

# Create a label to display images
label = tk.Label(root, bg='black')
label.pack(expand=True)

show_next_image()

def toggle_pause(event=None):
    global paused
    paused = not paused
    if not paused:
        show_next_image()

root.bind('<Escape>', lambda e: root.destroy())
root.bind('<space>', toggle_pause)
root.mainloop()

