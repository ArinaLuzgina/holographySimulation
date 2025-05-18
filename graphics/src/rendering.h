#ifndef RENDERING_H
#define RENDERING_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


// ╔════════════════════╗ 
// ║ External variables ║
// ╚════════════════════╝

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;



// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Initialize and configure window
GLFWwindow* initWindow(int width, int height, const char* title); 

// Make all recalculations
void renderScene(GLFWwindow* window,
                 GLuint shaderProgram,
                 GLuint intensityTex,
                 GLuint quadVAO);

// Cleanup before closing
void cleanupGraphics(GLuint vao, GLuint vbo, GLuint program);

#endif // RENDERING_H
