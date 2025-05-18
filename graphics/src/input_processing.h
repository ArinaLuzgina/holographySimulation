#ifndef INPUT_PROCESSING_H
#define INPUT_PROCESSING_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


// ╔════════════════════╗ 
// ║ External variables ║
// ╚════════════════════╝

extern float yaw;
extern float pitch;
extern const float rotSpeed;
extern glm::vec3 cameraStartPos;
extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;
extern float deltaTime;
extern float lastFrame;



// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Read input keys and take corresponding actions
void processInput(GLFWwindow *window);


#endif // INPUT_PROCESSING_H
