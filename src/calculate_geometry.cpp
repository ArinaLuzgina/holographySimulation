#include "geometry.h"
obj_geometry::obj_geometry() 
    : scale(1e-6), 
      number_of_points{100, 100}, 
      vertexes{{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}} 
{
    points.resize(number_of_points[0], std::vector<Point>(number_of_points[1]));
}

obj_geometry::obj_geometry(double scale, const std::vector<unsigned int>& number_of_points, const std::vector<Point>& vertexes) : scale(scale), number_of_points(number_of_points), vertexes(vertexes)
{
        points.resize(number_of_points[0], std::vector<Point>(number_of_points[1]));
}

bool obj_geometry::save_geometry(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << "\n";
            return false;
        }
    
        // Записываем заголовок с размерами матрицы
        file << "Point matrix (" << points.size() << "x";
        if (!points.empty() && !points[0].empty()) {
            file << points[0].size();
        } else {
            file << "0";
        }
        file << "):\n";
    
        // Записываем данные
        for (const auto& row : points) {
            for (size_t i = 0; i < row.size(); ++i) {
                const Point& p = row[i];
                file << p.x << " " << p.y << " " << p.z; // XYZ через пробел
                if (i != row.size() - 1) {
                    file << " ;"; // Разделитель между столбцами
                }
            }
            file << "\n"; // Новая строка матрицы
        }
    
        file.close();
        std::cout << "Matrix successfully written to " << filename << "\n";
        return true;
    }


bool check_enum(double p1, double p2, double p3, double p4, double p5, double p6, double p7, double p8, double eps){
    if (std::abs(p2 - p4) > eps){
        return std::abs(p5 - p7 + (p8 - p6) * (p1 - p3) / (p2 - p4)) > eps;
    }
    return false;
}

double calculate_param(double p1, double p2, double p3, double p4, double p5, double p6, double p7, double p8, double eps){
    if (check_enum(p1, p2, p3, p4, p5, p6, p7, p8, eps)){
        return (p8 - p7 + (p6 - p8) * (p3 - p4) / (p2 - p4)) / (p5 - p7 + (p8 - p6) * (p1 - p3) / (p2 - p4));
    }
    return 0.0;
}


bool obj_geometry::calculate_points(){
    std::vector<Point> sides;
    double eps = 1e-9 * scale;
    for(size_t i = 0; i < 4; i ++){ // задаем векторы боковых граней
        if (i < 2){ 
        sides.push_back({(vertexes[(i + 1) % 4].x - vertexes[i % 4].x) * scale, (vertexes[(i + 1) % 4].y - vertexes[i % 4].y) * scale, (vertexes[(i + 1) % 4].z - vertexes[i % 4].z) * scale});
        }else{
            sides.push_back({(vertexes[i % 4].x - vertexes[(i + 1) % 4].x) * scale, (vertexes[i % 4].y - vertexes[(i + 1) % 4].y) * scale, (vertexes[i % 4].z - vertexes[(i + 1) % 4].z) * scale});
        }
    }
    for(size_t i = 0; i < 4; i++){ // задаем шаг на каждой из сторон
        sides[i].x = sides[i].x * 1.0 / number_of_points[i % 2];
        sides[i].y = sides[i].y * 1.0 / number_of_points[i % 2];
        sides[i].z = sides[i].z * 1.0 / number_of_points[i % 2];
    }
    for(size_t i = 0; i < number_of_points[0]; i ++){ // теперь посчитаем все точки, который у нас есть
        for(size_t j = 0; j < number_of_points[1]; j ++){
            std::vector<Point> sup_vec;

            for(size_t k = 0; k < 4; k ++){
                if(k % 2 == 0){
                sup_vec.push_back({sides[k].x * i, sides[k].y * i, sides[k].z * i});
                }else{
                    sup_vec.push_back({sides[k].x * j, sides[k].y * i, sides[k].z * i});
                }
            }
            double v = 0.0;
            bool flag = true;
            if((v = calculate_param(sup_vec[0].x, sup_vec[1].x, sup_vec[2].x, sup_vec[3].x, sup_vec[0].y, sup_vec[1].y, sup_vec[2].y, sup_vec[3].y, eps)) > eps){
                ;//std::cout << v << std::endl;
            } 
            else if((v = calculate_param(sup_vec[0].x, sup_vec[1].x, sup_vec[2].x, sup_vec[3].x, sup_vec[0].z, sup_vec[1].z, sup_vec[2].z, sup_vec[3].z, eps)) > eps){
                ;//std::cout << v << std::endl;
            }
            else if((v = calculate_param(sup_vec[0].y, sup_vec[1].y, sup_vec[2].y, sup_vec[3].y, sup_vec[0].x, sup_vec[1].x, sup_vec[2].x, sup_vec[3].x, eps)) > eps){
                ;//std::cout << v << std::endl;
            } 
            else if((v = calculate_param(sup_vec[0].y, sup_vec[1].y, sup_vec[2].y, sup_vec[3].y, sup_vec[0].z, sup_vec[1].z, sup_vec[2].z, sup_vec[3].z, eps)) > eps){
                ;//std::cout << v << std::endl;
            }
            else if((v = calculate_param(sup_vec[0].z, sup_vec[1].z, sup_vec[2].z, sup_vec[3].z, sup_vec[0].x, sup_vec[1].x, sup_vec[2].x, sup_vec[3].x, eps)) > eps){
                ;//std::cout << v << std::endl;
            } 
            else if((v = calculate_param(sup_vec[0].z, sup_vec[1].z, sup_vec[2].z, sup_vec[3].z, sup_vec[0].y, sup_vec[1].y, sup_vec[2].y, sup_vec[3].y, eps)) > eps){
                ;//std::cout << v << std::endl;
            } 
            else if((v = calculate_param(sup_vec[1].x, sup_vec[0].x, sup_vec[3].x, sup_vec[2].x, sup_vec[1].y, sup_vec[0].y, sup_vec[3].y, sup_vec[2].y, eps)) > eps){
                flag = false;//std::cout << v << std::endl; 
            }
            else if((v = calculate_param(sup_vec[1].x, sup_vec[0].x, sup_vec[3].x, sup_vec[2].x, sup_vec[1].z, sup_vec[0].z, sup_vec[3].z, sup_vec[2].z, eps)) > eps){
                flag = false;//std::cout << v << std::endl;  
            }
            else if((v = calculate_param(sup_vec[1].y, sup_vec[0].y, sup_vec[3].y, sup_vec[2].y, sup_vec[1].x, sup_vec[0].x, sup_vec[3].x, sup_vec[2].x, eps)) > eps){
                flag = false;//std::cout << v << std::endl;
            }
            else if((v = calculate_param(sup_vec[1].y, sup_vec[0].y, sup_vec[3].y, sup_vec[2].y, sup_vec[1].z, sup_vec[0].z, sup_vec[3].z, sup_vec[2].z, eps)) > eps){
                flag = false;//std::cout << v << std::endl; 
            }
            else if((v = calculate_param(sup_vec[1].z, sup_vec[0].z, sup_vec[3].z, sup_vec[2].z, sup_vec[1].x, sup_vec[0].x, sup_vec[3].x, sup_vec[2].x, eps)) > eps){
                flag = false;//std::cout << v << std::endl;  
            }
            else if((v = calculate_param(sup_vec[1].z, sup_vec[0].z, sup_vec[3].z, sup_vec[2].z, sup_vec[1].y, sup_vec[0].y, sup_vec[3].y, sup_vec[2].y, eps)) > eps){
                flag = false;//std::cout << v << std::endl;  
            }
            if(v > eps){
                if(flag){
                    points[i][j].x = sup_vec[2].x + (sup_vec[0].x - sup_vec[2].x) * v + vertexes[0].x;
                    points[i][j].y = sup_vec[2].y + (sup_vec[0].y - sup_vec[2].y) * v + vertexes[0].y;
                    points[i][j].z = sup_vec[2].z + (sup_vec[0].z - sup_vec[2].z) * v + vertexes[0].z;
                }
                else{
                    points[i][j].x = sup_vec[3].x + (sup_vec[1].x - sup_vec[3].x) * v + vertexes[0].x;
                    points[i][j].y = sup_vec[3].y + (sup_vec[1].y - sup_vec[3].y) * v + vertexes[0].y;
                    points[i][j].z = sup_vec[3].z + (sup_vec[1].z - sup_vec[3].z) * v + vertexes[0].z;
                }
            }else{
                points[i][j].x = vertexes[0].x + sup_vec[0].x + sup_vec[1].x;
                points[i][j].y = vertexes[0].y + sup_vec[0].y + sup_vec[1].y;
                points[i][j].z = vertexes[0].z + sup_vec[0].z + sup_vec[1].z;
            }
            
        }
    }    
    return true;   
}

