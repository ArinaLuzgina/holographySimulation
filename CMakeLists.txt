cmake_minimum_required(VERSION 3.19)

project(holography LANGUAGES C CXX)

# Настройка стандарта C++20 и оптимизации
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-O3)


# Написать многабукав для clangd, чтобы не ругался сильно
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Поиск и подключение OpenMP (современный способ)
find_package(OpenMP REQUIRED)

# Настройка FetchContent
include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

# GLAD (требует дополнительной настройки)
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# GLM (заголовочная библиотека)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

# Настройка путей
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Сборка общих исходников (лучше явно перечислить файлы)
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
add_executable(main src/main.cpp ${COMMON_SOURCES})
add_executable(Test1 tests/test_geom.cpp ${COMMON_SOURCES})
add_executable(Test2 tests/test_hologram_prep.cpp ${COMMON_SOURCES})
add_executable(Test3 tests/test_visible_field.cpp ${COMMON_SOURCES})

# Общие настройки для всех целей
foreach(target main Test1 Test2 Test3)
    # Подключение заголовков
    target_include_directories(${target} PRIVATE
        ${HEADER_DIR}
        ${glad_SOURCE_DIR}/include
        ${glm_SOURCE_DIR}
    )
    
    # Подключение библиотек
    target_link_libraries(${target} PRIVATE
        OpenMP::OpenMP_CXX
        glfw
        glad
        glm::glm
    )
    
    # Настройки компилятора
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra)
    endif()
    
    # Для Windows
    if(WIN32)
        target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
endforeach()

# Специфичные настройки для GLAD (если требуется)
if(TARGET glad)
    target_compile_definitions(glad PRIVATE GLAD_GL_IMPLEMENTATION)
endif()
