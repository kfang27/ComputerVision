#include "image.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>

using namespace ComputerVisionProjects;

// Function to read the light directions from a file
bool ReadLightDirections(const std::string &filename, std::vector<std::vector<double>> &directions) {
    std::ifstream file(filename);
    if (!file) return false;

    double x, y, z;
    while (file >> x >> y >> z) {
        directions.push_back({x, y, z});
    }
    file.close();
    return directions.size() == 3; // Ensure three directions
}

// Function to compute normal and albedo based on light directions and pixel intensities
bool ComputeNormalAndAlbedo(const std::vector<std::vector<double>> &directions,
                            const std::vector<int> &intensities,
                            std::vector<double> &normal, double &albedo) {
    double I[3] = {intensities[0], intensities[1], intensities[2]};
    double L[3][3] = { {directions[0][0], directions[0][1], directions[0][2]},
                       {directions[1][0], directions[1][1], directions[1][2]},
                       {directions[2][0], directions[2][1], directions[2][2]} };
    double L_transpose[3][3];
    
    // Transpose L
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            L_transpose[i][j] = L[j][i];
        }
    }
    
    // Solve for G = L_transpose * I, then normalize G to get the normal
    double G[3] = {L_transpose[0][0]*I[0] + L_transpose[0][1]*I[1] + L_transpose[0][2]*I[2],
                   L_transpose[1][0]*I[0] + L_transpose[1][1]*I[1] + L_transpose[1][2]*I[2],
                   L_transpose[2][0]*I[0] + L_transpose[2][1]*I[1] + L_transpose[2][2]*I[2]};
                   
    // Albedo is the magnitude of G
    albedo = sqrt(G[0]*G[0] + G[1]*G[1] + G[2]*G[2]);
    
    // Normalize G to get the normal
    if (albedo > 0) {
        normal = {G[0]/albedo, G[1]/albedo, G[2]/albedo};
    } else {
        normal = {0, 0, 0}; // Default zero vector for invalid cases
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 9) {
        std::cerr << "Usage: " << argv[0] << " {input directions filename} {input object image 1 filename} {input object image 2 filename} {input object image 3 filename} {input step parameter} {input threshold parameter} {output normals image filename} {output albedo image filename}\n";
        return 1;
    }

    // Parse command line arguments
    const std::string directions_filename(argv[1]);
    const std::string image_filename1(argv[2]);
    const std::string image_filename2(argv[3]);
    const std::string image_filename3(argv[4]);
    const int step = std::stoi(argv[5]);
    const int threshold = std::stoi(argv[6]);
    const std::string output_normals_filename(argv[7]);
    const std::string output_albedo_filename(argv[8]);

    // Read light directions
    std::vector<std::vector<double>> directions;
    if (!ReadLightDirections(directions_filename, directions)) {
        std::cerr << "Error reading directions file.\n";
        return 1;
    }

    // Load images
    Image image1, image2, image3;
    if (!ReadImage(image_filename1, &image1) || !ReadImage(image_filename2, &image2) || !ReadImage(image_filename3, &image3)) {
        std::cerr << "Error reading input images.\n";
        return 1;
    }

    // Prepare output images
    Image normals_image = image1; // Copy of image1 for displaying normals
    Image albedo_image;
    albedo_image.AllocateSpaceAndSetSize(image1.num_rows(), image1.num_columns());

    // Loop over each pixel
    for (int y = 0; y < image1.num_rows(); y++) {
        for (int x = 0; x < image1.num_columns(); x++) {
            // Check visibility in all images
            int i1 = image1.GetPixel(y, x), i2 = image2.GetPixel(y, x), i3 = image3.GetPixel(y, x);
            if (i1 < threshold || i2 < threshold || i3 < threshold) {
                albedo_image.SetPixel(y, x, 0); // Set black for invisible pixels
                continue;
            }

            // Compute normal and albedo
            std::vector<double> normal;
            double albedo;
            ComputeNormalAndAlbedo(directions, {i1, i2, i3}, normal, albedo);
            albedo_image.SetPixel(y, x, std::min(255.0, albedo)); // Scale albedo to [0, 255]

            // For needle map: only plot every N pixels
            if (y % step == 0 && x % step == 0) {
                int x_end = x + static_cast<int>(normal[0] * 10); // Scale by 10 for visibility
                int y_end = y + static_cast<int>(normal[1] * 10);
                DrawLine(x, y, x_end, y_end, 255, &normals_image); // Draw white needle
                normals_image.SetPixel(y, x, 0); // Set gridpoint in black
            }
        }
    }

    // Save output images
    if (!WriteImage(output_normals_filename, normals_image) || !WriteImage(output_albedo_filename, albedo_image)) {
        std::cerr << "Error writing output images.\n";
        return 1;
    }

    std::cout << "Normals and albedo images generated successfully.\n";
    return 0;
}
