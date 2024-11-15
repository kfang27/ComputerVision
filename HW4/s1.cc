/*
Name: Kevin Fang
File: s1.cc
Description:
    The program, s1.cc, is supposed to locate the sphere in an image and computes
    its center and radius. The threshold value is used on a grayscale image to
    obtain a binary image.

    It calculates the center of the sphere by finding the centroid of the circle.
    The radius is determined by averaging the horizontal and vertical extents of the circle
    and dividing by two.

To run this program after compiling:
    ./s1 <input gray-level sphere image> <threshold value> <output parameters file>
    Ex: ./s1 sphere0.pgm 100 parameters.txt
*/
#include "image.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace ComputerVisionProjects;

void FindSphereCenterAndRadius(const Image &binary_image, int &x_center, int &y_center, double &radius) {
    int x_min = binary_image.num_columns(), x_max = 0;
    int y_min = binary_image.num_rows(), y_max = 0;
    int x_sum = 0, y_sum = 0, pixel_count = 0;

    for (size_t y = 0; y < binary_image.num_rows(); ++y) {
        for (size_t x = 0; x < binary_image.num_columns(); ++x) {
            if (binary_image.GetPixel(y, x) > 0) {
                x_sum += x;
                y_sum += y;
                pixel_count++;

                if (x < x_min) x_min = x;
                if (x > x_max) x_max = x;
                if (y < y_min) y_min = y;
                if (y > y_max) y_max = y;
            }
        }
    }

    x_center = x_sum / pixel_count;
    y_center = y_sum / pixel_count;
    double diameter = (x_max - x_min + y_max - y_min) / 2.0;
    radius = diameter / 2.0;
}

bool ThresholdImage(const Image &input_image, int threshold, Image *binary_image) {
    if (!binary_image) return false;
    binary_image->AllocateSpaceAndSetSize(input_image.num_rows(), input_image.num_columns());

    for (size_t y = 0; y < input_image.num_rows(); ++y) {
        for (size_t x = 0; x < input_image.num_columns(); ++x) {
            int pixel_value = input_image.GetPixel(y, x);
            binary_image->SetPixel(y, x, pixel_value >= threshold ? 1 : 0);
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " {input gray-level sphere image} {threshold value} {output parameters file}\n";
        return 1;
    }

    const std::string input_filename(argv[1]);
    const int threshold = std::stoi(argv[2]);
    const std::string output_filename(argv[3]);

    Image input_image;
    if (!ReadImage(input_filename, &input_image)) {
        std::cerr << "Error reading input image\n";
        return 1;
    }

    Image binary_image;
    if (!ThresholdImage(input_image, threshold, &binary_image)) {
        std::cerr << "Error creating binary image\n";
        return 1;
    }

    int x_center, y_center;
    double radius;
    FindSphereCenterAndRadius(binary_image, x_center, y_center, radius);

    std::ofstream output_file(output_filename);
    if (!output_file) {
        std::cerr << "Error opening output file\n";
        return 1;
    }
    output_file << x_center << " " << y_center << " " << radius << "\n";
    output_file.close();

    std::cout << "Sphere center and radius written to " << output_filename << "\n";
    return 0;
}
