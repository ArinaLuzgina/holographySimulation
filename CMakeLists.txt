cmake_minimum_required(VERSION 3.19)

set(PROJECT_NAME holography)
project(${PROJECT_NAME} LANGUAGES C CXX)

# Поиск поддержки OpenMP
find_package(OpenMP REQUIRED)

# Проверка, найден ли OpenMP
if(OpenMP_FOUND)
    message(STATUS "OpenMP found: ${OpenMP_CXX_VERSION}")
    # Добавляем флаги OpenMP для компилятора
    add_compile_options(${OpenMP_CXX_FLAGS})
else()
    message(FATAL_ERROR "OpenMP not found!")  # Прерываем сборку, если OpenMP отсутствует
endif()

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") 

# Пути к директориям
set(HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Исходные файлы
file(GLOB COMMON_SOURCES 
    "${SOURCE_DIR}/calculate_geometry.cpp" 
    "${SOURCE_DIR}/calculate_transp_matrix.cpp"
)


set(TEST1_SOURCES tests/test_geom.cpp)  # Первый тест
set(TEST2_SOURCES tests/test_hologram_prep.cpp)  # Второй тест

# Собираем первый тест
add_executable(Test1 
    ${TEST1_SOURCES}
    ${COMMON_SOURCES}  # Если нужны общие файлы
)

# Собираем второй тест
add_executable(Test2 
    ${TEST2_SOURCES}
    ${COMMON_SOURCES}  # Если нужны общие файлы
)

# Подключение OpenMP
if(OpenMP_FOUND)
    target_link_libraries(Test1 PRIVATE OpenMP::OpenMP_CXX)
    target_link_libraries(Test2 PRIVATE OpenMP::OpenMP_CXX)
endif()

# Подключение заголовков
target_include_directories(Test1 PRIVATE ${HEADER_DIR})
target_include_directories(Test2 PRIVATE ${HEADER_DIR})


# Настройки компилятора для GCC/Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Test1 PRIVATE -Wall -Wextra -Werror)
    target_compile_options(Test2 PRIVATE -Wall -Wextra -Werror)
endif()

# Для Windows
if(WIN32)
    target_compile_definitions(Test1 PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(Test2 PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()