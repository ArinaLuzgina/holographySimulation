// test_geometry.cpp
#include "geometry.h"
#include <cassert>
#include <sstream>

void test_constructors() {
    std::cout << "=== Testing constructors ===\n";
    
    // Тест конструктора по умолчанию
    obj_geometry geom1;
    assert(geom1.scale == 1e-6);
    assert(geom1.number_of_points[0] == 100);
    assert(geom1.vertexes.size() == 1);
    std::cout << "Default constructor: OK\n";

    // Тест параметризованного конструктора
    std::vector<std::vector<Point>> vertices = {{{0,0,0}, {1,0,0}, {0,1,0}, {1,1,0}}};
    obj_geometry geom2(2e-6, {50, 30}, vertices);
    assert(geom2.scale == 2e-6);
    assert(geom2.number_of_points[1] == 30);
    assert(geom2.vertexes.size() == 1);
    std::cout << "Parameterized constructor: OK\n";
}

void test_save_geometry() {
    std::cout << "\n=== Testing save_geometry ===\n";
    
    obj_geometry geom(1, {2, 2}, {{{0.0, 0.0, 0.0}, {1.0, 0.0, 1.0}, {1.0, 1.0, 1.0}, {0.0, 1.0, 0.0}}, {{-1,0,0}, {0,0,0}, {0,1,0}, {-1,1,0}}});
    geom.calculate_points();
    geom.calculate_surfaces();

    // Тест сохранения
    assert(geom.save_geometry("test_output"));
    
    // Проверка существования файлов
    std::ifstream points_file("test_output_points.txt");
    std::ifstream surfaces_file("test_output_surfaces.txt");
    assert(points_file.good());
    assert(surfaces_file.good());
    
    // Проверка формата точек
    std::string line;
    std::getline(points_file, line);
    assert(!line.empty() && line.find(';') != std::string::npos);
    
    // Проверка формата поверхностей
    std::getline(surfaces_file, line);
    assert(line.find(';') != std::string::npos);
    
    std::cout << "File saving: OK\n";
}

void test_calculation_methods() {
    std::cout << "\n=== Testing calculation methods ===\n";
    
    obj_geometry geom;
    size_t initial_points = geom.points.size();
    
    // Тест расчета одной поверхности
    std::vector<Point> vertices = {{0,0,0}, {1,0,0}, {0,1,0}, {1,1,0}};
    assert(geom.calculate_points_for_one_surface(vertices));
    assert(geom.points.size() > initial_points);
    std::cout << "calculate_points_for_one_surface: OK\n";

    // Тест расчета всех точек
    initial_points = geom.points.size();
    assert(geom.calculate_points());
    assert(geom.points.size() > initial_points);
    std::cout << "calculate_points: OK\n";

    // Тест расчета поверхностей
    size_t initial_surfaces = geom.surfaces.size();
    assert(geom.calculate_surfaces());
    assert(geom.surfaces.size() > initial_surfaces);
    std::cout << "calculate_surfaces: OK\n";
}

void test_edge_cases() {
    std::cout << "\n=== Testing edge cases ===\n";
    
    // Тест с нулевыми точками
    try {
        obj_geometry geom(1e-6, {0, 0}, {});
        std::cerr << "Error: Expected exception, but object was created\n";
        assert(false);
    } catch (const std::invalid_argument& e) {
        std::cout << "Zero points handling: OK\n";
    } catch (...) {
        std::cerr << "Unexpected exception type\n";
        assert(false);
    }

    // Тест с неверными поверхностями
    try {
        obj_geometry geom;
        std::vector<Point> invalid_vertices = {{0,0,0}}; // Недостаточно точек
        bool result = geom.calculate_points_for_one_surface(invalid_vertices);
        assert(!result); // Ожидаем false при ошибке
        std::cout << "Invalid surface handling: OK\n";
    } catch (...) {
        std::cerr << "Unexpected exception\n";
        assert(false);
    }
}

int main() {
    test_constructors();
    test_calculation_methods();
    test_save_geometry();
    test_edge_cases();
    
    std::cout << "\nAll tests passed successfully!\n";
    return 0;
}