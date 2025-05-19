#ifndef TEXTURES_H
#define TEXTURES_H

#include <glad/glad.h>
#include <vector>


// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Upload a grayscale texture from a [0..1] intensity matrix.
// @param intensity  2D array of size [width][height] with values in [0,1].
// @returns          OpenGL texture name, or 0 on failure.
GLuint createIntensityTexture(const std::vector<std::vector<double>>& intensity);

// Update texture based on new intensity matrix
void updateIntensityTexture(GLuint tex,
                            const std::vector<std::vector<double>>& intensity);


#endif // TEXTURES_H
