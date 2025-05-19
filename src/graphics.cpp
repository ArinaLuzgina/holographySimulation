#include <assert.h>

#include "shaders.h"
#include "VBO.h"
#include "textures.h"
#include "input_processing.h"
#include "rendering.h"
#include "settings.h"

#include "graphics.h"


// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Run everything
void runSimulation(std::vector<std::vector<double>>& intensity) {

    // Initializing GLFW window
    GLFWwindow* window = initWindow(SCR_WIDTH, SCR_HEIGHT, "HoloViz");
    assert(window);


    // Compiling shaders
    GLuint shaderProgram = createShaderProgram(vertexShaderSource,
                                               fragmentShaderSource);


    // Preparing textures
    GLuint intensityTex = createIntensityTexture(intensity);
    assert(intensityTex);


    // Preparing VBO
    auto [quadVAO, quadVBO] = createQuadVAO();
    assert(quadVAO != 0);


    // Main loop
    while (!glfwWindowShouldClose(window)) {

        processInput(window);

        // -------------------------------------
        // *Insert intensity recalculation here*
        // -------------------------------------
        updateIntensityTexture(intensityTex, intensity);

        renderScene(shaderProgram,
                    intensityTex,
                    quadVAO);

        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }


    // Cleanup
    cleanupGraphics(quadVAO, quadVBO, shaderProgram);
} 
