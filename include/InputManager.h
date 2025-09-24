#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class InputManager {
private:
    GLFWwindow* window;
    bool mousePressed;
    bool mouseReleased;
    glm::vec2 mousePosition;
    glm::vec2 lastMousePosition;
    
public:
    InputManager(GLFWwindow* win);
    ~InputManager();
    
    void update();
    void processInput(float deltaTime);
    
    // Mouse input
    bool isMousePressed() const { return mousePressed; }
    bool isMouseReleased() const { return mouseReleased; }
    glm::vec2 getMousePosition() const { return mousePosition; }
    glm::vec2 getLastMousePosition() const { return lastMousePosition; }
    
    // Convert screen coordinates to world coordinates
    glm::vec2 screenToWorld(glm::vec2 screenPos, int windowWidth, int windowHeight);
    
    // Keyboard input
    bool isKeyPressed(int key);
    bool isKeyReleased(int key);
    
    // Static callback functions for GLFW
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};