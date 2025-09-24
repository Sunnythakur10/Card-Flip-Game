#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>

#include "GameManager.h"
#include "Renderer.h"
#include "InputManager.h"

// Window dimensions
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const char* WINDOW_TITLE = "Memory Card Matching Game";

// Game settings
const int NUM_PAIRS = 8; // 16 cards total

class MemoryCardGame {
private:
    GLFWwindow* window;
    GameManager gameManager;
    Renderer renderer;
    std::unique_ptr<InputManager> inputManager;
    
    // Timing
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    float deltaTime;
    
    // Camera matrices
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    
public:
    MemoryCardGame() : window(nullptr), deltaTime(0.0f) {}
    
    bool initialize() {
        // Initialize GLFW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        
        // Set GLFW window hints
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        
        // Create window
        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }
        
        // Make OpenGL context current
        glfwMakeContextCurrent(window);
        
        // Initialize renderer
        if (!renderer.initialize()) {
            std::cerr << "Failed to initialize renderer" << std::endl;
            return false;
        }
        
        // Initialize input manager
        inputManager = std::make_unique<InputManager>(window);
        
        // Initialize game
        gameManager.initialize(NUM_PAIRS);
        
        // Set up camera matrices
        setupCamera();
        
        // Set up OpenGL state
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
        // Initialize timing
        lastFrameTime = std::chrono::high_resolution_clock::now();
        
        std::cout << "Memory Card Matching Game initialized successfully!" << std::endl;
        std::cout << "Instructions:" << std::endl;
        std::cout << "- Click on cards to flip them" << std::endl;
        std::cout << "- Match pairs to score points" << std::endl;
        std::cout << "- Press R to restart" << std::endl;
        std::cout << "- Press ESC to exit" << std::endl;
        
        return true;
    }
    
    void setupCamera() {
        // Set up view matrix (identity - looking down -Z axis)
        viewMatrix = glm::mat4(1.0f);
        
        // Set up projection matrix (orthographic for 2D)
        float aspectRatio = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
        float orthoSize = 400.0f; // Adjust this to zoom in/out
        projectionMatrix = glm::ortho(-orthoSize * aspectRatio, orthoSize * aspectRatio,
                                    -orthoSize, orthoSize, -100.0f, 100.0f);
    }
    
    void updateDeltaTime() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastFrameTime);
        deltaTime = duration.count() / 1000000.0f; // Convert to seconds
        lastFrameTime = currentTime;
        
        // Cap delta time to prevent large jumps
        if (deltaTime > 0.016f) { // ~60 FPS minimum
            deltaTime = 0.016f;
        }
    }
    
    void processInput() {
        glfwPollEvents();
        inputManager->processInput(deltaTime);
        
        // Handle mouse clicks
        if (inputManager->isMouseReleased()) {
            glm::vec2 screenPos = inputManager->getMousePosition();
            glm::vec2 worldPos = inputManager->screenToWorld(screenPos, WINDOW_WIDTH, WINDOW_HEIGHT);
            gameManager.handleMouseClick(worldPos.x, worldPos.y);
        }
        
        // Handle reset key
        if (inputManager->isKeyPressed(GLFW_KEY_R)) {
            gameManager.reset(NUM_PAIRS);
            std::cout << "Game reset!" << std::endl;
        }
    }
    
    void update() {
        gameManager.update(deltaTime);
        inputManager->update();
    }
    
    void render() {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render the game
        gameManager.render(renderer.getCardShaderProgram(), viewMatrix, projectionMatrix, &renderer);
        
        // Render UI elements
        renderUI();
        
        // Swap buffers
        glfwSwapBuffers(window);
    }
    
    void renderUI() {
        // Render score, moves, time, etc.
        // This is a placeholder - in a complete implementation you'd render text
        
        // Print game state to console regularly (every second)
        static int lastScore = -1;
        static int lastMoves = -1;
        static int lastTime = -1;
        
        int currentScore = gameManager.getScore();
        int currentMoves = gameManager.getMoves();
        int currentTime = static_cast<int>(gameManager.getElapsedTime());
        
        // Update display when score/moves change OR every second
        if (currentScore != lastScore || currentMoves != lastMoves || currentTime != lastTime) {
            lastScore = currentScore;
            lastMoves = currentMoves;
            lastTime = currentTime;
            
            std::cout << "Score: " << currentScore 
                     << " | Moves: " << currentMoves 
                     << " | Time: " << currentTime << "s";
            
            if (gameManager.isGameWon()) {
                std::cout << " | YOU WIN!";
            }
            std::cout << std::endl;
        }
    }
    
    void run() {
        while (!glfwWindowShouldClose(window)) {
            updateDeltaTime();
            processInput();
            update();
            render();
        }
    }
    
    void cleanup() {
        renderer.cleanup();
        if (window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }
    
    ~MemoryCardGame() {
        cleanup();
    }
};

int main() {
    MemoryCardGame game;
    
    if (!game.initialize()) {
        std::cerr << "Failed to initialize game" << std::endl;
        return -1;
    }
    
    game.run();
    
    return 0;
}