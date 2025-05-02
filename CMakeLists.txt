cmake_minimum_required(VERSION 3.19)

set(PROJECT_NAME holography)
project(${PROJECT_NAME} LANGUAGES C CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") 

include_directories(include)

# Список исходных файлов
add_executable(${PROJECT_NAME}
    src/calculate_transp_matrix.cpp
    src/main.cpp
    src/calculate_geometry.cpp
)
target_include_directories(holography
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# # Создание исполняемого файла
# add_executable(holography ${SOURCES})

