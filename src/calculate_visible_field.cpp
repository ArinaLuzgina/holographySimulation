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

    // Проверки безопасности (добавьте свои проверки из предыдущего ответа)
    if (transp_matrix.size() != rows || transp_matrix[0].size() != cols) {
        return false;
    }

    #pragma omp parallel for collapse(2) schedule(static) num_threads(14)
    for (size_t y_t = 0; y_t < rows; y_t++) {
        for (size_t x_t = 0; x_t < cols; x_t++) {
            const double dx = x - x_t * scale;
            const double dy = y - y_t * scale;
            const double r = sqrt(dx*dx + dy*dy + z*z);
            visible_matrix[y_t][x_t] = transp_matrix[y_t][x_t] * cos(k * r);
        }
    }
    
    return true;
}
