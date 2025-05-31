#include <iostream>
#include <cmath>

#include "textures.h"


// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Upload a grayscale texture from a [0..1] intensity matrix.
// @param intensity  2D array of size [width][height] with values in [0,1].
// @returns          OpenGL texture name, or 0 on failure.
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
            pixels.push_back(static_cast<float>(intensity[x][y])); //std::log(1 + static_cast<float>(intensity[x][y])) / std::log(2)
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

// Update texture based on new intensity matrix
void updateIntensityTexture(GLuint tex,
                            const std::vector<std::vector<double>>& intensity)
{
    int texW = static_cast<int>(intensity.size());
    int texH = static_cast<int>(intensity[0].size());

    // allocate once at full size
    std::vector<float> pixels(texW * texH);

    // parallel flatten; use collapse(2) to parallelize both loops
    #pragma omp parallel for collapse(2)
    for (int y = 0; y < texH; ++y) {
        for (int x = 0; x < texW; ++x) {
            pixels[y * texW + x] = static_cast<float>(intensity[x][y]);//std::log(1 + static_cast<float>(intensity[x][y])) / std::log(2);//static_cast<float>(intensity[x][y]);
        } //exp(static_cast<float>(intensity[x][y])) / exp(1.0);
    }

    glBindTexture(GL_TEXTURE_2D, tex);
    // re-upload all pixels in one shot
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,             // mip level
                    0, 0,          // xoffset, yoffset
                    texW, texH,    // width, height
                    GL_RED,
                    GL_FLOAT,
                    pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}
