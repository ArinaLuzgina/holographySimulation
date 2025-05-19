#ifndef visible_field
#define visible_field

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <omp.h>

class obj_visible_plate{

public:
    double scale;
    double lamb = 555 * 1e-9;
    double k = 2 * M_PI / lamb;
    std::vector<std::vector<double>> transp_matrix;
    std::vector<std::vector<double>> visible_matrix;

    obj_visible_plate();
    obj_visible_plate(double scale);
    bool read_intensity_matrix(const std::string& filename);
    bool update_visible_matrix(double x, double y, double z);
};


#endif