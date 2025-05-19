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

# FetchContent берёт либы с гитхаба
include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# GLAD
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v0.1.36
)
FetchContent_MakeAvailable(glad)

# GLM (заголовочный)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)
FetchContent_MakeAvailable(glm)


# Пути к директориям
set(HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Исходные файлы
file(GLOB COMMON_SOURCES 
    "${SOURCE_DIR}/calculate_geometry.cpp" 
    "${SOURCE_DIR}/calculate_transp_matrix.cpp"
    "${SOURCE_DIR}/calculate_visible_field.cpp"
    "${SOURCE_DIR}/graphics.cpp"
    "${SOURCE_DIR}/input_processing.cpp"
    "${SOURCE_DIR}/rendering.cpp"
    "${SOURCE_DIR}/settings.cpp"
    "${SOURCE_DIR}/shaders.cpp"
    "${SOURCE_DIR}/textures.cpp"
    "${SOURCE_DIR}/VBO.cpp"
  

)


set(TEST1_SOURCES tests/test_geom.cpp)  # Первый тест
set(TEST2_SOURCES tests/test_hologram_prep.cpp)  # Второй тест
set(TEST3_SOURCES tests/test_visible_field.cpp)  # Третий тест
set(MAIN_SOURCES src/main.cpp)  # основной файл тест

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

# Собираем третий тест
add_executable(Test3 
    ${TEST3_SOURCES}
    ${COMMON_SOURCES}  # Если нужны общие файлы
)

# Собираем третий тест
add_executable(main 
    ${MAIN_SOURCES}
    ${COMMON_SOURCES}  # Если нужны общие файлы
)
# Пути
target_include_directories(main
  PRIVATE
    ${glad_SOURCE_DIR}/include
)

# Подключение OpenMP
if(OpenMP_FOUND)
    target_link_libraries(Test1 PRIVATE OpenMP::OpenMP_CXX)
    target_link_libraries(Test2 PRIVATE OpenMP::OpenMP_CXX)
    target_link_libraries(Test3 PRIVATE OpenMP::OpenMP_CXX)
    target_link_libraries(main PRIVATE OpenMP::OpenMP_CXX glfw glad glm::glm)
endif()

# Подключение заголовков
target_include_directories(Test1 PRIVATE ${HEADER_DIR})
target_include_directories(Test2 PRIVATE ${HEADER_DIR})
target_include_directories(Test3 PRIVATE ${HEADER_DIR})
target_include_directories(main PRIVATE ${HEADER_DIR})


# Настройки компилятора для GCC/Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Test1 PRIVATE -Wall -Wextra -Werror)
    target_compile_options(Test2 PRIVATE -Wall -Wextra -Werror)
    target_compile_options(Test3 PRIVATE -Wall -Wextra -Werror)
    target_compile_options(main PRIVATE -Wall -Wextra -Werror)
endif()

# Для Windows
if(WIN32)
    target_compile_definitions(Test1 PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(Test2 PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(Test3 PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(main PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()