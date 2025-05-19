#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "rendering.h"
#include "settings.h"


// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Initialize and configure window
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


// Make all recalculations
void renderScene(GLuint shaderProgram,
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
        0.01f * scale, 100.0f * scale
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


// Cleanup before closing
void cleanupGraphics(GLuint vao, GLuint vbo, GLuint program) {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    glfwTerminate();
}

