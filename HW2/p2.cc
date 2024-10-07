#include <iostream>
#include <vector>
#include <unordered_map>
#include "image.h"

using namespace std;
using namespace ComputerVisionProjects;

// Helper function to find the root label (for equivalence resolution)
int findLabel(int label, unordered_map<int, int>& label_equiv) {
    while (label != label_equiv[label]) {
        label = label_equiv[label];
    }
    return label;
}

// Helper function to union two labels
void unionLabels(int label1, int label2, unordered_map<int, int>& label_equiv) {
    int root1 = findLabel(label1, label_equiv);
    int root2 = findLabel(label2, label_equiv);
    if (root1 != root2) {
        label_equiv[root2] = root1; // Union by setting one root to another
    }
}

void SegmentImage(const Image &input_image, Image &output_image) {
    int current_label = 1; // Start labeling from 1
    unordered_map<int, int> label_equiv; // Tracks label equivalences

    int rows = input_image.num_rows();
    int cols = input_image.num_columns();
    output_image.AllocateSpaceAndSetSize(rows, cols);
    output_image.SetNumberGrayLevels(255); // Max gray level for labeled image

    // First pass: Assign provisional labels
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (input_image.GetPixel(i, j) == 0) { // Background pixel
                output_image.SetPixel(i, j, 0);
                continue;
            }

            int left_label = (j > 0) ? output_image.GetPixel(i, j - 1) : 0;
            int top_label = (i > 0) ? output_image.GetPixel(i - 1, j) : 0;

            if (left_label == 0 && top_label == 0) {
                // New object, assign a new label
                output_image.SetPixel(i, j, current_label);
                label_equiv[current_label] = current_label; // Initialize equivalence map
                ++current_label;
            } else if (left_label != 0 && top_label == 0) {
                // Label according to left neighbor
                output_image.SetPixel(i, j, left_label);
            } else if (top_label != 0 && left_label == 0) {
                // Label according to top neighbor
                output_image.SetPixel(i, j, top_label);
            } else {
                // Both neighbors are labeled, assign the smaller label and record equivalence
                int min_label = min(left_label, top_label);
                output_image.SetPixel(i, j, min_label);
                if (left_label != top_label) {
                    unionLabels(left_label, top_label, label_equiv);
                }
            }
        }
    }

    // Second pass: Resolve label equivalences
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int label = output_image.GetPixel(i, j);
            if (label != 0) {
                output_image.SetPixel(i, j, findLabel(label, label_equiv));
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_binary_image.pgm> <output_labeled_image.pgm>" << std::endl;
        return 1;
    }

    const std::string input_filename = argv[1];
    const std::string output_filename = argv[2];

    Image binary_image;
    if (!ReadImage(input_filename, &binary_image)) {
        std::cerr << "Error reading binary image." << std::endl;
        return 1;
    }

    Image labeled_image;
    SegmentImage(binary_image, labeled_image);

    if (!WriteImage(output_filename, labeled_image)) {
        std::cerr << "Error writing labeled image." << std::endl;
        return 1;
    }

    std::cout << "Labeled image saved as: " << output_filename << std::endl;
    return 0;
}
