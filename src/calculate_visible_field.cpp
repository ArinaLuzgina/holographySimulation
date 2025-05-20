#include "visible_field.h"

obj_visible_plate::obj_visible_plate(): scale(1e-6){}

obj_visible_plate::obj_visible_plate(double scale) : scale(scale) {}


bool obj_visible_plate::read_intensity_matrix(const std::string& filename) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        // Заменяем разделители на пробелы
        std::replace(line.begin(), line.end(), ';', ' ');
        
        std::vector<double> row;
        std::istringstream iss(line);
        double value;
        
        while (iss >> value) {
            row.push_back(value);
        }
        
        if (!row.empty()) {
            transp_matrix.push_back(row);
            visible_matrix.push_back(row);
        }
    }
    
    file.close();
    return true;
}

bool obj_visible_plate::update_visible_matrix(double x, double y, double z) {
    const size_t rows = visible_matrix.size();
    if (rows == 0) return false;
    const size_t cols = visible_matrix[0].size();
    if (transp_matrix.size() != rows || transp_matrix[0].size() != cols) return false;

    // Precompute constants
    const double stepx = width * scale / number_of_points[1];
    const double stepy = height * scale / number_of_points[0];
    const double pre_x = x  - width * scale / 2.0;
    const double pre_y = y - height * scale / 2.0; // Возможна ошибка: должно быть height?
    const double pre_z = z;
    const double k_sin_alpha = k * sin_alpha;

    // Precompute delta1 matrix
    std::vector<std::vector<double>> delta1_precomputed(rows, std::vector<double>(cols));
    for (size_t y_t = 0; y_t < rows; ++y_t) {
        for (size_t x_t = 0; x_t < cols; ++x_t) {
            const double tx = x_t * stepx;
            const double ty = y_t * stepy;
            delta1_precomputed[y_t][x_t] = tx * k_sin_alpha;
        }
    }

    // Main processing
    double I_max = 0.0;
    std::vector<std::vector<double>> visible_field_m(rows, std::vector<double>(cols));

    #pragma omp parallel for reduction(max:I_max) schedule(dynamic)
    for (size_t y_v = 0; y_v < rows; ++y_v) {
        for (size_t x_v = 0; x_v < cols; ++x_v) { 
            const double dx0 = pre_x + x_v * stepx;
            const double dy0 = pre_y + y_v * stepy; 
            const double r_0 = std::sqrt(dx0*dx0 + dy0*dy0 + pre_z*pre_z);
            
            double I_res = 0.0;
            for (size_t y_t = 0; y_t < rows; ++y_t) {
                const double dy_step = y_t * stepx;
                for (size_t x_t = 0; x_t < cols; ++x_t) {
                    const double dx = dx0 - x_t * stepx;
                    const double dy = dy0 - dy_step;
                    const double r = std::sqrt(dx*dx + dy*dy + pre_z*pre_z);
                    
                    const double delta = k * (r - r_0) - delta1_precomputed[y_t][x_t];
                    I_res += transp_matrix[y_t][x_t] * std::cos(delta);
                }
            }
            
            visible_field_m[y_v][x_v] = I_res;
            I_max = std::max(I_max, std::fabs(I_res));
        }
    }

    // Normalization
    const double inv_I_max = 1.0 / I_max;
    #pragma omp parallel for
    for (size_t y_v = 0; y_v < rows; ++y_v) {
        for (size_t x_v = 0; x_v < cols; ++x_v) {
            visible_matrix[y_v][x_v] = visible_field_m[y_v][x_v] * inv_I_max;
        }
    }
    
    return true;
}

// bool obj_visible_plate::readPointsFromFile(const std::string& name, char delimiter) {
//     // Очищаем предыдущие данные
//     geom_matrix.clear();

//     // Чтение точек из _points.txt
//     const std::string points_file = name + "_points.txt";
//     std::ifstream pfile(points_file);
//     if (!pfile.is_open()) {
//         std::cerr << "Error: Can't open points file " << points_file << "\n";
//         return false;
//     }

//     std::string line;
//     while (std::getline(pfile, line)) {
//         std::replace(line.begin(), line.end(), delimiter, ' ');
//         std::istringstream iss(line);
//         Point p;
//         if (!(iss >> p.x >> p.y >> p.z)) {
//             std::cerr << "Error: Invalid point format in line: " << line << "\n";
//             return false;
//         }
//         geom_matrix.push_back(p);
//     }
//     pfile.close();

//     return true;
// }   