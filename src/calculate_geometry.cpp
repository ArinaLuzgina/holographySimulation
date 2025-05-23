#include <fstream>
#include <iostream>

#include "geometry.h"

obj_geometry::obj_geometry() 
    : scale(1e-6), 
      number_of_points{100, 100}, 
      vertexes{{{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}}} 
{
    //points.resize(number_of_points[0] * number_of_points[1] * vertexes.size());
}

obj_geometry::obj_geometry(double scale, const std::vector<unsigned int>& number_of_points, const std::vector<std::vector<Point>>& vertexes) 
    :
     scale(scale), 
     number_of_points(number_of_points),
     vertexes(vertexes)
{
    // Проверка валидности параметров
    if (number_of_points.size() != 2 || number_of_points[0] == 0 || number_of_points[1] == 0) {
        throw std::invalid_argument("Invalid number_of_points");
    }
    if (vertexes.empty()) {
        throw std::invalid_argument("Vertexes vector is empty");
    }
    for (const auto& v : vertexes) {
        if (v.size() != 4) {
            throw std::invalid_argument("Each surface must have exactly 4 vertices");
        }
}

}

bool obj_geometry::save_geometry(const std::string& filename, char delimeter) const {
    // Сохраняем вершины поверхностей
    const std::string vertex_filename = filename + "_vertexes.txt";
    std::ofstream vfile(vertex_filename);
    if (!vfile.is_open()) {
        std::cerr << "Error: Could not open vertex file " << vertex_filename << "\n";
        return false;
    }

    for (const auto& surface_vertices : vertexes) {
        if (surface_vertices.size() != 4) {
            std::cerr << "Warning: Invalid vertex count in surface\n";
            continue;
        }
        
        for (size_t i = 0; i < surface_vertices.size(); ++i) {
            const Point& p = surface_vertices[i];
            vfile << p.x << delimeter 
                  << p.y << delimeter 
                  << p.z;
            if (i != surface_vertices.size() - 1) {
                vfile << delimeter; // Разделитель между вершинами
            }
        }
        vfile << "\n";
    }
    vfile.close();
    std::cout << "Vertex data written to " << vertex_filename << "\n";

    // Оригинальный код сохранения точек и поверхностей
    const std::string points_filename = filename + "_points.txt";
    std::ofstream pfile(points_filename);
    if (!pfile.is_open()) {
        std::cerr << "Error: Could not open points file " << points_filename << "\n";
        return false;
    }

    for (const Point& p : points) {
        pfile << p.x << delimeter << p.y << delimeter << p.z << "\n";
    }
    pfile.close();

    const std::string surfaces_filename = filename + "_surfaces.txt";
    std::ofstream sfile(surfaces_filename);
    if (!sfile.is_open()) {
        std::cerr << "Error: Could not open surfaces file " << surfaces_filename << "\n";
        return false;
    }

    for (const auto& surface : surfaces) {
        if (surface.size() != 4) {
            std::cerr << "Warning: Invalid surface format\n";
            continue;
        }
        sfile << surface[0] << delimeter
              << surface[1] << delimeter
              << surface[2] << delimeter
              << surface[3] << "\n";
    }
    sfile.close();

    std::cout << "All data saved successfully!\n";
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
        double res =  (p8 - p7 + (p6 - p8) * (p3 - p4) / (p2 - p4)) / (p5 - p7 + (p8 - p6) * (p1 - p3) / (p2 - p4));
        // std::cout << res << " " << eps <<"\n";
        return res;
    }
    return 0.0;
}


bool obj_geometry::calculate_points_for_one_surface(std::vector<Point> vertex){
    if (vertex.size() != 4) {
        std::cerr << "Error: Surface must have exactly 4 vertices\n";
        return false;
    }
    std::vector<Point> sides;
    double eps = 1e-6 * scale;
    for(size_t i = 0; i < 4; i ++){ // задаем векторы боковых граней
        if (i < 2){ 
        sides.push_back({(vertex[(i + 1) % 4].x - vertex[i % 4].x) * scale, (vertex[(i + 1) % 4].y - vertex[i % 4].y) * scale, (vertex[(i + 1) % 4].z - vertex[i % 4].z) * scale});
        }else{
            sides.push_back({(vertex[i % 4].x - vertex[(i + 1) % 4].x) * scale, (vertex[i % 4].y - vertex[(i + 1) % 4].y) * scale, (vertex[i % 4].z - vertex[(i + 1) % 4].z) * scale});
        }
    }
    for(size_t i = 0; i < 4; i++){ // задаем шаг на каждой из сторон
        sides[i].x = sides[i].x * 1.0 / number_of_points[i % 2];
        sides[i].y = sides[i].y * 1.0 / number_of_points[i % 2];
        sides[i].z = sides[i].z * 1.0 / number_of_points[i % 2];
    }
    for(size_t i = 0; i <= number_of_points[0]; i ++){ // теперь посчитаем все точки, который у нас есть
        for(size_t j = 0; j <= number_of_points[1]; j ++){
            if(i == 0 and j == 0){
                continue;
            }
            std::vector<Point> sup_vec;
            Point p;

            for(size_t k = 0; k < 4; k ++){
                if(k % 2 == 0){
                    sup_vec.push_back({sides[k].x * i, sides[k].y * i, sides[k].z * i});
                }else{
                    sup_vec.push_back({sides[k].x * j, sides[k].y * j, sides[k].z * j});
                }
                // if(k < 2){
                //     sup_vec[k].x +=  vertex[k].x * scale;
                //     sup_vec[k].y +=  vertex[k].y * scale;
                //     sup_vec[k].z +=  vertex[k].z * scale;
                // }else{
                //     sup_vec[k].x +=  vertex[(k + 1) % 4].x * scale;
                //     sup_vec[k].y +=  vertex[(k + 1) % 4].y * scale;
                //     sup_vec[k].z +=  vertex[(k + 1) % 4].z * scale;
                // }
                //std::cout << sup_vec[k].x << " " << sup_vec[k].y << " " << sup_vec[k].z << " " << i << " " << j << std::endl;
            }
            double v = 0.0;
            // bool flag = true;
            // if((v = calculate_param(sup_vec[0].x + vertex[0].x, sup_vec[1].x + vertex[1].x, sup_vec[2].x + vertex[2].x, sup_vec[3].x + vertex[3].x, sup_vec[0].y + vertex[0].y, sup_vec[1].y + vertex[1].y, sup_vec[2].y + vertex[2].y, sup_vec[3].y + vertex[3].y, eps)) > eps){
            //     ;//std::cout << v << std::endl;
            //     std::cout << "1\n";
            // } 
            // else if((v = calculate_param(sup_vec[0].x, sup_vec[1].x, sup_vec[2].x, sup_vec[3].x, sup_vec[0].z, sup_vec[1].z, sup_vec[2].z, sup_vec[3].z, eps)) > eps){
            //     ;//std::cout << v << std::endl;
            //     std::cout << "2\n";

            // }
            // else if((v = calculate_param(sup_vec[0].y, sup_vec[1].y, sup_vec[2].y, sup_vec[3].y, sup_vec[0].x, sup_vec[1].x, sup_vec[2].x, sup_vec[3].x, eps)) > eps){
            //     ;//std::cout << v << std::endl;
            //     std::cout << "3\n";

            // } 
            // else if((v = calculate_param(sup_vec[0].y, sup_vec[1].y, sup_vec[2].y, sup_vec[3].y, sup_vec[0].z, sup_vec[1].z, sup_vec[2].z, sup_vec[3].z, eps)) > eps){
            //     ;//std::cout << v << std::endl;
            //     std::cout << "4\n";

            // }
            // else if((v = calculate_param(sup_vec[0].z, sup_vec[1].z, sup_vec[2].z, sup_vec[3].z, sup_vec[0].x, sup_vec[1].x, sup_vec[2].x, sup_vec[3].x, eps)) > eps){
            //     ;//std::cout << v << std::endl;
            //     std::cout << "5\n";

            // } 
            // else if((v = calculate_param(sup_vec[0].z, sup_vec[1].z, sup_vec[2].z, sup_vec[3].z, sup_vec[0].y, sup_vec[1].y, sup_vec[2].y, sup_vec[3].y, eps)) > eps){
            //     ;//std::cout << v << std::endl;
            //     std::cout << "6\n";

            // } 
            // else if((v = calculate_param(sup_vec[1].x, sup_vec[0].x, sup_vec[3].x, sup_vec[2].x, sup_vec[1].y, sup_vec[0].y, sup_vec[3].y, sup_vec[2].y, eps)) > eps){
            //     flag = false;//std::cout << v << std::endl; 
 
            //     std::cout << "7\n";
            // }
            // else if((v = calculate_param(sup_vec[1].x, sup_vec[0].x, sup_vec[3].x, sup_vec[2].x, sup_vec[1].z, sup_vec[0].z, sup_vec[3].z, sup_vec[2].z, eps)) > eps){
            //     flag = false;//std::cout << v << std::endl;  

            //     std::cout << "8\n";
            // }
            // else if((v = calculate_param(sup_vec[1].y, sup_vec[0].y, sup_vec[3].y, sup_vec[2].y, sup_vec[1].x, sup_vec[0].x, sup_vec[3].x, sup_vec[2].x, eps)) > eps){
            //     flag = false;//std::cout << v << std::endl;
            //     std::cout << "9\n";
            // }
            // else if((v = calculate_param(sup_vec[1].y, sup_vec[0].y, sup_vec[3].y, sup_vec[2].y, sup_vec[1].z, sup_vec[0].z, sup_vec[3].z, sup_vec[2].z, eps)) > eps){
            //     flag = false;//std::cout << v << std::endl; 
            //     std::cout << "10\n";
            // }
            // else if((v = calculate_param(sup_vec[1].z, sup_vec[0].z, sup_vec[3].z, sup_vec[2].z, sup_vec[1].x, sup_vec[0].x, sup_vec[3].x, sup_vec[2].x, eps)) > eps){
            //     flag = false;//std::cout << v << std::endl;  
            //     std::cout << "11\n";
            // }
            // else if((v = calculate_param(sup_vec[1].z, sup_vec[0].z, sup_vec[3].z, sup_vec[2].z, sup_vec[1].y, sup_vec[0].y, sup_vec[3].y, sup_vec[2].y, eps)) > eps){
            //     flag = false;//std::cout << v << std::endl;  
            //     std::cout << "12\n";
            // }
            // std::cout << v << "!" << std::endl;
            bool flag = true;
            v = eps * 5;
            if(v > eps){
                p.x = vertex[0].x * scale + sup_vec[0].x + sup_vec[1].x;
                p.y = vertex[0].y * scale + sup_vec[0].y + sup_vec[1].y;
                p.z = vertex[0].z * scale + sup_vec[0].z + sup_vec[1].z;
            }
            points.push_back(p);
        }
    }    
    return true;   
}

bool obj_geometry::calculate_surface(std::vector<Point> vertex){
    if (vertex.size() != 4) {
        std::cerr << "Error: Surface must have exactly 4 vertices\n";
        return false;
    }
    double a = (vertex[1].x - vertex[0].x) * scale;
    double b = (vertex[1].y - vertex[0].y) * scale;
    double c = (vertex[1].z - vertex[0].z) * scale;
    double d = (vertex[3].x - vertex[0].x) * scale;
    double g = (vertex[3].y - vertex[0].y) * scale;
    double f = (vertex[3].z - vertex[0].z) * scale;
    double X = b * f - c * g;
    double Y = d * c - a * f;
    double Z = a * g - b * d;
    double SHIFT = vertex[2].x  * scale * c * g + vertex[2].y * scale * a * f + vertex[2].z  * scale * b * d;
    surfaces.push_back({X, Y, Z, SHIFT});
    return true;
}

bool obj_geometry::calculate_points(){
    for(size_t c = 0; c < vertexes.size(); c++){
        points.push_back({vertexes[c][0].x * scale, vertexes[c][0].y * scale, vertexes[c][0].z * scale});
        calculate_points_for_one_surface(vertexes[c]);
    }
    return true;
}

bool obj_geometry::calculate_surfaces(){
    for(size_t c = 0; c < vertexes.size(); c++){
        calculate_surface(vertexes[c]);
    }
    return true;
}
