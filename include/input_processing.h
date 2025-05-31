#ifndef INPUT_PROCESSING_H
#define INPUT_PROCESSING_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "visible_field.h"


// ╔════════════════════╗ 
// ║ External variables ║
// ╚════════════════════╝

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

extern obj_visible_plate v_plate;

extern GLuint intensityTex;



// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Read input keys and take corresponding actions
void processInput(GLFWwindow *window, obj_visible_plate v_plate);


#endif // INPUT_PROCESSING_H
