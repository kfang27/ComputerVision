"""
Name:  Kevin Fang
File: project.py
"""
import cv2
import numpy as np
import matplotlib.pyplot as plt

def load_image(image_path):
    image = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)
    if image is None:
        raise FileNotFoundError(f"Image not found at path: {image_path}")
    return image

def display_image(image, title="Image"):
    """
    Display an image using matplotlib.
    Args:
        image (np.ndarray): Image to display.
        title (str): Title of the image window.
    """
    plt.imshow(image, cmap='gray')
    plt.title(title)
    plt.axis('off')
    plt.show()
    
def main():
    # Step 1: Load the image
    image_path = "path_to_your_image.jpg"  # Replace with image path
    try:
        original_image = load_image(image_path)
        display_image(original_image, "Original Image")
    except FileNotFoundError as e:
        print(e)
        return