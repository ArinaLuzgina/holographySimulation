#include <assert.h>

#include "shaders.h"
#include "VBO.h"
#include "textures.h"
#include "input_processing.h"
#include "rendering.h"
#include "settings.h"

#include "graphics.h"

#include "visible_field.h"

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
    intensityTex = createIntensityTexture(intensity);
    assert(intensityTex);


    // Preparing VBO
    auto [quadVAO, quadVBO] = createQuadVAO();
    assert(quadVAO != 0);

    v_plate = obj_visible_plate(1e-6);
    v_plate.read_intensity_matrix("book_2_intensity.txt");

    // Main loop
    while (!glfwWindowShouldClose(window)) {

        processInput(window);

        renderScene(shaderProgram,
                    intensityTex,
                    quadVAO);

        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }


    // Cleanup
    cleanupGraphics(quadVAO, quadVBO, shaderProgram);
} 
