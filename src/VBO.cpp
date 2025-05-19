#include <iostream>
#include <array>

#include "VBO.h"


// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Create a fullscreen quad VAO/VBO with position (loc=0) and UV (loc=1).
// @returns {vao, vbo} pair, or {0, 0} on failure.
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
