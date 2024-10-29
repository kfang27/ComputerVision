/*
Name: Kevin Fang
File: h2.cc
Description:
    The program, h2.cc, is supposed to convert a gray-level image 
    into a binary image (image will just be in the colors, black and white)
    while using a given user-input threshold value.

To run this program after compiling with the makefile (make all):
    ./p1 <input_image.pgm> <threshold> <binary_image.pgm>
    Ex: ./p1 two_objects.pgm 128 binary_two_objects.pgm
*/
#include <iostream>
#include "image.h"

using namespace std;
using namespace ComputerVisionProjects;

// Function to convert a gray-level image to a binary image based on a threshold value
void ConvertToBinary(const Image &input_image, Image &output_image, int threshold) {
    output_image.AllocateSpaceAndSetSize(input_image.num_rows(), input_image.num_columns());
    output_image.SetNumberGrayLevels(1);

    // Iterate through each pixel in the input image
    for (size_t i = 0; i < input_image.num_rows(); ++i) {
        for (size_t j = 0; j < input_image.num_columns(); ++j) {
            int gray_level = input_image.GetPixel(i, j);
            // Set the pixel in the output image based on the threshold
            if (gray_level >= threshold) {
                output_image.SetPixel(i, j, 255); // White
            } else {
                output_image.SetPixel(i, j, 0); // Black
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input.pgm> <threshold> <output.pgm>" << std::endl;
        return 1;
    }

    const std::string input_filename = argv[1];
    const int threshold = std::stoi(argv[2]);
    const std::string output_filename = argv[3];

    ComputerVisionProjects::Image image;

    // Was used to test to see whether the pgm image format was able to be read
    if (!ReadImage(input_filename, &image)) {
        std::cerr << "Error reading image." << std::endl;
        return 1;
    }

    ComputerVisionProjects::Image binary_image;
    binary_image.AllocateSpaceAndSetSize(image.num_rows(), image.num_columns());
    binary_image.SetNumberGrayLevels(255);

    for (size_t i = 0; i < image.num_rows(); ++i) {
        for (size_t j = 0; j < image.num_columns(); ++j) {
            int pixel_value = image.GetPixel(i, j);
            if (pixel_value > threshold) {
                // Setting pixel to white
                binary_image.SetPixel(i, j, 255); 
            } else {
                // Setting pixel to black
                binary_image.SetPixel(i, j, 0);   
            }
        }
    }

    if (!WriteImage(output_filename, binary_image)) {
        std::cerr << "Error writing binary image." << std::endl;
        return 1;
    }

    std::cout << "Binary image saved as: " << output_filename << std::endl;
    return 0;
}
