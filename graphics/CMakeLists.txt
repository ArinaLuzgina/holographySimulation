cmake_minimum_required(VERSION 3.5)
project(HologramVisualizer)

set(CMAKE_CXX_STANDARD 17)

# Подключим FetchContent
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

# Собираем ваши исходники
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Пути
target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${glad_SOURCE_DIR}/include
)

# Линкуем
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glfw
    glad
    glm::glm
)

# Дополнительно, если хотите IDE‑folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

