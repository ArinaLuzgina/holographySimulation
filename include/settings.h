#ifndef SETTINGS_H
#define SETTINGS_H

#include <glm/glm.hpp>

// ╔═══════════╗ 
// ║ Variables ║
// ╚═══════════╝

// Window dimensions
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

// Camera
extern float startYaw;
extern float yaw;

extern float startPitch;
extern float pitch;

extern const float moveSpeed;
extern const float rotSpeed;

extern glm::vec3 cameraStartPos;
extern glm::vec3 cameraPos;
extern glm::vec3 cameraStartFront;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;

extern float deltaTime;
extern float lastFrame;

// Plate
extern const float scale;

#endif // SETTINGS_H
