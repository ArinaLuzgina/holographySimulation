// test_intensity_manual.cpp
#include "hologram_prep.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool test_calculate_intensity() {
    std::cout << "Running test_calculate_intensity...\n";
    
    // 1. Подготовка тестовых данных
    obj_plate plate(1e-6, {2, 2});
    
    // Имитация geom_matrix с простыми данными
    plate.geom_matrix = {
        { {0.0, 0.0, 1.0}, {1.0, 0.0, 1.0} },
        { {0.0, 1.0, 1.0}, {1.0, 1.0, 1.0} }
    };

    // 2. Вызов тестируемой функции
    if (!plate.calculate_intensity_matrix()) {
        std::cerr << "Error: calculate_intensity_matrix failed\n";
        return false;
    }

    // 3. Проверки
    bool success = true;
    
    // Проверка размеров матрицы
    if (plate.intensity_matrix.size() != 2 || 
        plate.intensity_matrix[0].size() != 2) {
        std::cerr << "Error: Wrong matrix size\n";
        success = false;
    }

    // Проверка нормализации (максимум должен быть 1.0)
    double max_val = 0.0;
    for (const auto& row : plate.intensity_matrix) {
        for (double val : row) {
            if (val > max_val) max_val = val;
            if (val < 0 || val > 1) {
                std::cerr << "Error: Invalid value " << val << "\n";
                success = false;
            }
        }
    }
    
    if (std::abs(max_val - 1.0) > 1e-6) {
        std::cerr << "Error: Normalization failed. Max value: " << max_val << "\n";
        success = false;
    }

    return success;
}

bool test_save_to_file() {
    std::cout << "Running test_save_to_file...\n";
    
    // 1. Подготовка данных
    obj_plate plate(1e-6, {2, 2});
    plate.intensity_matrix = {
        {0.5, 1.0},
        {0.7, 0.3}
    };

    // 2. Тестирование записи
    const std::string test_file = "test_output.csv";
    
    // 2a. Корректная запись
    if (!plate.saveIntensityToFile(test_file, ';')) {
        std::cerr << "Error: Failed to save file\n";
        return false;
    }

    // 2b. Проверка содержимого файла
    std::ifstream file(test_file);
    if (!file.is_open()) {
        std::cerr << "Error: Can't open saved file\n";
        return false;
    }

    bool success = true;
    std::string line;
    for (const auto& row : plate.intensity_matrix) {
        if (!std::getline(file, line)) {
            std::cerr << "Error: File too short\n";
            success = false;
            break;
        }

        std::istringstream ss(line);
        std::string value;
        size_t count = 0;
        
        while (std::getline(ss, value, ';')) {
            count++;
        }
        
        if (count != row.size()) {
            std::cerr << "Error: Wrong column count in line\n";
            success = false;
        }
    }

    // 2c. Проверка ошибки записи
    if (plate.saveIntensityToFile("/invalid/path/test.csv")) {
        std::cerr << "Error: Should fail on invalid path\n";
        success = false;
    }

    return success;
}

int main() {
    int passed = 0;
    int total = 0;

    auto run_test = [&](const std::string& name, auto test) {
        std::cout << "=== " << name << " ===\n";
        total++;
        if (test()) {
            std::cout << "[PASSED]\n\n";
            passed++;
        } else {
            std::cout << "[FAILED]\n\n";
        }
    };

    run_test("Intensity Calculation", test_calculate_intensity);
    run_test("File Saving", test_save_to_file);

    std::cout << "Results: " << passed << "/" << total << " tests passed\n";
    return (passed == total) ? 0 : 1;
}