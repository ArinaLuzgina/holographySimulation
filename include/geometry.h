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
public:
    double scale;
    std::vector<unsigned int> number_of_points; // количество точек, на который делится сторона многогранника.
    std::vector<std::vector<Point>> vertexes; // один элемент это набор из 4-х точек, задающих поверхность
    std::vector<Point> points; // один элекмент одна точка
    std::vector<std::vector<double>> surfaces; // один элемент массива обозначает три коэффициента, задающих плоскость

// public:
    // Конструкторы
    obj_geometry();
    obj_geometry(double scale, 
                 const std::vector<unsigned int>& number_of_points, 
                 const std::vector<std::vector<Point>>& vertexes);

    // Деструктор
    ~obj_geometry() = default;

    // Методы класса
    bool save_geometry(const std::string& filename, char delimeter = ';') const;
    bool calculate_points_for_one_surface(std::vector<Point> vertex);
    bool calculate_points();
    bool calculate_surface(std::vector<Point> vertex);
    bool calculate_surfaces();
};

#endif