#include "hologram_prep.h"

obj_plate::obj_plate()
    :
    scale(1e-6),
    number_of_points{100, 100}
    {
        intensity_matrix.resize(number_of_points[0], std::vector<double>(number_of_points[1]));
    }

obj_plate::obj_plate(double scale,const std::vector<unsigned int>& number_of_points)
    : 
    scale(scale),
    number_of_points(number_of_points)
    {
        intensity_matrix.resize(number_of_points[0], std::vector<double>(number_of_points[1]));
    }

    
    
bool obj_plate::readDataFromFile(int rows, int cols, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;

    for (int i = 0; i < rows; ++i) {
        if (!std::getline(file, line)) {
            throw std::runtime_error("Not enough rows in file. Expected: " + std::to_string(rows));
            return false;
        }

        std::vector<Point> rowData;
        std::istringstream lineStream(line);
        std::string group;

        for (int j = 0; j < cols; ++j) {
            if (!std::getline(lineStream, group, ';')) {
                throw std::runtime_error("Not enough groups in row " + std::to_string(i + 1));
                return false;
            }

            // Удаление пробелов в начале и конце группы
            group.erase(0, group.find_first_not_of(" \t"));
            group.erase(group.find_last_not_of(" \t") + 1);

            std::istringstream groupStream(group);
            Point point;
            if (!(groupStream >> point.x >> point.y >> point.z)) {
                throw std::runtime_error("Invalid format in group. Row: " 
                    + std::to_string(i + 1) + ", Col: " + std::to_string(j + 1));
                return false;
            }

            // Проверка на лишние данные в группе
            std::string leftover;
            if (groupStream >> leftover) {
                throw std::runtime_error("Extra data in group. Row: " 
                    + std::to_string(i + 1) + ", Col: " + std::to_string(j + 1));
                return false;
            }

            rowData.push_back(point);
        }

        geom_matrix.push_back(rowData);
    }

    return true;
}
double obj_plate::calculate_intensity_from_obj(double x, double y) {
    double I_res = 0.0;

    #pragma omp parallel for reduction(+: I_res) num_threads(14)

    for(size_t yi = 0; yi < geom_matrix.size(); yi++) {
        for(size_t xi = 0; xi < geom_matrix[0].size(); xi++) {
            const double j = geom_matrix[yi][xi].x;
            const double i_val = geom_matrix[yi][xi].y;
            const double r_sq = (j - x) * (j - x) + (i_val - y) * (i_val - y) + geom_matrix[yi][xi].z * geom_matrix[yi][xi].z;
            const double delta = sqrt(r_sq) - geom_matrix[yi][xi].z / cos_alpha;
            I_res += 2 * I * (1 + cos(k * delta));
        }
    }
    return I_res;
}

bool obj_plate::calculate_intensity_matrix(){
    double I_max = 0.0;

    #pragma omp parallel for collapse(2) reduction(max: I_max) num_threads(14)

    for(size_t yp = 0; yp < number_of_points[0]; yp++){
        for(size_t xp = 0; xp < number_of_points[1]; xp ++){
            intensity_matrix[yp][xp] = calculate_intensity_from_obj(xp * scale, yp * scale);
            #pragma omp critical
            if(I_max < intensity_matrix[yp][xp]){
                I_max = intensity_matrix[yp][xp];
            }
        }
    }
    #pragma omp parallel for collapse(2) num_threads(14)
    for(size_t yp = 0; yp < number_of_points[0]; yp++){
        for(size_t xp = 0; xp < number_of_points[1]; xp ++){
            intensity_matrix[yp][xp] /= I_max;
        }
    }
    return true;
}


bool obj_plate::saveIntensityToFile(const std::string& filename, char delimiter) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing\n";
        return false;
    }

    // Записываем матрицу
    for (const auto& row : intensity_matrix) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i != row.size() - 1) {
                file << delimiter;
            }
        }
        file << '\n';
    }

    file.close();
    return true;
}


