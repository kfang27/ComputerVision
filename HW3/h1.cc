/*
Name: Kevin Fang
File: h1.cc
Description:
    The program, h1.cc, is supposed to perform edge detection on an
    input gray-level image by using the Sobel operator, which detects edges
    by calculating the gradient magnitude at each pixel.
    Areas of high intensity change are supposed to correspond to edges in the image.

    Then, for each pixel, the program will calculate the gradient in both x and y directions, and
    compute the overall gradient magnitude with those values.
    The resulting edge intensities are then stored in a new image, where the stronger edges
    should appear as brighter pixels (should be whiter). 

Compile with:
g++ h1.cc image.cc -o h1

To run this program after compiling:
    ./h1 <input gray-level image> <output gray-level edge image>
    Ex: ./h1 hough_simple_1.pgm output_gray_edge.pgm
*/
#include "image.h"
#include <iostream>
#include <cmath>

using namespace ComputerVisionProjects;

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " {input gray-level image} {output gray-level edge image}\n";
        return 0;
    }

    const std::string input_filename(argv[1]);
    const std::string output_filename(argv[2]);

    Image input_image;
    if (!ReadImage(input_filename, &input_image)) {
        std::cerr << "Error reading input image file.\n";
        return 1;
    }

    // This is to prepare the output image with the same size as the input image
    Image output_image;
    output_image.AllocateSpaceAndSetSize(input_image.num_rows(), input_image.num_columns());
    output_image.SetNumberGrayLevels(255);

    // Sobel kernels
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

    // Iterate over the image pixels, excluding the boundary pixels
    for (size_t i = 1; i < input_image.num_rows() - 1; ++i) {
        for (size_t j = 1; j < input_image.num_columns() - 1; ++j) {
        int gradient_x = 0;
        int gradient_y = 0;

        // Apply Sobel operators to calculate gradients in x and y directions
        for (int m = -1; m <= 1; ++m) {
            for (int n = -1; n <= 1; ++n) {
            int pixel_value = input_image.GetPixel(i + m, j + n);
            gradient_x += Gx[m + 1][n + 1] * pixel_value;
            gradient_y += Gy[m + 1][n + 1] * pixel_value;
            }
        }

        int gradient_magnitude = gradient_x * gradient_x + gradient_y * gradient_y;
        int edge_intensity = std::min(255, static_cast<int>(std::sqrt(gradient_magnitude)));

        output_image.SetPixel(i, j, edge_intensity);
        }
    }

    if (!WriteImage(output_filename, output_image)) {
        std::cerr << "Error writing output edge image file.\n";
        return 1;
    }

    return 0;
}
