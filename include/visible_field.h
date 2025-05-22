#pragma once
#include <vector>
#include <string>
#include <Eigen/Dense>

using namespace Eigen;

struct Point {
    double x, y, z;
};

class obj_visible_plate {    
public:
double scale;
std::vector<std::vector<double>> transp_matrix;
std::vector<std::vector<double>> visible_matrix;

    // Параметры системы
    double width = 10;     // 1 mm
    double height = 10;    // 1 mm
    std::vector<unsigned int> number_of_points{2048, 2048};
    double k = 2*M_PI / 555e-9;
    double sin_alpha = sin(M_PI / 18);

    obj_visible_plate();
    explicit obj_visible_plate(double scale);
    
    bool read_intensity_matrix(const std::string& filename);
    bool update_visible_matrix(double x, double y, double z);
    MatrixXd fftshift(const MatrixXd& matrix);
};