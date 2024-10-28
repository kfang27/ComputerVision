#include "image.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

using namespace std;
using namespace ComputerVisionProjects;

#define M_PI 3.14159265358979323846

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " {input binary edge image} {output gray-level Hough image} {output Hough-voting array}\n";
        return 0;
    }

    const string input_filename(argv[1]);
    const string output_filename(argv[2]);
    const string voting_array_filename(argv[3]);

    // Load the input binary edge image
    Image edge_image;
    if (!ReadImage(input_filename, &edge_image)) {
        cerr << "Error reading input edge image.\n";
        return 1;
    }

    int width = edge_image.num_columns();
    int height = edge_image.num_rows();
    int max_rho = static_cast<int>(std::sqrt(width * width + height * height));
    int theta_bins = 180;  // Dividing [0, π] into 180 bins (1-degree increments)
    int rho_bins = max_rho * 2;

    // Accumulator array for Hough votes
    vector<vector<int>> accumulator(rho_bins, vector<int>(theta_bins, 0));

    // Hough Transform: Vote in the accumulator array
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (edge_image.GetPixel(y, x) > 0) {  // If it's an edge point
                for (int t = 0; t < theta_bins; ++t) {
                    double theta = t * M_PI / theta_bins;
                    int rho = static_cast<int>(x * cos(theta) + y * sin(theta)) + max_rho;
                    if (rho >= 0 && rho < rho_bins) {
                        accumulator[rho][t]++;
                    }
                }
            }
        }
    }

    // Create the Hough image based on the accumulator array
    Image hough_image;
    hough_image.AllocateSpaceAndSetSize(theta_bins, rho_bins); // Ensure width and height are correctly set
    hough_image.SetNumberGrayLevels(255);

    int max_votes = 0;
    for (int r = 0; r < rho_bins; ++r) {
        for (int t = 0; t < theta_bins; ++t) {
            max_votes = std::max(max_votes, accumulator[r][t]);
        }
    }

    // Normalize and set pixel intensity in Hough space image
    for (int r = 0; r < rho_bins; ++r) {
        for (int t = 0; t < theta_bins; ++t) {
            int intensity = static_cast<int>(255.0 * accumulator[r][t] / max_votes);
            hough_image.SetPixel(t, r, intensity); // Use (t, r) to set the pixel correctly
        }
    }

    // Write Hough space image
    if (!WriteImage(output_filename, hough_image)) {
        cerr << "Error writing Hough image.\n";
        return 1;
    }

    // Write voting array to output file
    ofstream voting_array_file(voting_array_filename);
    if (!voting_array_file) {
        cerr << "Error opening voting array output file.\n";
        return 1;
    }
    for (int r = 0; r < rho_bins; ++r) {
        for (int t = 0; t < theta_bins; ++t) {
            voting_array_file << accumulator[r][t] << " ";
        }
        voting_array_file << "\n";
    }

    cout << "Hough Transform completed. Output saved to " << output_filename << " and " << voting_array_filename << ".\n";
    return 0;
}
