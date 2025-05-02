#ifndef geometry
#define geometry

#include <vector> 
#include <string>
#include <fstream>
#include <iostream>

// Объявление структуры Point
struct Point {
    double x;
    double y;
    double z;
};

// Объявление класса obj_geometry
class obj_geometry {
private:
    double scale;
    std::vector<unsigned int> number_of_points;
    std::vector<Point> vertexes;
    std::vector<std::vector<Point>> points;

public:
    // Конструкторы
    obj_geometry();
    obj_geometry(double scale, 
                 const std::vector<unsigned int>& number_of_points, 
                 const std::vector<Point>& vertexes);

    // Деструктор
    ~obj_geometry() = default;

    // Методы класса
    bool save_geometry(const std::string& filename) const;
    bool calculate_points();
};

#endif