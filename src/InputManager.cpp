#include "InputManager.h"

InputManager::InputManager(GLFWwindow* win) 
    : window(win), mousePressed(false), mouseReleased(false),
      mousePosition(0.0f, 0.0f), lastMousePosition(0.0f, 0.0f) {
    
    // Set this instance as user pointer for callbacks
    glfwSetWindowUserPointer(window, this);
    
    // Set up callback functions
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetKeyCallback(window, keyCallback);
}

InputManager::~InputManager() {
}

void InputManager::update() {
    // Reset per-frame states
    mouseReleased = false;
    
    // Update last mouse position
    lastMousePosition = mousePosition;
}

void InputManager::processInput(float deltaTime) {
    // Process keyboard input for game controls
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

glm::vec2 InputManager::screenToWorld(glm::vec2 screenPos, int windowWidth, int windowHeight) {
    // Convert screen coordinates to OpenGL coordinates
    // Screen: (0,0) top-left, (width,height) bottom-right
    // OpenGL: (-width/2, -height/2) bottom-left, (width/2, height/2) top-right
    
    float worldX = (screenPos.x - windowWidth / 2.0f);
    float worldY = (windowHeight / 2.0f - screenPos.y);
    
    return glm::vec2(worldX, worldY);
}

bool InputManager::isKeyPressed(int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool InputManager::isKeyReleased(int key) {
    return glfwGetKey(window, key) == GLFW_RELEASE;
}

// Static callback functions
void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!inputManager) return;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            inputManager->mousePressed = true;
        } else if (action == GLFW_RELEASE) {
            inputManager->mousePressed = false;
            inputManager->mouseReleased = true;
        }
    }
}

void InputManager::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!inputManager) return;
    
    inputManager->mousePosition = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
}

void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!inputManager) return;
    
    // Handle specific key events here if needed
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        // Reset game (this would need to be communicated to the game manager)
    }
}