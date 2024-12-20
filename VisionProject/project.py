"""
Name:  Kevin Fang
File: project.py
"""
import cv2
import os
import numpy as np
import matplotlib.pyplot as plt

def load_image(image_path):
    image = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)
    if image is None:
        raise FileNotFoundError(f"Image not found at path: {image_path}")
    return image

def display_image(image, title="Image"):
    plt.imshow(image, cmap='gray')
    plt.title(title)
    plt.axis('off')
    plt.show()
    
def save_image(image, output_path):
    cv2.imwrite(output_path, image)
    print(f"Image saved at: {output_path}")

def apply_gaussian_blur(image, kernel_size=(15, 15), sigma=5):
    return cv2.GaussianBlur(image, kernel_size, sigma)


def add_gaussian_noise(image, mean=0, std_dev=20):
    noise = np.random.normal(mean, std_dev, image.shape).astype(np.float32)
    noisy_image = cv2.add(image.astype(np.float32), noise)
    return np.clip(noisy_image, 0, 255).astype(np.uint8)

def main():
    # Paths for input and output images
    input_image_path = "image1.jpeg"  # Replace with your image path
    output_dir = "output_images"
    os.makedirs(output_dir, exist_ok=True)  # Create the directory if it doesn't exist

    try:
        # Step 1: Load the image
        original_image = load_image(input_image_path)
        save_image(original_image, os.path.join(output_dir, "original_image.jpg"))
    except FileNotFoundError as e:
        print(e)
        return
    
    # Step 2: Apply Gaussian blur
    blurred_image = apply_gaussian_blur(original_image, kernel_size=(15, 15), sigma=5)
    save_image(blurred_image, os.path.join(output_dir, "blurred_image.jpg"))
    
    # Step 3: Add Gaussian noise
    noisy_image = add_gaussian_noise(blurred_image, mean=0, std_dev=20)
    save_image(noisy_image, os.path.join(output_dir, "noisy_image.jpg"))
    
if __name__ == "__main__":
    main()