#include "visible_field.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <fftw3.h>
#include <iostream>
#include <limits>

obj_visible_plate::obj_visible_plate() : scale(1e-6), width(10), height(10), number_of_points{1024, 1024}, cos_alpha(cos(M_PI / 18)), sin_alpha(sin(M_PI / 18)) {}
obj_visible_plate::obj_visible_plate(double scale, double width, double height, std::vector<unsigned int> number_of_points, double alpha) : scale(scale), width(width), height(height), number_of_points(number_of_points), sin_alpha(sin(alpha)), cos_alpha(cos(alpha)) {}

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
    visible_matrix.resize(transp_matrix.size(), std::vector<double>(transp_matrix[0].size(), 0.0));
    double I_max = 0.0;
    for(int i = 0; i < transp_matrix.size(); i ++){
        for(int j = 0; j < transp_matrix[0].size(); j ++){
            if(transp_matrix[i][j] > I_max){
                I_max = transp_matrix[i][j];
            }
        }
    }
    for(int i = 0; i < transp_matrix.size(); i ++){
        for(int j = 0; j < transp_matrix[0].size(); j ++){
            transp_matrix[i][j] /= I_max;
        }
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
void obj_visible_plate::enhance_contrast() {
    double min_val = std::numeric_limits<double>::max();
    double max_val = std::numeric_limits<double>::lowest();

    // Находим реальные min и max в матрице
    for (const auto& row : visible_matrix) {
        for (double val : row) {
            if (val < min_val) min_val = val;
            if (val > max_val) max_val = val;
        }
    }

    // Проверка на случай одинаковых значений
    if (min_val >= max_val) return;

    // Применяем линейное растяжение
    const double range = max_val - min_val;
    for (auto& row : visible_matrix) {
        for (double& val : row) {
            val = (val - min_val) / range;
        }
    }
}

bool obj_visible_plate::update_visible_matrix(double x, double y, double z) {
    const int rows = transp_matrix.size();
    const int cols = transp_matrix[0].size();
    const double stepx = width * scale / cols;
    const double stepy = height * scale / rows;
    const double left_x = x - rows * 1.0 / 2 * stepx;
    const double left_y = y - cols * 1.0 / 2 * stepy;
    double I_max = 0.0;
    double s = 1.0 / (rows * cols);
    // Распараллеливание основного блока вычислений
    #pragma omp parallel for collapse(2) reduction(max:I_max)
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            double x_0 = left_x + stepx * i;
            double y_0 = left_y + stepy * j;
            double I_res = 0.0;

            for (int m = 0; m < rows; m++) {
                for (int n = 0; n < cols; n++) {
                    double dx = m * stepx - x_0;
                    double dy = n * stepy - y_0;
                    double r = sqrt(dx*dx + dy*dy + z*z) - m * stepx * sin_alpha ;//- sqrt(x_0 * x_0 + y_0 * y_0 + z * z);
                    I_res += transp_matrix[m][n] *transp_matrix[m][n] * 1.0 *(1 + cos(k * r)) *s;
                    //std::cout << I_res << std::endl;
                }
            }
            //std::cout << I_res <<std::endl;
            
            visible_matrix[i][j] = I_res;
            if (I_res > I_max) {
                I_max = I_res;
            }
            //std::cout << visible_matrix[i][j] << std::endl;

        }
    }

    // Нормализация результатов
    if (I_max > scale * 1e-9) { // Убрана лишняя точка с запятой
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                visible_matrix[i][j] /= I_max;
            }
        }
    }
    enhance_contrast();
    std::cout << "I has finished my calc\n";

    return true;
}

// bool obj_visible_plate::update_visible_matrix(double x, double y, double z) {
//     //scale = 1e-4;
//     const int rows = transp_matrix.size();
//     const int cols = transp_matrix[0].size();
    
//     // Precompute parameters
//     const double stepx = width * scale / cols;
//     const double stepy = height * scale / rows;

//     // FFT preparation
//     fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*rows*cols);
//     fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*rows*cols);
//     fftw_plan plan = fftw_plan_dft_2d(rows, cols, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

//     // Fill input
//     for (int i = 0; i < rows; ++i) {
//         for (int j = 0; j < cols; ++j) {
//             int idx = i*cols + j;
//             const double x_point = j*stepx;
//             const double y_point = i*stepy;
//             const double phase = k * sin_alpha * x_point;
            
//             in[idx][0] = transp_matrix[i][j] * cos(phase);
//             in[idx][1] = transp_matrix[i][j] * sin(phase);        }
//     }

//     // Execute FFT
//     fftw_execute(plan);

//     // Process output
//     MatrixXd reconstructed(rows, cols);
//     for (int i = 0; i < rows; ++i) {
//         for (int j = 0; j < cols; ++j) {
//             const double x_point = j*stepx;
//             const double y_point = i*stepy;
            
//             // Calculate distance to camera
//             const double dx = x  - x_point;
//             const double dy = y - y_point;
//             const double dz = z ;
//             const double r = sqrt(dx*dx + dy*dy + dz*dz);
//             reconstructed(i,j) = out[i*cols+j][0]*out[i*cols+j][0] * cos(-k * r) * cos(-k * r) 
//                                + out[i*cols+j][1]*out[i*cols+j][1] * sin(-k * r) * sin(-k * r) ;
//         }
//     }

//     // Apply fftshift and normalize
//     // Apply fftshift and normalize
//     reconstructed = fftshift(reconstructed);
    
//     // Новая нормировка
//     // double max_val = reconstructed.maxCoeff();
//     // if (max_val < 1e-9) max_val = 1e-9; // Защита от нуля
//     // reconstructed.array() *= (100.0 / max_val); // Умножаем на 1000/max_val
//     reconstructed = (reconstructed.array() + 1e-12* scale).log10();
//     reconstructed = (reconstructed.array() - reconstructed.minCoeff()) / 
//                    (reconstructed.maxCoeff() - reconstructed.minCoeff());
    
//     // Обрезка значений до [0, 1] (опционально)
//     //reconstructed = reconstructed.cwiseMax(0.0).cwiseMin(1.0);
//     // Update visible matrix
    
//     visible_matrix.resize(rows, std::vector<double>(cols));
//     for (int i = 0; i < rows; ++i)
//         for (int j = 0; j < cols; ++j)
//             visible_matrix[i][j] = reconstructed(i,j);

//     // Cleanup
//     fftw_destroy_plan(plan);
//     fftw_free(in);
//     fftw_free(out);
//     return true;
// }