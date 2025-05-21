cmake_minimum_required(VERSION 3.19)
project(holography LANGUAGES C CXX)

# Настройка стандарта C++20 и оптимизации
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-O3)

# Для clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Поиск зависимостей
find_package(OpenMP REQUIRED)
# find_package(PkgConfig REQUIRED)  # Добавлено
# pkg_check_modules(FFTW REQUIRED fftw3)  # Используем pkg-config
# Ручное указание путей (для подстраховки)
# Ручное указание путей к FFTW (для Ubuntu)
set(FFTW_INCLUDE_DIRS "/usr/include")
set(FFTW_LIBRARIES "/usr/lib/x86_64-linux-gnu/libfftw3.so")
find_package(Eigen3 REQUIRED)

# Настройка FetchContent
include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

# Пути
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Общие исходники
set(COMMON_SOURCES
    ${SOURCE_DIR}/calculate_geometry.cpp
    ${SOURCE_DIR}/calculate_transp_matrix.cpp
    ${SOURCE_DIR}/calculate_visible_field.cpp
    ${SOURCE_DIR}/graphics.cpp
    ${SOURCE_DIR}/input_processing.cpp
    ${SOURCE_DIR}/rendering.cpp
    ${SOURCE_DIR}/settings.cpp
    ${SOURCE_DIR}/shaders.cpp
    ${SOURCE_DIR}/textures.cpp
    ${SOURCE_DIR}/VBO.cpp
)

# Исполняемые файлы
add_executable(create_geometry src/make_geom.cpp ${COMMON_SOURCES})
add_executable(create_transp_matrix src/make_transp_matrix.cpp ${COMMON_SOURCES})
add_executable(main src/main.cpp ${COMMON_SOURCES})
add_executable(Test1 tests/test_geom.cpp ${COMMON_SOURCES})
add_executable(Test2 tests/test_hologram_prep.cpp ${COMMON_SOURCES})
#add_executable(Test3 tests/test_visible_field.cpp ${COMMON_SOURCES})

# Настройки для всех целей
foreach(target create_geometry create_transp_matrix main Test1 Test2 )#Test3)
    target_include_directories(${target} PRIVATE
        ${HEADER_DIR}
        ${FFTW_INCLUDE_DIRS}
        ${glad_SOURCE_DIR}/include
        ${glm_SOURCE_DIR}
        ${Eigen3_INCLUDE_DIRS}
    )
    
    target_link_libraries(${target} PRIVATE
        ${FFTW_LIBRARIES}
        OpenMP::OpenMP_CXX
        glfw
        glad
        glm::glm
        Eigen3::Eigen
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE  -ffast-math)
    endif()
    
    if(WIN32)
        target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
endforeach()

# Дополнительные настройки для GLAD
if(TARGET glad)
    target_compile_definitions(glad PRIVATE GLAD_GL_IMPLEMENTATION)
endif()