
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <random>

// Window dimensions
const unsigned int SCR_WIDTH  = 1280;
const unsigned int SCR_HEIGHT = 720;

// Camera
float yaw       = -90.0f;
float pitch     =   0.0f;
const float rotSpeed    = 45.0f; // deg/sec
glm::vec3 cameraStartPos = {0.0f, 0.0f, 3.0f};
glm::vec3 cameraPos      = cameraStartPos;
glm::vec3 cameraFront    = {0.0f, 0.0f, -1.0f};
glm::vec3 cameraUp       = {0.0f, 1.0f, 0.0f};
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Creates and returns a matrix (vector of vectors) of the specified size,
// initialized to zeros.
std::vector<std::vector<double>> createIntensity(std::size_t rows, std::size_t cols) {
    return std::vector<std::vector<double>>(rows, std::vector<double>(cols, 0.0));
}

// Fills the given matrix with random double values in the range [0, 1).
// Uses a Mersenne Twister engine and uniform real distribution.
void fillRandom(std::vector<std::vector<double>>& intensity) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (auto& row : intensity) {
        for (auto& val : row) {
            val = dist(gen);
        }
    }
}
// Dummy intensity data;
// std::vector<std::vector<double>> intensity = [](){
//     int W=10000, H=10000;
//     std::vector<std::vector<double>> m(W, std::vector<double>(H));
//     for(int i=0;i<W;i++) for(int j=0;j<H;j++)
//         m[i][j] = (double)i/W; // градиент
//     return m;
// }();

void processInput(GLFWwindow *window) {
    // reset
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cameraPos   = cameraStartPos;
        yaw         = -90.0f;
        pitch       =   0.0f;
        cameraFront = {0.0f, 0.0f, -1.0f};
    }

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    float camSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += camSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= camSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * camSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * camSpeed;

    float angleStep = rotSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS) yaw   -= angleStep;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) yaw   += angleStep;
    if (glfwGetKey(window, GLFW_KEY_UP   ) == GLFW_PRESS) pitch += angleStep;
    if (glfwGetKey(window, GLFW_KEY_DOWN ) == GLFW_PRESS) pitch -= angleStep;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// Шейдеры
const char *vertexShaderSource = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;
out vec2 TexCoords;
uniform mat4 model, view, projection;
void main() {
    TexCoords = aTex;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)glsl";

const char *fragmentShaderSource = R"glsl(
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D intensityMap;
void main() {
    float v = texture(intensityMap, TexCoords).r;
    FragColor = vec4(v, v, v, 1.0);
}
)glsl";

int main() {
    auto intensity = createIntensity(8, 8); 
    fillRandom(intensity);

    // GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HoloViz", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    glEnable(GL_DEPTH_TEST);

    // Компиляция шейдеров…
    auto compile = [&](GLenum type, const char* src){
        GLuint s = glCreateShader(type);
        glShaderSource(s,1,&src,NULL);
        glCompileShader(s);
        return s;
    };
    GLuint vs = compile(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);
    glUseProgram(prog);
    glUniform1i(glGetUniformLocation(prog, "intensityMap"), 0);

    // Подготовка данных по текстуре
    int texW = intensity.size();
    int texH = intensity[0].size();
    std::vector<float> pixels;
    pixels.reserve(texW * texH);
    for(int y=0;y<texH;y++)
        for(int x=0;x<texW;x++)
            pixels.push_back(static_cast<float>(intensity[x][y]));

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texW, texH, 0, GL_RED, GL_FLOAT, pixels.data());

    // Квад с UV
    float quadVerts[] = {
        //  X     Y    Z     U    V
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
    };
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers       (1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer       (GL_ARRAY_BUFFER, VBO);
    glBufferData       (GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // Рендер-цикл
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(prog);
        // матрицы
        glm::mat4 model      = glm::mat4(1.0f);
        glm::mat4 view       = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                    float(SCR_WIDTH)/SCR_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(prog,"model"),      1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(prog,"view"),       1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(prog,"projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers     (1,&VBO);
    glDeleteProgram     (prog);
    glfwTerminate();
    return 0;
}

