#include "settings.h"
#include "input_processing.h"

// Window dimensions
const unsigned int SCR_WIDTH  = 1280;
const unsigned int SCR_HEIGHT = 720;


// Camera
float startYaw   = -90.0f;
float yaw        = startYaw;

float startPitch = 0.0f;
float pitch      = startPitch;

const float moveSpeed = 1e-5;
const float rotSpeed  = 45.0f; // deg/sec

glm::vec3 cameraStartPos   = {0.0f, 0.0f, 3e-6};
glm::vec3 cameraPos        = cameraStartPos;
glm::vec3 cameraStartFront = {0.0f, 1.0f, -1.0f};
glm::vec3 cameraFront      = cameraStartFront;
glm::vec3 cameraUp         = {0.0f, 1.0f, 0.0f};

float deltaTime = 0.0f;
float lastFrame = 0.0f;


// Plate
const float scale = 1e-6;


obj_visible_plate v_plate; 
GLuint intensityTex = 0;
