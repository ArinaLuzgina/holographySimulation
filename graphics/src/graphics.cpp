
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <random>
#include <array>

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



// compile a single shader, print errors on failure
static GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // error-check
    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << (type==GL_VERTEX_SHADER ? "VERT" : "FRAG")
                  << " shader compile error:\n" << log << "\n";
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

// build, link, delete, use, set the "intensityMap" uniform to unit 0
GLuint createShaderProgram(const char* vertSrc, const char* fragSrc) {
    GLuint vs = compileShader(GL_VERTEX_SHADER,   vertSrc);
    if (!vs) return 0;
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!fs) { glDeleteShader(vs); return 0; }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    // link-check
    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        std::cerr << "PROGRAM link error:\n" << log << "\n";
        glDeleteProgram(prog);
        prog = 0;
    }

    // cleanup shaders no longer needed
    glDeleteShader(vs);
    glDeleteShader(fs);

    if (prog) {
        glUseProgram(prog);
        // set the sampler uniform to texture unit 0
        GLint loc = glGetUniformLocation(prog, "intensityMap");
        if (loc != -1)
            glUniform1i(loc, 0);
        else
            std::cerr << "'intensityMap' uniform not found\n";
    }

    return prog;
}



GLFWwindow* initWindow(int width, int height, const char* title) {
    // 1) initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return nullptr;
    }

    // 2) set hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,    GLFW_OPENGL_CORE_PROFILE);

    // 3) create the window
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return nullptr;
    }

    // 4) make its context current _before_ loading GL function pointers
    glfwMakeContextCurrent(window);

    // 5) load all OpenGL function pointers via GLAD (or your loader)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    // 6) any other per-window GL state
    glEnable(GL_DEPTH_TEST);

    // **finally** return the valid window pointer
    return window;
}



/// Uploads a grayscale texture from a [0..1] intensity matrix.
/// @param intensity  2D array of size [width][height] with values in [0,1].
/// @returns          OpenGL texture name, or 0 on failure.
GLuint createIntensityTexture(const std::vector<std::vector<double>>& intensity) {
    if (intensity.empty() || intensity[0].empty()) {
        std::cerr << "createIntensityTexture: input matrix is empty\n";
        return 0;
    }

    const int texW = static_cast<int>(intensity.size());
    const int texH = static_cast<int>(intensity[0].size());

    // Flatten into a contiguous float array in x-major order:
    std::vector<float> pixels;
    pixels.reserve(texW * texH);
    for (int y = 0; y < texH; ++y) {
        for (int x = 0; x < texW; ++x) {
            pixels.push_back(static_cast<float>(intensity[x][y]));
        }
    }

    // Generate and bind
    GLuint texID = 0;
    glGenTextures(1, &texID);
    if (texID == 0) {
        std::cerr << "createIntensityTexture: glGenTextures failed\n";
        return 0;
    }
    glBindTexture(GL_TEXTURE_2D, texID);

    // Setup filtering and wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Upload to GPU
    glTexImage2D(
      GL_TEXTURE_2D,    // target
      0,                // mipmap level
      GL_R32F,          // internal format
      texW,             // width
      texH,             // height
      0,                // border
      GL_RED,           // format of the pixel data
      GL_FLOAT,         // data type
      pixels.data()     // pointer to data
    );

    // Unbind for safety
    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}


void updateIntensityTexture(GLuint tex,
                            const std::vector<std::vector<double>>& intensity)
{
    int texW = static_cast<int>(intensity.size());
    int texH = static_cast<int>(intensity[0].size());

    // flatten as before
    std::vector<float> pixels;
    pixels.reserve(texW * texH);
    for (int y = 0; y < texH; ++y)
        for (int x = 0; x < texW; ++x)
            pixels.push_back(static_cast<float>(intensity[x][y]));

    // bind & re-upload
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,             // mip level
                    0, 0,          // xoffset, yoffset
                    texW, texH,    // width, height
                    GL_RED,
                    GL_FLOAT,
                    pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}




/// Creates a fullscreen quad VAO/VBO with position (loc=0) and UV (loc=1).
/// @returns {vao, vbo} pair, or {0, 0} on failure.
std::pair<GLuint, GLuint> createQuadVAO() {
    // vertex data: X, Y, Z, U, V
    static constexpr std::array<float, 30> quadVerts = {
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f
    };

    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers      (1, &vbo);
    if (vao == 0 || vbo == 0) {
        std::cerr << "createQuadVAO: glGen failed\n";
        return {0, 0};
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts.data(), GL_STATIC_DRAW);

    // position attribute (location = 0)
    glVertexAttribPointer(
        0,                  // layout(location = 0)
        3,                  // vec3
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        5 * sizeof(float),  // stride (bytes between vertices)
        (void*)0            // offset in the buffer
    );
    glEnableVertexAttribArray(0);

    // uv attribute (location = 1)
    glVertexAttribPointer(
        1,                          // layout(location = 1)
        2,                          // vec2
        GL_FLOAT,                   // type
        GL_FALSE,                   // normalized?
        5 * sizeof(float),          // stride
        (void*)(3 * sizeof(float))  // offset = after XYZ
    );
    glEnableVertexAttribArray(1);

    // unbind for safety
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return {vao, vbo};
}



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



void renderScene(GLFWwindow* window,
                 GLuint shaderProgram,
                 GLuint intensityTex,
                 GLuint quadVAO)
{
    // clear
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use shader
    glUseProgram(shaderProgram);

    // build & upload matrices
    glm::mat4 model      = glm::mat4(1.0f);
    glm::mat4 view       = glm::lookAt(cameraPos,
                                       cameraPos + cameraFront,
                                       cameraUp);
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        float(SCR_WIDTH) / SCR_HEIGHT,
        0.1f, 100.0f
    );

    GLint locModel      = glGetUniformLocation(shaderProgram, "model");
    GLint locView       = glGetUniformLocation(shaderProgram, "view");
    GLint locProjection = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(locModel,      1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(locView,       1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(locProjection, 1, GL_FALSE, glm::value_ptr(projection));

    // bind your intensity map to unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, intensityTex);

    // draw the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}



// Shaders
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

    // Creating intensity distribution
    auto intensity = createIntensity(8, 8); 
    fillRandom(intensity);


    // Initializing GLFW window
    GLFWwindow* window = initWindow(SCR_WIDTH, SCR_HEIGHT, "HoloViz");
    if (!window) return -1;


    // Compiling shaders
    GLuint shaderProgram = createShaderProgram(vertexShaderSource,
                                               fragmentShaderSource);


    // Preparing textures
    GLuint intensityTex = createIntensityTexture(intensity);
    if (!intensityTex) return -1;


    // Preparing VBO
    auto [quadVAO, quadVBO] = createQuadVAO();
    if (quadVAO == 0) return -1;


    // Main loop
    while (!glfwWindowShouldClose(window)) {

        processInput(window);

        fillRandom(intensity);                    
        updateIntensityTexture(intensityTex, intensity);

        renderScene(window,
                    shaderProgram,
                    intensityTex,
                    quadVAO);

        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }


    // Cleanup
    glDeleteVertexArrays(1,&quadVAO);
    glDeleteBuffers     (1,&quadVBO);
    glDeleteProgram     (shaderProgram);
    glfwTerminate();
    return 0;
}

