#include <vector>
#include <random>
#include <omp.h>

#include "graphics.h"


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
    auto intensity = createIntensity(200, 200); 
    fillRandom(intensity);
    
    runSimulation(intensity);

    return 0;
}

