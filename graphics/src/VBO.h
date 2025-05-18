#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include <utility>


// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Create a fullscreen quad VAO/VBO with position (loc=0) and UV (loc=1).
// @returns {vao, vbo} pair, or {0, 0} on failure.
std::pair<GLuint, GLuint> createQuadVAO();


#endif // VBO_H
