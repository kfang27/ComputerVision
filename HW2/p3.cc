#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <tuple>
#include "image.h"

using namespace std;
using namespace ComputerVisionProjects;

#define M_PI 3.14159265358979323846  // Define M_PI manually

// Function to calculate object attributes
void ComputeObjectAttributes(const Image &labeled_image, const string &output_file, Image &output_image) {
    int rows = labeled_image.num_rows();
    int cols = labeled_image.num_columns();

    // Vector to store attributes
    vector<tuple<int, double, double, double, double, double, double>> attributes;

    // Iterate through the image to compute attributes
    for (int label = 1; label <= 255; ++label) {
        int area = 0;
        double sum_row = 0;
        double sum_col = 0;
        double sum_ii = 0; // For moment of inertia
        double sum_xy = 0; // For calculating orientation moments

        // Iterate through the image to find pixels belonging to the current object
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (labeled_image.GetPixel(i, j) == label) {
                    area++;
                    sum_row += i;
                    sum_col += j;

                    // Calculate moments for inertia and orientation
                    double x_diff = i - (sum_row / area);
                    double y_diff = j - (sum_col / area);
                    sum_ii += (x_diff * x_diff); // Update for Emin
                    sum_xy += (x_diff * y_diff);
                }
            }
        }

        if (area > 0) {
            double center_row = sum_row / area;
            double center_col = sum_col / area;
            double Emin = sum_ii / area; // Minimum moment of inertia
            double roundedness = (4 * M_PI * area) / (pow(area, 2)); // Roundedness formula
            double orientation = atan2(2 * sum_xy, sum_ii) * (180.0 / M_PI); // Orientation in degrees

            attributes.emplace_back(label, center_row, center_col, Emin, area, roundedness, orientation);
        }
    }

    // Write the attributes to the output file
    ofstream out(output_file);
    if (!out.is_open()) {
        cerr << "Error opening output file: " << output_file << endl;
        return;
    }

    for (const auto &attr : attributes) {
        out << get<0>(attr) << " " << get<1>(attr) << " " << get<2>(attr) << " "
            << get<3>(attr) << " " << get<4>(attr) << " "
            << get<5>(attr) << " " << get<6>(attr) << endl;
    }
    out.close();

    // Draw center and orientation on output image
    output_image.AllocateSpaceAndSetSize(rows, cols);
    output_image.SetNumberGrayLevels(255); // Set to max gray levels

    for (const auto &attr : attributes) {
        int label = get<0>(attr);
        double center_row = get<1>(attr);
        double center_col = get<2>(attr);
        double orientation = get<6>(attr);

        // Draw a dot at the center position
        output_image.SetPixel(static_cast<int>(center_row), static_cast<int>(center_col), 255); // White dot

        // Draw orientation line
        int line_length = 10; // Length of the orientation line
        int end_row = static_cast<int>(center_row + line_length * cos(orientation * M_PI / 180.0));
        int end_col = static_cast<int>(center_col + line_length * sin(orientation * M_PI / 180.0));

        // Ensure the end position is within image bounds
        if (end_row >= 0 && end_row < rows && end_col >= 0 && end_col < cols) {
            output_image.SetPixel(end_row, end_col, 255); // Draw line end
        }
    }
}

// Main function
int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <input_labeled_image.pgm> <output_object_descriptions.txt> <output_image.pgm>" << endl;
        return 1;
    }

    const string input_filename = argv[1];
    const string output_description_filename = argv[2];
    const string output_image_filename = argv[3];

    Image labeled_image;
    if (!ReadImage(input_filename, &labeled_image)) {
        cerr << "Error reading labeled image." << endl;
        return 1;
    }

    Image output_image;
    ComputeObjectAttributes(labeled_image, output_description_filename, output_image);

    if (!WriteImage(output_image_filename, output_image)) {
        cerr << "Error writing output image." << endl;
        return 1;
    }

    cout << "Object descriptions saved as: " << output_description_filename << endl;
    cout << "Output image saved as: " << output_image_filename << endl;
    return 0;
}
