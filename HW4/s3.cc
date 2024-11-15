/*
Name: Kevin Fang
File: s3.cc
Description:
    The program, s3.cc, is supposed to compute the surface normals and albedo of an object's surface
    using three images taken with distinct light sources. Using the direction vectors of 
    each light source from an input file, it calculates surface normals and albedo values 
    at each pixel where the object is visible in all three images (above a specified brightness threshold).

To run this program after compiling:
    ./s3 <input directions filename> <input object image 1 filename> <input object image 2 filename>
    <input object image 3 filename> <{input step parameter (integer greater than 0)>
    <{input threshold parameter (integer greater than 0)> <output normals image filename> <output albedo image filename>

    Ex: ./s3 directions.txt object1.pgm object2.pgm object3.pgm 10 80 normals_output.pgm albedo_output.pgm
*/
#include "image.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

using namespace ComputerVisionProjects;

bool InvertMatrix(const double S[3][3], double S_inv[3][3]) {
    double det = S[0][0] * (S[1][1] * S[2][2] - S[1][2] * S[2][1]) -
                 S[0][1] * (S[1][0] * S[2][2] - S[1][2] * S[2][0]) +
                 S[0][2] * (S[1][0] * S[2][1] - S[1][1] * S[2][0]);

    if (det == 0) {
        std::cerr << "Matrix is singular, cannot invert.\n";
        return false;
    }

    double inv_det = 1.0 / det;

    // Calculating the inverse
    S_inv[0][0] =  (S[1][1] * S[2][2] - S[1][2] * S[2][1]) * inv_det;
    S_inv[0][1] = -(S[0][1] * S[2][2] - S[0][2] * S[2][1]) * inv_det;
    S_inv[0][2] =  (S[0][1] * S[1][2] - S[0][2] * S[1][1]) * inv_det;
    S_inv[1][0] = -(S[1][0] * S[2][2] - S[1][2] * S[2][0]) * inv_det;
    S_inv[1][1] =  (S[0][0] * S[2][2] - S[0][2] * S[2][0]) * inv_det;
    S_inv[1][2] = -(S[0][0] * S[1][2] - S[0][2] * S[1][0]) * inv_det;
    S_inv[2][0] =  (S[1][0] * S[2][1] - S[1][1] * S[2][0]) * inv_det;
    S_inv[2][1] = -(S[0][0] * S[2][1] - S[0][1] * S[2][0]) * inv_det;
    S_inv[2][2] =  (S[0][0] * S[1][1] - S[0][1] * S[1][0]) * inv_det;

    return true;
}

void MultiplyMatrixVector(const double S_inv[3][3], const double I[3], double N[3]) {
    for (int i = 0; i < 3; ++i) {
        N[i] = S_inv[i][0] * I[0] + S_inv[i][1] * I[1] + S_inv[i][2] * I[2];
    }
}

int ScaleTo255(double value, double max_value) {
    return static_cast<int>((value / max_value) * 255.0);
}

int main(int argc, char *argv[]) {
    if (argc != 9) {
        std::cerr << "Usage: " << argv[0]
                  << " {input directions filename} {input object image 1 filename} {input object image 2 filename} {input object image 3 filename} "
                     "{input step parameter} {input threshold parameter} {output normals image filename} {output albedo image filename}\n";
        return 1;
    }

    const std::string directions_filename(argv[1]);
    const std::string image_filename1(argv[2]);
    const std::string image_filename2(argv[3]);
    const std::string image_filename3(argv[4]);
    const int step = std::stoi(argv[5]);
    const int threshold = std::stoi(argv[6]);
    const std::string output_normals_filename(argv[7]);
    const std::string output_albedo_filename(argv[8]);

    Image image1, image2, image3;
    if (!ReadImage(image_filename1, &image1) ||
        !ReadImage(image_filename2, &image2) ||
        !ReadImage(image_filename3, &image3)) {
        std::cerr << "Error reading input images.\n";
        return 1;
    }

    double S[3][3];
    std::ifstream directions_file(directions_filename);
    if (!directions_file) {
        std::cerr << "Error reading directions file.\n";
        return 1;
    }
    for (int i = 0; i < 3; ++i) {
        directions_file >> S[i][0] >> S[i][1] >> S[i][2];
    }

    // Inverting the directions matrix
    double S_inv[3][3];
    if (!InvertMatrix(S, S_inv)) {
        std::cerr << "Error inverting directions matrix.\n";
        return 1;
    }

    Image output_normals = image1;
    Image output_albedo;
    output_albedo.AllocateSpaceAndSetSize(image1.num_rows(), image1.num_columns());

    double max_albedo = 0.0;
    std::vector<std::vector<double>> albedo_values(image1.num_rows(), std::vector<double>(image1.num_columns(), 0.0));

    for (size_t y = 0; y < image1.num_rows(); ++y) {
        for (size_t x = 0; x < image1.num_columns(); ++x) {
            int I1 = image1.GetPixel(y, x);
            int I2 = image2.GetPixel(y, x);
            int I3 = image3.GetPixel(y, x);

            if (I1 > threshold && I2 > threshold && I3 > threshold) {
                double I[3] = {static_cast<double>(I1), static_cast<double>(I2), static_cast<double>(I3)};
                double N[3];
                MultiplyMatrixVector(S_inv, I, N);

                double albedo = std::sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2]);
                if (albedo > max_albedo) max_albedo = albedo;
                albedo_values[y][x] = albedo;

                N[0] /= albedo;
                N[1] /= albedo;
                N[2] /= albedo;

                if (x % step == 0 && y % step == 0) {
                    int nx = static_cast<int>(10 * N[0]);
                    int ny = static_cast<int>(10 * N[1]);
                    output_normals.SetPixel(y, x, 0);

                    // Attempt to draw the needle
                    DrawLine(x, y, x + nx, y + ny, 255, &output_normals);

                }
            } else {
                albedo_values[y][x] = 0;
            }
        }
    }

    for (size_t y = 0; y < output_albedo.num_rows(); ++y) {
        for (size_t x = 0; x < output_albedo.num_columns(); ++x) {
            output_albedo.SetPixel(y, x, ScaleTo255(albedo_values[y][x], max_albedo));
        }
    }

    // For error encounters and messages within terminal
    if (!WriteImage(output_normals_filename, output_normals)) {
        std::cerr << "Error writing normals image.\n";
        return 1;
    }

    if (!WriteImage(output_albedo_filename, output_albedo)) {
        std::cerr << "Error writing albedo image.\n";
        return 1;
    }

    std::cout << "Normals and albedo images created.\n";
    return 0;
}
