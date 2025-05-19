#include "visible_field.h"
#include <cassert>

void test_constructors() {
    std::cout << "=== Testing constructors ===\n";
    
    // Тест конструктора по умолчанию
    obj_visible_plate plate1;
    assert(plate1.scale == 1e-6);
    std::cout << "Default constructor: OK\n";

    // Тест параметризованного конструктора
    obj_visible_plate plate2(2e-5);
    assert(plate2.scale == 2e-5);
    std::cout << "Parameterized constructor: OK\n";
}

void test_read_intensity() {
    std::cout << "\n=== Testing read_intensity_matrix ===\n";
    
    // Создаем тестовый файл
    std::ofstream("test_intensity.csv") << "1.0;2.0;3.0\n4.0;5.0;6.0\n";
    
    obj_visible_plate plate;
    bool result = plate.read_intensity_matrix("test_intensity.csv");
    
    // Проверки
    assert(result);
    assert(plate.transp_matrix.size() == 2);
    assert(plate.visible_matrix.size() == 2);
    assert(plate.transp_matrix[0][2] == 3.0);
    std::cout << "Data reading: OK\n";

    // Тест обработки ошибок
    try {
        plate.read_intensity_matrix("invalid_file.csv");
        assert(false); // Не должно дойти до этой строки
    } catch(const std::exception& e) {
        std::cout << "Error handling: OK\n";
    }
}

void test_update_matrix() {
    std::cout << "\n=== Testing update_visible_matrix ===\n";
    
    obj_visible_plate plate;
    plate.scale = 1.0;
    plate.k = 2*M_PI/0.5; // λ = 0.5
    
    // Создаем тестовые данные
    plate.transp_matrix = {{1.0, 0.5}, {0.5, 1.0}};
    plate.visible_matrix = plate.transp_matrix;

    // Обновляем матрицу для точки (0,0,1)
    assert(plate.update_visible_matrix(0, 0, 1));
    
    // Проверяем расчеты
    const double expected = cos(2*M_PI/0.5 * 1.0); // cos(k*r)
    assert(fabs(plate.visible_matrix[0][0] - expected) < 1e-9);
    std::cout << "Calculation: OK\n";

    // Тест с некорректными данными
    obj_visible_plate invalid_plate;
    assert(!invalid_plate.update_visible_matrix(0,0,0));
    std::cout << "Invalid data handling: OK\n";
}

void test_edge_cases() {
    std::cout << "\n=== Testing edge cases ===\n";
    
    // Тест с нулевым scale
    obj_visible_plate plate;
    plate.scale = 0.0;
    plate.transp_matrix = {{1.0}};
    plate.visible_matrix = {{1.0}};
    assert(plate.update_visible_matrix(0,0,1));
    std::cout << "Zero scale handling: OK\n";
}

int main() {
    test_constructors();
    test_read_intensity();
    test_update_matrix();
    test_edge_cases();
    
    std::cout << "\nAll tests passed!\n";
    return 0;
}