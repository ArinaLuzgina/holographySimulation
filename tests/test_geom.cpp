#include <iostream>
#include "geometry.h" // Подключаем заголовочный файл класса

int main() {
    // Тест 1: Создание объекта с параметрами по умолчанию
    std::cout << "=== Test 1: Default Constructor ===\n";
    obj_geometry default_obj;
    if (default_obj.calculate_points()) {
        std::cout << "Points calculated successfully!\n";
        default_obj.save_geometry("default_geometry.txt");
    } else {
        std::cerr << "Error calculating points!\n";
    }

    // Тест 2: Создание объекта с кастомными параметрами
    std::cout << "\n=== Test 2: Custom Constructor ===\n";
    std::vector<unsigned int> custom_points = {5, 5}; // 5x5 точек
    std::vector<Point> custom_vertices = {
        {0.0, 0.0, 0.0},
        {-2.0, 0.0, 2.0},
        {-2.0, 2.0, 2.0},
        {0.0, 2.0, 0.0}
    };
    double custom_scale = 1e-5;

    obj_geometry custom_obj(custom_scale, custom_points, custom_vertices);
    if (custom_obj.calculate_points()) {
        std::cout << "Points calculated successfully!\n";
        custom_obj.save_geometry("custom_geometry.txt");
    } else {
        std::cerr << "Error calculating points!\n";
    }

    // Тест 3: Проверка обработки ошибок (неверный путь)
    std::cout << "\n=== Test 3: Error Handling ===\n";
    if (!custom_obj.save_geometry("/invalid_path/test.txt")) {
        std::cerr << "(Expected) Failed to write to invalid path!\n";
    }

    return 0;
}