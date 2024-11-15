#include "image.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>

using namespace ComputerVisionProjects;

// To compute the normal vector at a point (x, y) on the sphere's surface
void ComputeNormal(int x, int y, int x_center, int y_center, double radius, double &nx, double &ny, double &nz) {
    nx = (x - x_center) / radius;
    ny = (y - y_center) / radius;
    nz = std::sqrt(1.0 - nx * nx - ny * ny);
}

// To find the brightest pixel's coordinates and brightness in an image
void FindBrightestPixel(const Image &image, int &x, int &y, int &brightness) {
    brightness = -1;
    for (size_t i = 0; i < image.num_rows(); ++i) {
        for (size_t j = 0; j < image.num_columns(); ++j) {
            int pixel_value = image.GetPixel(i, j);
            if (pixel_value > brightness) {
                brightness = pixel_value;
                x = j;
                y = i;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " {input parameters filename} {input sphere image 1 filename} {input sphere image 2 filename} {input sphere image 3 filename} {output directions filename}\n";
        return 1;
    }

    const std::string parameters_filename(argv[1]);
    const std::string image_filename1(argv[2]);
    const std::string image_filename2(argv[3]);
    const std::string image_filename3(argv[4]);
    const std::string output_filename(argv[5]);

    // This part reads sphere center and radius from parameters file
    int x_center, y_center;
    double radius;
    std::ifstream param_file(parameters_filename);
    if (!param_file || !(param_file >> x_center >> y_center >> radius)) {
        std::cerr << "Error reading parameters file.\n";
        return 1;
    }
    param_file.close();

    std::vector<std::string> image_filenames = {image_filename1, image_filename2, image_filename3};
    std::vector<std::vector<double>> light_directions;

    for (const auto &filename : image_filenames) {
        Image image;
        if (!ReadImage(filename, &image)) {
            std::cerr << "Error reading image: " << filename << "\n";
            return 1;
        }

        int x, y, brightness;
        FindBrightestPixel(image, x, y, brightness);

        double nx, ny, nz;
        ComputeNormal(x, y, x_center, y_center, radius, nx, ny, nz);

        nx *= brightness;
        ny *= brightness;
        nz *= brightness;

        light_directions.push_back({nx, ny, nz});
    }

    std::ofstream output_file(output_filename);
    if (!output_file) {
        std::cerr << "Error opening output file.\n";
        return 1;
    }
    for (const auto &direction : light_directions) {
        output_file << direction[0] << " " << direction[1] << " " << direction[2] << "\n";
    }
    output_file.close();

    std::cout << "Light source directions written to " << output_filename << "\n";
    return 0;
}
