#include "image.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#define M_PI 3.14159265358979323846
using namespace ComputerVisionProjects;
using namespace std;

struct HoughPoint {
    int rho;
    int theta;
    int intensity;
};

// Reads the Hough voting array from a file
bool ReadHoughArray(const string &filename, vector<vector<int>> &hough_space, int theta_bins) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open Hough array file: " << filename << endl;
        return false;
    }

    int value;
    vector<int> temp_row;

    // Read until the end of the file
    while (file >> value) {
        temp_row.push_back(value);
        // When the row reaches the expected number of columns, push it to hough_space
        if (temp_row.size() == theta_bins) {
            hough_space.push_back(temp_row);
            temp_row.clear(); // Clear for the next row
        }
    }

    // Handle any remaining values in temp_row
    if (!temp_row.empty()) {
        cerr << "Warning: Last row has fewer columns than expected. Discarding." << endl;
    }

    file.close();

    // Check for empty hough_space or inconsistent row sizes
    if (hough_space.empty()) {
        cerr << "Error: No data was read from the file." << endl;
        return false;
    }

    // Verify that all rows have the correct number of columns
    for (const auto& row : hough_space) {
        if (row.size() != theta_bins) {
            cerr << "Error: Inconsistent number of columns in the Hough array." << endl;
            return false;
        }
    }

    return true;
}

// Threshold the Hough space and collect points above threshold
void ThresholdHoughSpace(const vector<vector<int>> &hough_space, int threshold, vector<HoughPoint> &line_centers) {
    int rows = hough_space.size();
    int cols = hough_space[0].size();
    for (int rho = 0; rho < rows; ++rho) {
        for (int theta = 0; theta < cols; ++theta) {
            if (hough_space[rho][theta] >= threshold) {
                line_centers.push_back({rho, theta, hough_space[rho][theta]});
            }
        }
    }
}

// Calculate weighted centers of brightness areas in the Hough space
void CalculateWeightedCenters(const vector<HoughPoint> &points, vector<pair<int, int>> &weighted_centers) {
    int total_rho = 0, total_theta = 0, total_intensity = 0;
    
    for (const auto &pt : points) {
        total_rho += pt.rho * pt.intensity;
        total_theta += pt.theta * pt.intensity;
        total_intensity += pt.intensity;
    }
    
    if (total_intensity != 0) {
        int avg_rho = total_rho / total_intensity;
        int avg_theta = total_theta / total_intensity;
        weighted_centers.emplace_back(avg_rho, avg_theta);
    }
}

// Draw the detected lines on the image
void DrawDetectedLines(Image &image, const vector<pair<int, int>> &line_centers) {
    const int color = 255;  // Maximum gray level for visibility
    
    for (const auto &center : line_centers) {
        int rho = center.first;
        int theta = center.second;
        double rad_theta = theta * M_PI / 180.0;  // Convert theta to radians
        
        // Calculate endpoints for the line based on the polar coordinates
        int x0 = static_cast<int>(rho * cos(rad_theta));
        int y0 = static_cast<int>(rho * sin(rad_theta));
        
        int x1 = x0 + 1000 * -sin(rad_theta);  // Adjust the length of the line segment
        int y1 = y0 + 1000 * cos(rad_theta);
        int x2 = x0 - 1000 * -sin(rad_theta);
        int y2 = y0 - 1000 * cos(rad_theta);
        
        DrawLine(x1, y1, x2, y2, color, &image);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " {input original gray–level image} {input Hough–voting array} {input Hough threshold value} {output gray–level line image}" << endl;
        return 1;
    }
    
    const string input_image_file(argv[1]);
    const string hough_array_file(argv[2]);
    const int threshold = stoi(argv[3]);
    const string output_image_file(argv[4]);
    
    Image image;
    if (!ReadImage(input_image_file, &image)) {
        cerr << "Could not read input image: " << input_image_file << endl;
        return 1;
    }
    
    const int theta_bins = 180;  // This should match the value in h3.cc
    vector<vector<int>> hough_space;
    
    if (!ReadHoughArray(hough_array_file, hough_space, theta_bins)) {
        cerr << "Could not read Hough array: " << hough_array_file << endl;
        return 1;
    }
    
    vector<HoughPoint> thresholded_points;
    ThresholdHoughSpace(hough_space, threshold, thresholded_points);
    
    vector<pair<int, int>> line_centers;
    CalculateWeightedCenters(thresholded_points, line_centers);
    
    DrawDetectedLines(image, line_centers);
    
    if (!WriteImage(output_image_file, image)) {
        cerr << "Could not write output image: " << output_image_file << endl;
        return 1;
    }
    
    cout << "Line detection completed successfully!" << endl;
    return 0;
}
