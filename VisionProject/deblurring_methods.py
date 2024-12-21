"""
Name:  Kevin Fang
File: deblurring_methods.py
Description: 
    Implements image deblurring techniques and performance metrics.
"""
import cv2
import numpy as np
from skimage.restoration import wiener, richardson_lucy
from skimage.metrics import peak_signal_noise_ratio as psnr
from skimage.metrics import structural_similarity as ssim

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