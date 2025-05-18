#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>


// ╔═══════════╗ 
// ║ Variables ║
// ╚═══════════╝

// Vertex shader.
// Defines projection of the texture on the holografic plate
extern const char *vertexShaderSource;

// Fragment shader.
// Maps intensity value with pixel color on the plate.
extern const char *fragmentShaderSource;



// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Compile a single shader, print errors on failure
GLuint compileShader(GLenum type, const char* src);

// build, link, delete, use, set the "intensityMap" uniform to unit 0
GLuint createShaderProgram(const char* vertSrc, const char* fragSrc);


#endif // SHADERS_H
