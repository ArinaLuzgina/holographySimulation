// test_obj_plate.cpp
#include "hologram_prep.h"
#include <cassert>

void test_constructors() {
    std::cout << "=== Testing constructors ===\n";
    
    // Конструктор по умолчанию
    obj_plate plate1;
    assert(plate1.scale == 1e-6);
    assert(plate1.number_of_points[0] == 100);
    std::cout << "Default constructor: OK\n";

    // Параметризованный конструктор
    obj_plate plate2(2e-6, {50, 50});
    assert(plate2.scale == 2e-6);
    assert(plate2.number_of_points[1] == 50);
    std::cout << "Parameterized constructor: OK\n";
}

void test_read_data() {
    std::cout << "\n=== Testing readDataFromFile ===\n";
    
    // Создаем тестовые файлы
    std::ofstream("test_vertexes.txt") << "0;0;0;1;0;0;0;1;0;1;1;0\n";
    std::ofstream("test_points.txt") << "1.0;2.0;3.0\n4.0;5.0;6.0\n";
    std::ofstream("test_surfaces.txt") << "0.1;0.2;0.3;0.4\n0.5;0.6;0.7;0.8\n";

    obj_plate plate;
    bool result = plate.readDataFromFile("test");
    
    // Проверяем результат
    assert(result);
    assert(plate.vertexes.size() == 1);
    assert(plate.geom_matrix.size() == 2);
    assert(plate.surfaces.size() == 2);
    assert(plate.vertexes[0][0].x == 0.0);
    std::cout << "Data reading: OK\n";
}

void test_check_point() {
    std::cout << "\n=== Testing check_point_raw ===\n";
    
    obj_plate plate;
    plate.vertexes = {{{0,0,0.5}, {1,0,0.5}, {0,1,0.5}, {1,1,0.5}}};
    plate.surfaces = {{0, 0, 1, -0.5}}; // Уравнение плоскости: z = 0

    // Точка (0.5, 0.5, 2.0) → луч направлен вверх, не пересекает плоскость z=0
    assert(plate.check_point_raw(0.5, 0.5, 0.5, 0.5, 2.0) == false);

    // Точка (0.5, 0.5, 0.5) → луч пересекает плоскость z=0
    assert(plate.check_point_raw(0.5, 0.5, 0.5, 0.5, -0.5) == true);
        
    std::cout << "Intersection checks: OK\n";
}

void test_intensity_calculation() {
    std::cout << "\n=== Testing intensity calculations ===\n";
    
    obj_plate plate;

    // Тест 1: Нет данных → интенсивность должна быть нулевой
    plate.geom_matrix.clear();
    assert(plate.calculate_intensity_from_obj(0.5, 0.5) == 0.0);
    std::cout << "Test 1 (Empty data): OK\n";

    // Тест 2: Одна точка на объекте без пересечений
    plate.geom_matrix = {{0.0, 0.0, 0.5}};
    plate.vertexes = {{{0, 0, 0.5}, {1, 0, 0.5}, {0, 1, 0.5}, {1, 1, 0.5}}};
    plate.surfaces = {{0, 0, 1, -0.5}}; // Плоскость z=0

    double intensity = plate.calculate_intensity_from_obj(0.5, 0.5);
    assert(intensity > 0.0); // Луч не пересекает плоскость
    std::cout << "Test 2 (No intersection): OK\n";

    // Тест 3: Точка пересекает поверхность → интенсивность должна быть нулевой
    plate.geom_matrix = {{0.5, 0.5, 1}};
    intensity = plate.calculate_intensity_from_obj(0.5, 0.5);
    assert(intensity == 0.0);
    std::cout << "Test 3 (Intersection): OK\n";

    // Тест 4: Расчет матрицы интенсивности
    plate.number_of_points = {2, 2};
    plate.scale = 1e-6;
    plate.geom_matrix = {{0.0, 0.0, 0.5}, {1.0, 1.0, 0.5}};
    assert(plate.calculate_intensity_matrix());
    assert(plate.intensity_matrix[0][0] >= 0.0 && plate.intensity_matrix[0][0] <= 1.0);
    std::cout << "Test 4 (Matrix calculation): OK\n";
}

void test_save_intensity() {
    std::cout << "\n=== Testing saveIntensityToFile ===\n";
    
    obj_plate plate;
    plate.intensity_matrix = {{0.5, 1.0}, {0.7, 0.3}};
    
    assert(plate.saveIntensityToFile("test_intensity.csv"));
    
    // Проверка существования файла
    std::ifstream file("test_intensity.csv");
    assert(file.good());
    std::cout << "File saving: OK\n";
}

void test_edge_cases() {
    std::cout << "\n=== Testing edge cases ===\n";
    
    // Пустые данные
    obj_plate plate;
    plate.geom_matrix.clear();
    assert(plate.calculate_intensity_from_obj(0, 0) == 0.0);
    std::cout << "Empty data handling: OK\n";
}

int main() {
    test_constructors();
    test_read_data();
    test_check_point();
    test_intensity_calculation();
    test_save_intensity();
    test_edge_cases();
    
    std::cout << "\nAll tests passed!\n";
    return 0;
}

