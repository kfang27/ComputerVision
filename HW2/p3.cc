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
        double sum_xx = 0; // For calculating inertia
        double sum_yy = 0; // For calculating inertia
        double sum_xy = 0; // For calculating cross moment

        // Iterate through the image to find pixels belonging to the current object
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (labeled_image.GetPixel(i, j) == label) {
                    area++;
                    sum_row += i;
                    sum_col += j;

                    double x_diff = i; // y-coordinate
                    double y_diff = j; // x-coordinate
                    sum_xx += (x_diff * x_diff); // Sum of y^2
                    sum_yy += (y_diff * y_diff); // Sum of x^2
                    sum_xy += (x_diff * y_diff); // Sum of xy
                }
            }
        }

        if (area > 0) {
            double center_row = sum_row / area;
            double center_col = sum_col / area;

            // Calculate a, b, and c for E_min and E_max
            double a = sum_xx / area; // Average of y^2
            double b = sum_xy / area; // Average of xy
            double c = sum_yy / area; // Average of x^2

            // Calculate theta1 in radians
            double theta1 = atan2(b, a - c) / 2.0;

            // Calculate E_min
            double e_min = a * sin(theta1) * sin(theta1) - b * sin(theta1) * cos(theta1) + c * cos(theta1) * cos(theta1);

            // Calculate theta2
            double theta2 = theta1 + M_PI / 2.0;

            // Calculate E_max
            double e_max = a * sin(theta2) * sin(theta2) - b * sin(theta2) * cos(theta2) + c * cos(theta2) * cos(theta2);

            // Calculate roundedness
            double roundness = (e_max != 0) ? (e_min / e_max) : 0.0; // Prevent division by zero

            // Store attributes
            attributes.emplace_back(label, center_row, center_col, e_min, area, roundness, theta1 * (180.0 / M_PI)); // Store angle in degrees
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
