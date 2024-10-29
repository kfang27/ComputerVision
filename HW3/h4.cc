#include "image.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

using namespace std;
using namespace ComputerVisionProjects;

#define M_PI 3.14159265358979323846

// Function to apply threshold to the Hough space
void ApplyThreshold(vector<vector<int>> &accumulator, int rho_bins, int theta_bins, int threshold) {
    for (int r = 0; r < rho_bins; ++r) {
        for (int t = 0; t < theta_bins; ++t) {
            if (accumulator[r][t] < threshold) {
                accumulator[r][t] = 0;
            }
        }
    }
}

// Function to calculate weighted center of bright areas in Hough space
void CalculateWeightedCenter(const vector<vector<int>> &accumulator, int rho_bins, int theta_bins, vector<pair<int, int>> &line_parameters) {
    for (int r = 0; r < rho_bins; ++r) {
        for (int t = 0; t < theta_bins; ++t) {
            if (accumulator[r][t] > 0) {
                // Weighted center calculations
                int weighted_sum_x = 0, weighted_sum_y = 0, total_weight = 0;
                
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dt = -1; dt <= 1; ++dt) {
                        int rr = r + dr, tt = t + dt;
                        if (rr >= 0 && rr < rho_bins && tt >= 0 && tt < theta_bins && accumulator[rr][tt] > 0) {
                            int weight = accumulator[rr][tt];
                            weighted_sum_x += tt * weight;
                            weighted_sum_y += rr * weight;
                            total_weight += weight;
                        }
                    }
                }
                
                if (total_weight > 0) {
                    int center_x = weighted_sum_x / total_weight;
                    int center_y = weighted_sum_y / total_weight;
                    line_parameters.emplace_back(center_y, center_x);
                }
            }
        }
    }
}

// Function to draw detected lines on the output image
void DrawLines(Image &image, const vector<pair<int, int>> &line_parameters, int max_rho) {
    int width = image.num_columns();
    int height = image.num_rows();

    for (const auto &params : line_parameters) {
        int rho = params.first - max_rho;
        double theta = params.second * M_PI / 180.0;

        // Convert polar coordinates (rho, theta) to Cartesian line
        double cos_theta = cos(theta);
        double sin_theta = sin(theta);

        for (int x = 0; x < width; ++x) {
            int y = static_cast<int>((rho - x * cos_theta) / sin_theta);
            if (y >= 0 && y < height) {
                image.SetPixel(y, x, 100);  // Draw line pixel in white
            }
        }

        for (int y = 0; y < height; ++y) {
            int x = static_cast<int>((rho - y * sin_theta) / cos_theta);
            if (x >= 0 && x < width) {
                image.SetPixel(y, x, 100);  // Draw line pixel in white
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 5) {
        std::cout << "Usage: " << argv[0] << " {input original gray-level image} {input Hough-voting array} {input Hough threshold value} {output gray-level line image}\n";
        return 0;
    }

    const string input_filename(argv[1]);
    const string voting_array_filename(argv[2]);
    const int threshold = stoi(argv[3]);
    const string output_filename(argv[4]);

    // Load original gray-level image
    Image original_image;
    if (!ReadImage(input_filename, &original_image)) {
        cerr << "Error reading original image.\n";
        return 1;
    }

    int width = original_image.num_columns();
    int height = original_image.num_rows();
    int max_rho = static_cast<int>(sqrt(width * width + height * height));
    int theta_bins = 180;
    int rho_bins = max_rho * 2;

    // Read the Hough voting array
    vector<vector<int>> accumulator(rho_bins, vector<int>(theta_bins, 0));
    ifstream voting_array_file(voting_array_filename);
    if (!voting_array_file) {
        cerr << "Error opening voting array file.\n";
        return 1;
    }

    for (int r = 0; r < rho_bins; ++r) {
        for (int t = 0; t < theta_bins; ++t) {
            voting_array_file >> accumulator[r][t];
        }
    }

    // Threshold the Hough space
    ApplyThreshold(accumulator, rho_bins, theta_bins, threshold);

    // Segment bright areas and calculate weighted centers (line parameters)
    vector<pair<int, int>> line_parameters;
    CalculateWeightedCenter(accumulator, rho_bins, theta_bins, line_parameters);

    // Draw lines on the original image
    DrawLines(original_image, line_parameters, max_rho);

    // Write the output image with lines
    if (!WriteImage(output_filename, original_image)) {
        cerr << "Error writing output line image.\n";
        return 1;
    }

    cout << "Line detection completed. Output saved to " << output_filename << ".\n";
    return 0;
}
