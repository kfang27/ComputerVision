"""
Name:  Kevin Fang
File: project.py
"""
import cv2
import os
import numpy as np
import matplotlib.pyplot as plt
from skimage.restoration import wiener
from skimage.restoration import richardson_lucy
from skimage.metrics import peak_signal_noise_ratio as psnr
from skimage.metrics import structural_similarity as ssim

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

def apply_wiener_filter(noisy_image, kernel_size=5):
    # Normalize the input noisy image to [0, 1]
    normalized_image = noisy_image / 255.0
    
    # Create a uniform kernel
    kernel = np.ones((kernel_size, kernel_size)) / (kernel_size ** 2)
    
    # Estimate noise variance (ensure noisy_image and filtered image have same scale)
    filtered_image = cv2.filter2D(normalized_image, -1, kernel)
    noise_variance = np.var(normalized_image - filtered_image)
    
    # Apply Wiener filter
    deblurred_image = wiener(normalized_image, kernel, noise_variance)
    
    # Scale the result back to [0, 255] and convert to uint8
    return np.clip(deblurred_image * 255, 0, 255).astype(np.uint8)

def apply_lucy_richardson(noisy_image, psf, iterations=30):
    # Normalize the noisy image to the range [0, 1]
    noisy_image_normalized = noisy_image / 255.0
    
    # Perform Lucy-Richardson deconvolution
    deblurred_image = richardson_lucy(noisy_image_normalized, psf, num_iter=iterations)
    
    # Scale the result back to [0, 255] and convert to uint8
    deblurred_image = np.clip(deblurred_image * 255, 0, 255).astype(np.uint8)
    return deblurred_image

def calculate_psnr(original_image, processed_image):
    """
    Calculate PSNR between the original and processed images.
    """
    return psnr(original_image, processed_image, data_range=255)

def calculate_ssim(original_image, processed_image):
    """
    Calculate SSIM between the original and processed images.
    """
    return ssim(original_image, processed_image, data_range=255)

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
    
    # Step 4: Apply Wiener filtering
    deblurred_image_wiener = apply_wiener_filter(noisy_image, kernel_size=5)
    save_image(deblurred_image_wiener, os.path.join(output_dir, "deblurred_image_wiener.jpg"))
    deblurred_wiener = apply_wiener_filter(blurred_image, kernel_size=5)
    save_image(deblurred_wiener, os.path.join(output_dir, "deblurred_wiener.jpg"))
    
    # Step 5: Apply Lucy-Richardson deconvolution
    psf = cv2.getGaussianKernel(15, 5)  # Create a Gaussian PSF matching the blur
    psf = psf @ psf.T  # Convert 1D kernel to 2D

    deblurred_image_lucy = apply_lucy_richardson(noisy_image, psf, iterations=30)
    save_image(deblurred_image_lucy, os.path.join(output_dir, "deblurred_image_lucy.jpg"))
    deblurred_lucy = apply_lucy_richardson(blurred_image, psf, iterations=30)
    save_image(deblurred_lucy, os.path.join(output_dir, "deblurred_lucy.jpg"))
    
    # Step 6: Compare the results using PSNR and SSIM
    psnr_wiener = calculate_psnr(original_image, deblurred_image_wiener)
    psnr_lucy = calculate_psnr(original_image, deblurred_image_lucy)
    
    # With blurred images only (not noisy)
    psnr_wiener2 = calculate_psnr(original_image, deblurred_wiener)
    psnr_lucy2 = calculate_psnr(original_image, deblurred_lucy)
    
    ssim_wiener = calculate_ssim(original_image, deblurred_image_wiener)
    ssim_lucy = calculate_ssim(original_image, deblurred_image_lucy)
    
    ssim_wiener2 = calculate_ssim(original_image, deblurred_wiener)
    ssim_lucy2 = calculate_ssim(original_image, deblurred_lucy)
    
    print(f"PSNR (Wiener Filter): {psnr_wiener:.2f}")
    print(f"PSNR (Lucy-Richardson): {psnr_lucy:.2f}")
    print(f"SSIM (Wiener Filter): {ssim_wiener:.2f}")
    print(f"SSIM (Lucy-Richardson): {ssim_lucy:.2f}")
    
    
    print(f"PSNR (Wiener Filter): {psnr_wiener2:.2f}")
    print(f"PSNR (Lucy-Richardson): {psnr_lucy2:.2f}")
    print(f"SSIM (Wiener Filter): {ssim_wiener2:.2f}")
    print(f"SSIM (Lucy-Richardson): {ssim_lucy2:.2f}")


    
if __name__ == "__main__":
    main()