#ifndef hologram_prep
#define hologram_prep

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <omp.h>
#include "geometry.h"

class obj_plate {

public:
    double scale; 
    std::vector<unsigned int> number_of_points;
    double cos_alpha = cos(M_PI / 6);
    double I = 1.0;
    double lamb = 555 * 1e-9;
    double k = 2 * M_PI / lamb;
    std::vector<std::vector<double>> intensity_matrix;
    std::vector<Point> geom_matrix;
    std::vector<std::vector<double>> surfaces;
    std::vector<std::vector<Point>> vertexes; // один элемент это набор из 4-х точек, задающих поверхность


    obj_plate();
    obj_plate(double scale, const std::vector<unsigned int>& number_of_points);
    
    bool readDataFromFile(const std::string& name, char delimiter = ';');
    bool check_point_raw(double u, double s, double a, double b, double g);
    bool calculate_intensity_matrix();
    double calculate_intensity_from_obj(double x, double y);
    bool saveIntensityToFile(const std::string& filename, char delimiter = ';');
    ~obj_plate() = default;

};

#endif