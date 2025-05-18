#include "settings.h"

// Window dimensions
const unsigned int SCR_WIDTH  = 1280;
const unsigned int SCR_HEIGHT = 720;

// Camera
float yaw       = -90.0f;
float pitch     =   0.0f;
const float rotSpeed    = 45.0f; // deg/sec
glm::vec3 cameraStartPos = {0.0f, 0.0f, 3.0f};
glm::vec3 cameraPos      = cameraStartPos;
glm::vec3 cameraFront    = {0.0f, 0.0f, -1.0f};
glm::vec3 cameraUp       = {0.0f, 1.0f, 0.0f};
float deltaTime = 0.0f;
float lastFrame = 0.0f;
