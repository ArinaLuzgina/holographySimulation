#include "visible_field.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <fftw3.h>

obj_visible_plate::obj_visible_plate() : scale(1e-6) {}
obj_visible_plate::obj_visible_plate(double scale) : scale(scale) {}

bool obj_visible_plate::read_intensity_matrix(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    transp_matrix.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::replace(line.begin(), line.end(), ';', ' ');
        std::vector<double> row;
        double val;
        std::istringstream iss(line);
        while (iss >> val) row.push_back(val);
        if (!row.empty()) transp_matrix.push_back(row);
    }
    return true;
}

MatrixXd obj_visible_plate::fftshift(const MatrixXd& matrix) {
    int rows = matrix.rows();
    int cols = matrix.cols();
    MatrixXd shifted(rows, cols);
    int half_r = rows/2, half_c = cols/2;
    shifted << matrix.bottomRightCorner(half_r, half_c),
               matrix.bottomLeftCorner(half_r, half_c),
               matrix.topRightCorner(half_r, half_c),
               matrix.topLeftCorner(half_r, half_c);
    return shifted;
}

bool obj_visible_plate::update_visible_matrix(double x, double y, double z) {
    //scale = 1e-4;
    const int rows = transp_matrix.size();
    const int cols = transp_matrix[0].size();
    
    // Precompute parameters
    const double stepx = width * scale / cols;
    const double stepy = height * scale / rows;
    const double k_sin_alpha = k * sin_alpha;

    // FFT preparation
    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*rows*cols);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*rows*cols);
    fftw_plan plan = fftw_plan_dft_2d(rows, cols, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    // Fill input
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int idx = i*cols + j;
            const double x_point = j*stepx;
            const double y_point = i*stepy;
            const double phase = k_sin_alpha * sqrt(x_point*x_point + y_point*y_point);
            
            in[idx][0] = transp_matrix[i][j] * cos(phase);
            in[idx][1] = transp_matrix[i][j] * sin(phase);        }
    }

    // Execute FFT
    fftw_execute(plan);

    // Process output
    MatrixXd reconstructed(rows, cols);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            const double x_point = j*stepx;
            const double y_point = i*stepy;
            
            // Calculate distance to camera
            const double dx = x  - x_point;
            const double dy = y - y_point;
            const double dz = z ;
            const double r = sqrt(dx*dx + dy*dy + dz*dz);
            reconstructed(i,j) = out[i*cols+j][0]*out[i*cols+j][0] * cos(-k * r) * cos(-k * r) 
                               + out[i*cols+j][1]*out[i*cols+j][1] * sin(-k * r) * sin(-k * r) ;
        }
    }

    // Apply fftshift and normalize
    // Apply fftshift and normalize
    reconstructed = fftshift(reconstructed);
    
    // Новая нормировка
    // double max_val = reconstructed.maxCoeff();
    // if (max_val < 1e-9) max_val = 1e-9; // Защита от нуля
    // reconstructed.array() *= (100.0 / max_val); // Умножаем на 1000/max_val
    reconstructed = (reconstructed.array() + 1e-12* scale).log10();
    reconstructed = (reconstructed.array() - reconstructed.minCoeff()) / 
                   (reconstructed.maxCoeff() - reconstructed.minCoeff());
    
    // Обрезка значений до [0, 1] (опционально)
    //reconstructed = reconstructed.cwiseMax(0.0).cwiseMin(1.0);
    // Update visible matrix
    
    visible_matrix.resize(rows, std::vector<double>(cols));
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            visible_matrix[i][j] = reconstructed(i,j);

    // Cleanup
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
    return true;
}