#include "textures.h"
#include "visible_field.h"

#include "input_processing.h"

// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Read input keys and take corresponding actions
void processInput(GLFWwindow *window) {
    // reset
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        // -------------------------------------
        // Intensity recalculation
        v_plate.update_visible_matrix(cameraPos.x, cameraPos.y, cameraPos.z);
        // -------------------------------------
        updateIntensityTexture(intensityTex, v_plate.visible_matrix);
    }

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    float camSpeed = moveSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += camSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= camSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * camSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * camSpeed;

    float angleStep = rotSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS) yaw   -= angleStep;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) yaw   += angleStep;
    if (glfwGetKey(window, GLFW_KEY_UP   ) == GLFW_PRESS) pitch += angleStep;
    if (glfwGetKey(window, GLFW_KEY_DOWN ) == GLFW_PRESS) pitch -= angleStep;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
