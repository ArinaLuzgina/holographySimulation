#include <iostream>

#include "shaders.h"


// ╔═══════════╗ 
// ║ Variables ║
// ╚═══════════╝

// Vertex shader.
// Defines projection of the texture on the holografic plate
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


// Fragment shader.
// Maps intensity value with pixel color on the plate.
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



// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Compile a single shader, print errors on failure
GLuint compileShader(GLenum type, const char* src) {
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
