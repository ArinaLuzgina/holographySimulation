#include <vector>
#include <random>
#include <omp.h>

#include "graphics.h"
#include "visible_field.h"

// Creates and returns a matrix (vector of vectors) of the specified size,
// initialized to zeros.
std::vector<std::vector<double>> createIntensity(std::size_t rows, std::size_t cols) {
    return std::vector<std::vector<double>>(rows, std::vector<double>(cols, 0.0));
}

// Fills the given matrix with random double values in the range [0, 1).
// Uses a Mersenne Twister engine and uniform real distribution.
void fillRandom(std::vector<std::vector<double>>& intensity) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (auto& row : intensity) {
        for (auto& val : row) {
            val = dist(gen);
        }
    }
}



int main() {

    // Creating intensity distribution

    int n = 256;
    auto intensity = createIntensity(n, n); 
    // fillRandom(intensity);
    std::vector<unsigned int> np = {n, n};
    obj_visible_plate v_plate = obj_visible_plate(1, 10.0, 10.0, np, 0);
    v_plate.read_intensity_matrix("intensity_p.txt");

    
    runSimulation(intensity, v_plate);

    return 0;
}

