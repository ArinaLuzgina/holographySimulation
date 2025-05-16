#include <iostream>
#include "hologram_prep.h"
// Вспомогательная функция для сравнения точек
bool comparePoints(const Point& p1, const Point& p2, double epsilon = 1e-6) {
    return std::abs(p1.x - p2.x) < epsilon &&
           std::abs(p1.y - p2.y) < epsilon &&
           std::abs(p1.z - p2.z) < epsilon;
}

// Тест 1: Корректные данные
bool test_valid_data(Point expected[]) {
    // Создание тестового файла
    const std::string filename = "test_valid.txt";
    {
        std::ofstream file(filename);
        file << "1.0 2.0 3.0; 4.0 5.0 6.0; 7.0 8.0 9.0;\n"
             << "10.0 11.0 12.0; 13.0 14.0 15.0; 16.0 17.0 18.0;\n";
    }

    try {
        obj_plate plate;
        plate.readDataFromFile(2, 3, filename);
        
        // Проверка структуры
        if(plate.geom_matrix.size() != 2) return false;
        if(plate.geom_matrix[0].size() != 3 || plate.geom_matrix[1].size() != 3) return false;

        // Проверка значений
        
        int idx = 0;
        for(const auto& row : plate.geom_matrix) {
            for(const auto& point : row) {
                if(!comparePoints(point, expected[idx++])) return false;
            }
        }
    }
    catch(...) {
        return false;
    }
    
    std::remove(filename.c_str());
    return true;
}

// Тест 2: Файл не найден
bool test_file_not_found(std::string file_name) {
    try {
        obj_plate plate;
        plate.readDataFromFile(1, 1, file_name);
        return false; // Не должно сюда попасть
    }
    catch(const std::runtime_error&) {
        return true;
    }
    catch(...) {
        return false;
    }
}

// Тест 3: Неправильный формат данных
bool test_invalid_format(std::string invalid_data) {
    const std::string filename = "test_invalid.txt";
    {
        std::ofstream file(filename);
        file << invalid_data;
    }

    try {
        obj_plate plate;
        plate.readDataFromFile(1, 1, filename);
        return false;
    }
    catch(const std::runtime_error&) {
        std::remove(filename.c_str());
        return true;
    }
    catch(...) {
        std::remove(filename.c_str());
        return false;
    }
}

int main() {
    int passed = 0;
    int total = 6;

    std::cout << "Running tests...\n";
    Point expected[] = {
        {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0},
        {10.0, 11.0, 12.0}, {13.0, 14.0, 15.0}, {16.0, 17.0, 18.0}
    };
    // Запуск тестов
    
    if(test_valid_data(expected)) {
        std::cout << "Test 1 passed (data read correctly)\n";
        passed++;
    } else {
        std::cout << "Test 1 failed (data have not been read correctly)\n";
    }
    expected[1].y = 8.0;
    if(test_valid_data(expected)) {
        std::cout << "Test 2 passed (data read correctly)\n";
        passed++;
    } else {
        std::cout << "Test 2 failed (data have not been read correctly)\n";
    }
    std::string file_name = "non_existent_file.txt";
    if(test_file_not_found(file_name)) {
        std::cout << "Test 3 passed (file does not exist)\n";
        passed++;
    } else {
        std::cout << "Test 3 failed (file exists)\n";
    }
    file_name = "test_valid.txt";
    if(test_file_not_found(file_name)) {
        std::cout << "Test 4 passed (file does not exist)\n";
        passed++;
    } else {
        std::cout << "Test 4 failed (file exists)\n";
    }
    std::string invalid_data = "1.0 abc 3.0;\n";
    if(test_invalid_format(invalid_data)) {
        std::cout << "Test 5 passed (data is invalid)\n";
        passed++;
    } else {
        std::cout << "Test 5 failed (data is not invalid)\n";
    }
    invalid_data = "1.0 2.0 3.0;\n";
    if(test_invalid_format(invalid_data)) {
        std::cout << "Test 6 passed (data is invalid)\n";
        passed++;
    } else {
        std::cout << "Test 6 failed (data is not invalid)\n";
    }
    // Вывод результатов
    std::cout << "\nPassed: " << passed << "/" << total << "\n";
    return passed == total ? 0 : 1;
}