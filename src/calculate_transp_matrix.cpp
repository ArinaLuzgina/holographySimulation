#include "hologram_prep.h"

obj_plate::obj_plate()
    :
    scale(1e-6),
    number_of_points{100, 100},
    width(10.0),
    height(10.0),
    cos_alpha(cos(0)),
    sin_alpha(sin(0))
    {
        intensity_matrix.resize(number_of_points[0], std::vector<double>(number_of_points[1]));
    }

obj_plate::obj_plate(double scale,const std::vector<unsigned int>& number_of_points, double width, double height, double alpha)
    : 
    scale(scale),
    number_of_points(number_of_points),
    width(width),
    height(height),
    cos_alpha(cos(alpha)),
    sin_alpha(sin(alpha))
    {
        intensity_matrix.resize(number_of_points[0], std::vector<double>(number_of_points[1]));
    }
    
bool obj_plate::readDataFromFile(const std::string& name, char delimiter) {
        // Очищаем предыдущие данные
        geom_matrix.clear();
        surfaces.clear();
        vertexes.clear();
    
        // 1. Чтение вершин из _vertexes.txt
        const std::string vertex_file = name + "_vertexes.txt";
        std::ifstream vfile(vertex_file);
        if (vfile.is_open()) {
            std::string line;
            while (std::getline(vfile, line)) {
                std::replace(line.begin(), line.end(), delimiter, ' ');
                std::istringstream iss(line);
                std::vector<Point> surface_vertices;
                Point p;
                
                // Читаем 4 вершины (12 значений)
                for (size_t i = 0; i < 4; ++i) {
                    if (!(iss >> p.x >> p.y >> p.z)) {
                        std::cerr << "Error: Invalid vertex format in line: " << line << "\n";
                        return false;
                    }
                    surface_vertices.push_back(p);
                }
                vertexes.push_back(surface_vertices);
            }
            vfile.close();
        } else {
            std::cerr << "Warning: Vertex file not found " << vertex_file << "\n";
        }
    
        // 2. Чтение точек из _points.txt
        const std::string points_file = name + "_points.txt";
        std::ifstream pfile(points_file);
        if (!pfile.is_open()) {
            std::cerr << "Error: Can't open points file " << points_file << "\n";
            return false;
        }
    
        std::string line;
        while (std::getline(pfile, line)) {
            std::replace(line.begin(), line.end(), delimiter, ' ');
            std::istringstream iss(line);
            Point p;
            if (!(iss >> p.x >> p.y >> p.z)) {
                std::cerr << "Error: Invalid point format in line: " << line << "\n";
                return false;
            }
            geom_matrix.push_back(p);
        }
        pfile.close();
    
        // 3. Чтение поверхностей из _surfaces.txt
        const std::string surfaces_file = name + "_surfaces.txt";
        std::ifstream sfile(surfaces_file);
        if (!sfile.is_open()) {
            std::cerr << "Error: Can't open surfaces file " << surfaces_file << "\n";
            return false;
        }
    
        while (std::getline(sfile, line)) {
            std::replace(line.begin(), line.end(), delimiter, ' ');
            std::istringstream iss(line);
            std::vector<double> surface(4);
            if (!(iss >> surface[0] >> surface[1] >> surface[2] >> surface[3])) {
                std::cerr << "Error: Invalid surface format in line: " << line << "\n";
                return false;
            }
            surfaces.push_back(surface);
        }
        sfile.close();
    
        return true;
}   

bool obj_plate::check_point_ray(double u, double s, double a, double b, double g) {
    // эта функция проверяет, что луч который идет от точки на объекте к точке на пластинке не пересекает 
    //нигде поверхноcть объекта (то есть то, что световой луч мог попасть в эту точку)

    
    // Проверка согласованности данных
    if (surfaces.empty() || surfaces.size() != vertexes.size()) {
        std::cerr << "Error: surfaces and vertexes size mismatch\n";
        return true;
    }

    for (size_t surf = 0; surf < surfaces.size(); surf++) {
        // Проверка, что поверхность содержит 4 вершины
        if (vertexes[surf].size() != 4) {
            std::cerr << "Error: surface " << surf << " has invalid vertex count\n";
            continue;
        }

        const auto& surf_vertices = vertexes[surf];
        const auto& surf_coeffs = surfaces[surf];

        // Проверка деления на ноль
        double denominator = (a - u) * surf_coeffs[0] + (b - s) * surf_coeffs[1] + g * surf_coeffs[2];
        if (std::abs(denominator) < 1e-6 * scale) {
            continue; // Избегаем деления на ноль
        }

        double t = -1.0 * (surf_coeffs[3] + u * surf_coeffs[0] + s * surf_coeffs[1]) / denominator;

        if (t >= 0 && t < 1) {
            double xt = u + t * (a - u);
            double yt = s + t * (b - s);
            double zt = t * g;

            // Коэффициенты для плоскости
            double dx1 = surf_vertices[1].x - surf_vertices[0].x;
            double dy1 = surf_vertices[1].y - surf_vertices[0].y;
            double dx3 = surf_vertices[3].x - surf_vertices[0].x;
            double dy3 = surf_vertices[3].y - surf_vertices[0].y;

            double denominator_coef = dx1 * dy3 - dy1 * dx3;
            if (std::abs(denominator_coef) < 1e-6 * scale) {
                continue; // Избегаем деления на ноль
            }

            double coefS = ((xt - surf_vertices[0].x) * dy3 - (yt - surf_vertices[0].y) * dx3) / denominator_coef;
            double coefT = ((yt - surf_vertices[0].y) * dx1 - (xt - surf_vertices[0].x) * dy1) / denominator_coef;
            // Проверка принадлежности точки поверхности
            if (coefS >= -1e-6 && coefS <= 1 + 1e-6 && 
                coefT >= -1e-6 && coefT <= 1 + 1e-6) {
                double expected_z = surf_vertices[0].z + coefS * (surf_vertices[1].z - surf_vertices[0].z) + coefT * (surf_vertices[3].z - surf_vertices[0].z);
                if (fabs(zt - expected_z) < 1e-6 * scale)  {
                    return false; // Пересечение обнаружено
                }
            }
        }
    }
    return true; // Пересечений нет
}

double obj_plate::calculate_intensity_from_obj(double x, double y) {
    double I_res = 0.0;

    //#pragma omp parallel for reduction(+: I_res) num_threads(14)

    for(size_t ind = 0; ind < geom_matrix.size(); ind++) {
        bool intersect = check_point_ray(x, y, geom_matrix[ind].x, geom_matrix[ind].y, geom_matrix[ind].z);
        if(intersect){

            const double j = geom_matrix[ind].x; 
            const double i_val = geom_matrix[ind].y;
            double r = sqrt((j - x) * (j - x) + (i_val - y) * (i_val - y) + geom_matrix[ind].z * geom_matrix[ind].z);
            if(r < scale * 1e-9){
                r = scale * 1e-9;
            }
            // double cos_a_x = (x - j) / r;
            // double cos_a_y = (y - i_val) / r;
            // double cos_a_z = (-geom_matrix[ind].z) / r;
            //std::cout << r << std::endl;
            //std::cout << j << " " << x << " " << i_val << " " << y << std::endl;
            double rho = sqrt((j - x) * (j - x) + (i_val - y) * (i_val - y));
            double phase = k * r  - k * sin_alpha * x;//k * rho * rho / (2 * geom_matrix[ind].z);//(k * sin_alpha - k * cos_a_x) * x + (-1.0) * k * cos_a_y * y;
            //std::cout << phase << std::endl;
            I_res += I * (1 + geom_matrix[ind].z * geom_matrix[ind].z/(r*r) + 2 * cos(phase) * geom_matrix[ind].z/r);
        }
    }
    return I_res;
}

bool obj_plate::calculate_intensity_matrix() {
    double I_max = 0.0;
    const double stepx = width * scale / number_of_points[1];
    const double stepy = height * scale / number_of_points[0];

    // Первый этап: вычисление интенсивностей и поиск максимума
    #pragma omp parallel for collapse(2) reduction(max: I_max) num_threads(14)
    for (size_t yp = 0; yp < number_of_points[0]; ++yp) {
        for (size_t xp = 0; xp < number_of_points[1]; ++xp) {
            const double intensity = calculate_intensity_from_obj(xp * stepx, yp * stepy);
            intensity_matrix[yp][xp] = intensity;
            if (intensity > I_max) {
                I_max = intensity;
            }
        }
    }

    // Нормализация
    if (I_max != 0.0) {
        #pragma omp parallel for collapse(2) num_threads(14)
        for (size_t yp = 0; yp < number_of_points[0]; ++yp) {
            for (size_t xp = 0; xp < number_of_points[1]; ++xp) {
                intensity_matrix[yp][xp] /= I_max;
            }
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
    std::cout << "intensity has been saved\n";
    return true;
}

