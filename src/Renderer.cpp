#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// Global VAO for card rendering (accessible by Card class)
GLuint g_cardVAO = 0;

// Vertex data for a quad
const float quadVertices[] = {
    // positions        // texture coords
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f
};

const unsigned int quadIndices[] = {
    0, 1, 2,
    2, 3, 0
};

Renderer::Renderer() : VAO(0), VBO(0), EBO(0), cardShaderProgram(0), textShaderProgram(0) {
}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize() {
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }
    
    // Create and compile shaders
    cardShaderProgram = loadShader("shaders/card_vertex.glsl", "shaders/card_fragment.glsl");
    if (cardShaderProgram == 0) {
        std::cerr << "Failed to load card shaders" << std::endl;
        return false;
    }
    
    // Set up vertex data and buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Set global VAO for card rendering
    g_cardVAO = VAO;
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return true;
}

void Renderer::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
        g_cardVAO = 0; // Reset global VAO
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    if (cardShaderProgram != 0) {
        glDeleteProgram(cardShaderProgram);
        cardShaderProgram = 0;
    }
    if (textShaderProgram != 0) {
        glDeleteProgram(textShaderProgram);
        textShaderProgram = 0;
    }
}

GLuint Renderer::loadShader(const std::string& vertexPath, const std::string& fragmentPath) {
    // Read vertex shader source
    std::ifstream vertexFile(vertexPath);
    if (!vertexFile.is_open()) {
        std::cerr << "Failed to open vertex shader file: " << vertexPath << std::endl;
        return 0;
    }
    
    std::stringstream vertexStream;
    vertexStream << vertexFile.rdbuf();
    std::string vertexSource = vertexStream.str();
    vertexFile.close();
    
    // Read fragment shader source
    std::ifstream fragmentFile(fragmentPath);
    if (!fragmentFile.is_open()) {
        std::cerr << "Failed to open fragment shader file: " << fragmentPath << std::endl;
        return 0;
    }
    
    std::stringstream fragmentStream;
    fragmentStream << fragmentFile.rdbuf();
    std::string fragmentSource = fragmentStream.str();
    fragmentFile.close();
    
    // Compile shaders
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    
    if (vertexShader == 0 || fragmentShader == 0) {
        return 0;
    }
    
    // Link shaders into program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Check linking
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
    
    // Clean up individual shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

GLuint Renderer::compileShader(const std::string& source, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, NULL);
    glCompileShader(shader);
    
    // Check compilation
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint Renderer::loadTexture(const std::string& path) {
    // This is a placeholder - you would use a library like stb_image to load actual textures
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // For now, create a solid color texture as placeholder
    unsigned char data[] = {255, 255, 255, 255}; // White
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    return texture;
}

GLuint Renderer::createCardTexture(int rank, int suit, bool isFront) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Create simple colored texture based on card properties
    // This is a placeholder - in a real implementation you'd draw card graphics
    unsigned char color[4];
    if (isFront) {
        // Front of card - color based on suit
        switch (suit) {
            case 0: color[0] = 255; color[1] = 0; color[2] = 0; break;   // Red (Hearts)
            case 1: color[0] = 255; color[1] = 100; color[2] = 0; break; // Orange (Diamonds)
            case 2: color[0] = 0; color[1] = 0; color[2] = 0; break;     // Black (Clubs)
            case 3: color[0] = 0; color[1] = 0; color[2] = 255; break;   // Blue (Spades)
        }
        color[3] = 255; // Alpha
    } else {
        // Back of card - uniform blue
        color[0] = 50; color[1] = 50; color[2] = 200; color[3] = 255;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, color);
    
    return texture;
}

void Renderer::renderCard(float x, float y, float width, float height, float rotation, GLuint texture) {
    if (VAO == 0 || cardShaderProgram == 0) return;
    
    glUseProgram(cardShaderProgram);
    glBindVertexArray(VAO);
    
    if (texture != 0) {
        glBindTexture(GL_TEXTURE_2D, texture);
    }
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
}

void Renderer::renderText(const std::string& text, float x, float y, float scale) {
    // Enhanced pixel-based text rendering with better visibility
    // This creates clear, readable text on the OpenGL window
    
    glDisable(GL_DEPTH_TEST); // Render UI on top
    
    float charWidth = 0.04f * scale;   // Slightly larger characters
    float charHeight = 0.06f * scale;
    float spacing = charWidth * 1.15f; // Better spacing
    
    // First pass: render black outline/shadow for better readability
    float shadowOffset = 0.003f * scale;
    for (int shadowPass = 0; shadowPass < 2; shadowPass++) {
        float renderX = x + (shadowPass == 0 ? shadowOffset : 0);
        float renderY = y - (shadowPass == 0 ? shadowOffset : 0);
        bool isMainText = (shadowPass == 1);
    
        // Create orthographic projection for UI rendering
        glm::mat4 orthoProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        
        for (size_t i = 0; i < text.length(); i++) {
            char c = text[i];
            if (c == ' ') {
                renderX += spacing;
                continue;
            }
            
            // Create a larger bitmap texture for each character
            const int CHAR_SIZE = 12; // Increased for better quality
            unsigned char charBitmap[CHAR_SIZE * CHAR_SIZE * 4]; // RGBA
            
            // Fill with transparent background
            for (int j = 0; j < CHAR_SIZE * CHAR_SIZE * 4; j += 4) {
                if (isMainText) {
                    charBitmap[j] = 255;     // R - White text
                    charBitmap[j+1] = 255;   // G  
                    charBitmap[j+2] = 255;   // B
                } else {
                    charBitmap[j] = 0;       // R - Black shadow
                    charBitmap[j+1] = 0;     // G
                    charBitmap[j+2] = 0;     // B
                }
                charBitmap[j+3] = 0;         // A (transparent)
            }
        
            // Draw character pattern (enhanced pixel art for 12x12)
            switch (c) {
                case '0': case 'O':
                    // Draw O shape
                    for (int row = 2; row < 10; row++) {
                        for (int col = 2; col < 10; col++) {
                            if ((row == 2 || row == 9) && (col > 3 && col < 8)) {
                                charBitmap[(row * CHAR_SIZE + col) * 4 + 3] = 255;
                            } else if ((col == 2 || col == 9) && (row > 3 && row < 8)) {
                                charBitmap[(row * CHAR_SIZE + col) * 4 + 3] = 255;
                            } else if ((row == 3 || row == 8) && (col == 3 || col == 8)) {
                                charBitmap[(row * CHAR_SIZE + col) * 4 + 3] = 255;
                            }
                        }
                    }
                    break;
                case '1': case 'I': case 'l':
                    // Draw I/1 shape
                    for (int row = 1; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 5) * 4 + 3] = 255;
                        charBitmap[(row * CHAR_SIZE + 6) * 4 + 3] = 255;
                    }
                    // Add serifs for better visibility
                    for (int col = 4; col < 8; col++) {
                        charBitmap[(1 * CHAR_SIZE + col) * 4 + 3] = 255; // top
                        charBitmap[(10 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    break;
                case '2':
                    // Draw 2 shape
                    for (int col = 2; col < 9; col++) {
                        charBitmap[(2 * CHAR_SIZE + col) * 4 + 3] = 255; // top
                        charBitmap[(6 * CHAR_SIZE + col) * 4 + 3] = 255; // middle
                        charBitmap[(10 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    // Right side top
                    for (int row = 3; row < 6; row++) {
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255;
                    }
                    // Left side bottom
                    for (int row = 7; row < 10; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255;
                    }
                    break;
                case '3':
                    // Draw 3 shape
                    for (int col = 2; col < 8; col++) {
                        charBitmap[(2 * CHAR_SIZE + col) * 4 + 3] = 255; // top
                        charBitmap[(6 * CHAR_SIZE + col) * 4 + 3] = 255; // middle
                        charBitmap[(10 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 3; row < 10; row++) {
                        if (row != 6) charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255; // right side
                    }
                    break;
                case '4':
                    // Draw 4 shape
                    for (int row = 2; row < 7; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255; // left side
                    }
                    for (int col = 2; col < 9; col++) {
                        charBitmap[(6 * CHAR_SIZE + col) * 4 + 3] = 255; // middle
                    }
                    for (int row = 2; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255; // right side
                    }
                    break;
                case '5': case 'S': 
                    // Draw 5/S shape
                    for (int col = 2; col < 8; col++) {
                        charBitmap[(2 * CHAR_SIZE + col) * 4 + 3] = 255; // top
                        charBitmap[(6 * CHAR_SIZE + col) * 4 + 3] = 255; // middle
                        charBitmap[(10 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 3; row < 6; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255; // left side top
                    }
                    for (int row = 7; row < 10; row++) {
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255; // right side bottom
                    }
                    break;
                case '6':
                    // Draw 6 shape (enhanced for 12x12)
                    for (int col = 2; col < 8; col++) {
                        charBitmap[(2 * CHAR_SIZE + col) * 4 + 3] = 255; // top
                        charBitmap[(6 * CHAR_SIZE + col) * 4 + 3] = 255; // middle
                        charBitmap[(10 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 3; row < 10; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255; // left side
                    }
                    for (int row = 7; row < 10; row++) {
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255; // right side bottom
                    }
                    break;
                case '7':
                    // Draw 7 shape  
                    for (int col = 2; col < 9; col++) {
                        charBitmap[(2 * CHAR_SIZE + col) * 4 + 3] = 255; // top
                    }
                    for (int row = 3; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255; // right side
                    }
                    break;
                case '8':
                    // Draw 8 shape (enhanced)
                    for (int col = 2; col < 8; col++) {
                        charBitmap[(2 * CHAR_SIZE + col) * 4 + 3] = 255; // top
                        charBitmap[(6 * CHAR_SIZE + col) * 4 + 3] = 255; // middle
                        charBitmap[(10 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 3; row < 10; row++) {
                        if (row != 6) {
                            charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255; // left side
                            charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255; // right side
                        }
                    }
                    break;
                case '9':
                    // Draw 9 shape (enhanced)
                    for (int col = 2; col < 8; col++) {
                        charBitmap[(2 * CHAR_SIZE + col) * 4 + 3] = 255; // top
                        charBitmap[(6 * CHAR_SIZE + col) * 4 + 3] = 255; // middle
                        charBitmap[(10 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 3; row < 6; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255; // left side top
                    }
                    for (int row = 3; row < 10; row++) {
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255; // right side
                    }
                    break;
                case 'T': case 't':
                    // Draw T shape (enhanced)
                    for (int col = 1; col < 10; col++) {
                        charBitmap[(2 * CHAR_SIZE + col) * 4 + 3] = 255; // top
                    }
                    for (int row = 3; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 5) * 4 + 3] = 255; // center vertical
                        charBitmap[(row * CHAR_SIZE + 6) * 4 + 3] = 255;
                    }
                    break;
                case 'M': case 'm':
                    // Draw M shape (enhanced)
                    for (int row = 2; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 1) * 4 + 3] = 255; // left
                        charBitmap[(row * CHAR_SIZE + 9) * 4 + 3] = 255; // right
                    }
                    // Middle peaks  
                    charBitmap[(3 * CHAR_SIZE + 3) * 4 + 3] = 255;
                    charBitmap[(3 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(4 * CHAR_SIZE + 4) * 4 + 3] = 255;
                    charBitmap[(4 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(5 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    break;
                case 'E': case 'e':
                    // Draw E shape (enhanced)
                    for (int row = 2; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255; // left side
                    }
                    for (int col = 2; col < 8; col++) {
                        charBitmap[(2 * CHAR_SIZE + col) * 4 + 3] = 255; // top
                        charBitmap[(6 * CHAR_SIZE + col) * 4 + 3] = 255; // middle
                        charBitmap[(10 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    break;
                case 'V': case 'v':
                    // Draw V shape (enhanced)
                    for (int row = 2; row < 7; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255; // left
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255; // right
                    }
                    charBitmap[(7 * CHAR_SIZE + 3) * 4 + 3] = 255;
                    charBitmap[(7 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(8 * CHAR_SIZE + 4) * 4 + 3] = 255;
                    charBitmap[(8 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(9 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    break;
                case 'C': case 'c':
                    // Draw C shape (enhanced)
                    for (int col = 3; col < 8; col++) {
                        charBitmap[(2 * CHAR_SIZE + col) * 4 + 3] = 255; // top
                        charBitmap[(10 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 3; row < 10; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255; // left side
                    }
                    break;
                case 'R': case 'r':
                    // Draw R shape (enhanced)
                    for (int row = 2; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255; // left side
                    }
                    for (int col = 2; col < 7; col++) {
                        charBitmap[(2 * CHAR_SIZE + col) * 4 + 3] = 255; // top
                        charBitmap[(6 * CHAR_SIZE + col) * 4 + 3] = 255; // middle
                    }
                    for (int row = 3; row < 6; row++) {
                        charBitmap[(row * CHAR_SIZE + 7) * 4 + 3] = 255; // right side top
                    }
                    // Diagonal leg
                    charBitmap[(7 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    charBitmap[(8 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(9 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(10 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    break;
                case 'Y': case 'y':
                    // Draw Y shape (enhanced)
                    charBitmap[(2 * CHAR_SIZE + 2) * 4 + 3] = 255; // top left
                    charBitmap[(2 * CHAR_SIZE + 8) * 4 + 3] = 255; // top right
                    charBitmap[(3 * CHAR_SIZE + 3) * 4 + 3] = 255; // converging
                    charBitmap[(3 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(4 * CHAR_SIZE + 4) * 4 + 3] = 255;
                    charBitmap[(4 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    for (int row = 5; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 5) * 4 + 3] = 255; // center vertical
                    }
                    break;
                case 'U': case 'u':
                    // Draw U shape (enhanced)
                    for (int row = 2; row < 9; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255; // left side
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255; // right side
                    }
                    for (int col = 3; col < 8; col++) {
                        charBitmap[(9 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                        charBitmap[(10 * CHAR_SIZE + col) * 4 + 3] = 255;
                    }
                    break;
                case 'W': case 'w':
                    // Draw W shape (enhanced)
                    for (int row = 2; row < 9; row++) {
                        charBitmap[(row * CHAR_SIZE + 1) * 4 + 3] = 255; // left
                        charBitmap[(row * CHAR_SIZE + 9) * 4 + 3] = 255; // right
                    }
                    charBitmap[(7 * CHAR_SIZE + 5) * 4 + 3] = 255; // middle peak
                    charBitmap[(8 * CHAR_SIZE + 3) * 4 + 3] = 255; // middle supports
                    charBitmap[(8 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(9 * CHAR_SIZE + 4) * 4 + 3] = 255;
                    charBitmap[(9 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(10 * CHAR_SIZE + 5) * 4 + 3] = 255; // bottom center
                    break;
                case 'N': case 'n':
                    // Draw N shape (enhanced)
                    for (int row = 2; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255; // left
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255; // right
                        if (row > 2 && row < 10) {
                            int diagCol = 2 + (row - 2);
                            if (diagCol < 8) {
                                charBitmap[(row * CHAR_SIZE + diagCol) * 4 + 3] = 255; // diagonal
                            }
                        }
                    }
                    break;
                case ':':
                    // Draw colon (enhanced)
                    charBitmap[(4 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    charBitmap[(4 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(5 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    charBitmap[(5 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(7 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    charBitmap[(7 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(8 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    charBitmap[(8 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    break;
                case '!':
                    // Draw exclamation mark (enhanced)
                    for (int row = 2; row < 8; row++) {
                        charBitmap[(row * CHAR_SIZE + 5) * 4 + 3] = 255;
                        charBitmap[(row * CHAR_SIZE + 6) * 4 + 3] = 255;
                    }
                    charBitmap[(9 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    charBitmap[(9 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(10 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    charBitmap[(10 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    break;
                default:
                    // Draw a small rectangle for unknown characters (enhanced)
                    for (int row = 4; row < 8; row++) {
                        for (int col = 3; col < 8; col++) {
                            charBitmap[(row * CHAR_SIZE + col) * 4 + 3] = 255;
                        }
                    }
                    break;
            }
        
            // Flip the bitmap vertically to fix mirroring issue
            unsigned char flippedBitmap[CHAR_SIZE * CHAR_SIZE * 4];
            for (int row = 0; row < CHAR_SIZE; row++) {
                for (int col = 0; col < CHAR_SIZE; col++) {
                    int srcIndex = (row * CHAR_SIZE + col) * 4;
                    int dstIndex = ((CHAR_SIZE - 1 - row) * CHAR_SIZE + col) * 4;
                    flippedBitmap[dstIndex + 0] = charBitmap[srcIndex + 0];
                    flippedBitmap[dstIndex + 1] = charBitmap[srcIndex + 1];
                    flippedBitmap[dstIndex + 2] = charBitmap[srcIndex + 2];
                    flippedBitmap[dstIndex + 3] = charBitmap[srcIndex + 3];
                }
            }
            
            // Create texture from flipped bitmap
            GLuint charTexture;
            glGenTextures(1, &charTexture);
            glBindTexture(GL_TEXTURE_2D, charTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHAR_SIZE, CHAR_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, flippedBitmap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            // Render the character quad
            glUseProgram(cardShaderProgram);
            
            // Set up model matrix for the character
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(renderX + charWidth/2, renderY + charHeight/2, 0.1f));
            model = glm::scale(model, glm::vec3(charWidth, charHeight, 1.0f));
            
            // Set uniforms
            GLint modelLoc = glGetUniformLocation(cardShaderProgram, "model");
            GLint viewLoc = glGetUniformLocation(cardShaderProgram, "view");
            GLint projLoc = glGetUniformLocation(cardShaderProgram, "projection");
            
            if (modelLoc >= 0) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
            if (viewLoc >= 0) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
            if (projLoc >= 0) glUniformMatrix4fv(projLoc, 1, GL_FALSE, &orthoProjection[0][0]);
            
            // Bind texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, charTexture);
            GLint texLoc = glGetUniformLocation(cardShaderProgram, "cardTexture");
            if (texLoc >= 0) glUniform1i(texLoc, 0);
            
            // Draw character
            if (VAO != 0) {
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
            
            // Cleanup
            glDeleteTextures(1, &charTexture);
            
            renderX += spacing;
        }
    }
    
    glEnable(GL_DEPTH_TEST); // Re-enable depth testing
}

void Renderer::renderEnhancedText(const std::string& text, float x, float y, float scale) {
    // Enhanced text rendering with better contrast and readability
    glDisable(GL_DEPTH_TEST);
    
    // Scale font size based on viewport dimensions for better readability
    float baseSize = std::min(viewportWidth, viewportHeight) / 40.0f;
    float charWidth = (baseSize * scale) / viewportWidth * 2.0f;   // Normalized to screen space
    float charHeight = (baseSize * scale * 1.4f) / viewportHeight * 2.0f;
    float spacing = charWidth * 1.2f;
    
    // Create semi-transparent background panel for better readability
    float panelWidth = text.length() * spacing + 0.02f;
    float panelHeight = charHeight + 0.02f;
    
    // Render background panel
    glUseProgram(cardShaderProgram);
    
    // Create dark semi-transparent background
    const int PANEL_SIZE = 4;
    unsigned char panelData[PANEL_SIZE * PANEL_SIZE * 4];
    for (int i = 0; i < PANEL_SIZE * PANEL_SIZE * 4; i += 4) {
        panelData[i] = 0;     // R
        panelData[i+1] = 0;   // G
        panelData[i+2] = 0;   // B
        panelData[i+3] = 128; // A (semi-transparent)
    }
    
    GLuint panelTexture;
    glGenTextures(1, &panelTexture);
    glBindTexture(GL_TEXTURE_2D, panelTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, PANEL_SIZE, PANEL_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, panelData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Render background panel
    glm::mat4 orthoProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 panelModel = glm::mat4(1.0f);
    panelModel = glm::translate(panelModel, glm::vec3(x + panelWidth/2 - 0.01f, y + panelHeight/2 - 0.01f, 0.05f));
    panelModel = glm::scale(panelModel, glm::vec3(panelWidth, panelHeight, 1.0f));
    
    GLint modelLoc = glGetUniformLocation(cardShaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(cardShaderProgram, "view");
    GLint projLoc = glGetUniformLocation(cardShaderProgram, "projection");
    
    if (modelLoc >= 0) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &panelModel[0][0]);
    if (viewLoc >= 0) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    if (projLoc >= 0) glUniformMatrix4fv(projLoc, 1, GL_FALSE, &orthoProjection[0][0]);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, panelTexture);
    GLint texLoc = glGetUniformLocation(cardShaderProgram, "cardTexture");
    if (texLoc >= 0) glUniform1i(texLoc, 0);
    
    if (VAO != 0) {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    glDeleteTextures(1, &panelTexture);
    
    // Now render text with outline for better visibility
    float shadowOffsets[] = {-0.002f, 0.002f, 0.0f};
    bool isShadowPass[] = {true, true, false};
    
    for (int pass = 0; pass < 3; pass++) {
        float renderX = x + shadowOffsets[pass];
        float renderY = y + shadowOffsets[pass];
        bool isMainText = !isShadowPass[pass];
        
        for (size_t i = 0; i < text.length(); i++) {
            char c = text[i];
            if (c == ' ') {
                renderX += spacing;
                continue;
            }
            
            const int CHAR_SIZE = 16; // Even larger for better quality
            unsigned char charBitmap[CHAR_SIZE * CHAR_SIZE * 4];
            
            // Fill with transparent background
            for (int j = 0; j < CHAR_SIZE * CHAR_SIZE * 4; j += 4) {
                if (isMainText) {
                    charBitmap[j] = 255;     // R - Bright white text
                    charBitmap[j+1] = 255;   // G  
                    charBitmap[j+2] = 255;   // B
                } else {
                    charBitmap[j] = 0;       // R - Black outline
                    charBitmap[j+1] = 0;     // G
                    charBitmap[j+2] = 0;     // B
                }
                charBitmap[j+3] = 0;         // A (transparent)
            }
            
            // Enhanced character patterns with complete alphabet support
            switch (c) {
                case '0': case 'O': case 'o':
                    for (int row = 3; row < 13; row++) {
                        for (int col = 3; col < 13; col++) {
                            if ((row == 3 || row == 12) && (col > 4 && col < 11)) {
                                charBitmap[(row * CHAR_SIZE + col) * 4 + 3] = 255;
                            } else if ((col == 3 || col == 12) && (row > 4 && row < 11)) {
                                charBitmap[(row * CHAR_SIZE + col) * 4 + 3] = 255;
                            } else if ((row == 4 || row == 11) && (col == 4 || col == 11)) {
                                charBitmap[(row * CHAR_SIZE + col) * 4 + 3] = 255;
                            }
                        }
                    }
                    break;
                case '1': case 'I': case 'i':
                    for (int row = 2; row < 14; row++) {
                        charBitmap[(row * CHAR_SIZE + 7) * 4 + 3] = 255;
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255;
                    }
                    for (int col = 5; col < 11; col++) {
                        charBitmap[(2 * CHAR_SIZE + col) * 4 + 3] = 255;
                        charBitmap[(13 * CHAR_SIZE + col) * 4 + 3] = 255;
                    }
                    break;
                case '2':
                    for (int col = 3; col < 12; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 4; row < 8; row++) {
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right top
                    }
                    for (int row = 9; row < 12; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left bottom
                    }
                    break;
                case '3':
                    for (int col = 3; col < 11; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 4; row < 12; row++) {
                        if (row != 8) charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right
                    }
                    break;
                case '4':
                    for (int row = 3; row < 9; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                    }
                    for (int col = 3; col < 12; col++) {
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                    }
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right
                    }
                    break;
                case '5': case 'S': case 's':
                    for (int col = 3; col < 11; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 4; row < 8; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left top
                    }
                    for (int row = 9; row < 12; row++) {
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right bottom
                    }
                    break;
                case '6':
                    for (int col = 3; col < 11; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 4; row < 12; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                    }
                    for (int row = 9; row < 12; row++) {
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right bottom
                    }
                    break;
                case '7':
                    for (int col = 3; col < 12; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                    }
                    for (int row = 4; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right
                    }
                    break;
                case '8':
                    for (int col = 3; col < 11; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 4; row < 12; row++) {
                        if (row != 8) {
                            charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                            charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right
                        }
                    }
                    break;
                case '9':
                    for (int col = 3; col < 11; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 4; row < 8; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left top
                    }
                    for (int row = 4; row < 12; row++) {
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right
                    }
                    break;
                case 'A': case 'a':
                    for (int col = 4; col < 11; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                    }
                    for (int row = 4; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right
                    }
                    break;
                case 'B': case 'b':
                    for (int col = 3; col < 10; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                    }
                    for (int row = 4; row < 8; row++) {
                        charBitmap[(row * CHAR_SIZE + 10) * 4 + 3] = 255; // right top
                    }
                    for (int row = 9; row < 12; row++) {
                        charBitmap[(row * CHAR_SIZE + 10) * 4 + 3] = 255; // right bottom
                    }
                    break;
                case 'C': case 'c':
                    for (int col = 4; col < 11; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 4; row < 12; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                    }
                    break;
                case 'D': case 'd':
                    for (int col = 3; col < 10; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                    }
                    for (int row = 4; row < 12; row++) {
                        charBitmap[(row * CHAR_SIZE + 10) * 4 + 3] = 255; // right
                    }
                    charBitmap[(4 * CHAR_SIZE + 11) * 4 + 3] = 255;
                    charBitmap[(11 * CHAR_SIZE + 11) * 4 + 3] = 255;
                    break;
                case 'E': case 'e':
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                    }
                    for (int col = 3; col < 11; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    break;
                case 'F': case 'f':
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                    }
                    for (int col = 3; col < 11; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                    }
                    break;
                case 'G': case 'g':
                    for (int col = 4; col < 11; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 4; row < 12; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                    }
                    for (int row = 8; row < 12; row++) {
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right bottom
                    }
                    for (int col = 8; col < 11; col++) {
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle bar
                    }
                    break;
                case 'H': case 'h':
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right
                    }
                    for (int col = 3; col < 12; col++) {
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                    }
                    break;
                case 'J': case 'j':
                    for (int row = 3; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right
                    }
                    for (int col = 4; col < 11; col++) {
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    charBitmap[(11 * CHAR_SIZE + 3) * 4 + 3] = 255;
                    break;
                case 'K': case 'k':
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                    }
                    // Diagonal lines
                    charBitmap[(5 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(6 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(7 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    charBitmap[(8 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(9 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(10 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    charBitmap[(4 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    charBitmap[(3 * CHAR_SIZE + 9) * 4 + 3] = 255;
                    charBitmap[(11 * CHAR_SIZE + 9) * 4 + 3] = 255;
                    charBitmap[(12 * CHAR_SIZE + 10) * 4 + 3] = 255;
                    break;
                case 'L': case 'l':
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                    }
                    for (int col = 3; col < 11; col++) {
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    break;
                case 'M': case 'm':
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255;  // left
                        charBitmap[(row * CHAR_SIZE + 12) * 4 + 3] = 255; // right
                    }
                    // Middle peaks
                    charBitmap[(4 * CHAR_SIZE + 4) * 4 + 3] = 255;
                    charBitmap[(4 * CHAR_SIZE + 10) * 4 + 3] = 255;
                    charBitmap[(5 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    charBitmap[(5 * CHAR_SIZE + 9) * 4 + 3] = 255;
                    charBitmap[(6 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(6 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    charBitmap[(7 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    break;
                case 'N': case 'n':
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right
                        // Diagonal
                        int diagCol = 3 + (row - 3);
                        if (diagCol <= 11) {
                            charBitmap[(row * CHAR_SIZE + diagCol) * 4 + 3] = 255;
                        }
                    }
                    break;
                case 'P': case 'p':
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                    }
                    for (int col = 3; col < 10; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                    }
                    for (int row = 4; row < 8; row++) {
                        charBitmap[(row * CHAR_SIZE + 10) * 4 + 3] = 255; // right top
                    }
                    break;
                case 'Q': case 'q':
                    for (int col = 4; col < 10; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(11 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    for (int row = 4; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                        charBitmap[(row * CHAR_SIZE + 10) * 4 + 3] = 255; // right
                    }
                    // Tail
                    charBitmap[(10 * CHAR_SIZE + 9) * 4 + 3] = 255;
                    charBitmap[(11 * CHAR_SIZE + 10) * 4 + 3] = 255;
                    charBitmap[(12 * CHAR_SIZE + 11) * 4 + 3] = 255;
                    break;
                case 'R': case 'r':
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                    }
                    for (int col = 3; col < 10; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(8 * CHAR_SIZE + col) * 4 + 3] = 255;  // middle
                    }
                    for (int row = 4; row < 8; row++) {
                        charBitmap[(row * CHAR_SIZE + 10) * 4 + 3] = 255; // right top
                    }
                    // Diagonal leg
                    charBitmap[(9 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(10 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(11 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    charBitmap[(12 * CHAR_SIZE + 9) * 4 + 3] = 255;
                    break;
                case 'T': case 't':
                    for (int col = 2; col < 13; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                    }
                    for (int row = 4; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 7) * 4 + 3] = 255;  // center
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255;
                    }
                    break;
                case 'U': case 'u':
                    for (int row = 3; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right
                    }
                    for (int col = 4; col < 11; col++) {
                        charBitmap[(11 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255;
                    }
                    break;
                case 'V': case 'v':
                    for (int row = 3; row < 9; row++) {
                        charBitmap[(row * CHAR_SIZE + 3) * 4 + 3] = 255;  // left
                        charBitmap[(row * CHAR_SIZE + 11) * 4 + 3] = 255; // right
                    }
                    charBitmap[(9 * CHAR_SIZE + 4) * 4 + 3] = 255;
                    charBitmap[(9 * CHAR_SIZE + 10) * 4 + 3] = 255;
                    charBitmap[(10 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    charBitmap[(10 * CHAR_SIZE + 9) * 4 + 3] = 255;
                    charBitmap[(11 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(11 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    charBitmap[(12 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    break;
                case 'W': case 'w':
                    for (int row = 3; row < 11; row++) {
                        charBitmap[(row * CHAR_SIZE + 2) * 4 + 3] = 255;  // left
                        charBitmap[(row * CHAR_SIZE + 12) * 4 + 3] = 255; // right
                    }
                    charBitmap[(9 * CHAR_SIZE + 7) * 4 + 3] = 255;  // center peak
                    charBitmap[(10 * CHAR_SIZE + 4) * 4 + 3] = 255; // left support
                    charBitmap[(10 * CHAR_SIZE + 10) * 4 + 3] = 255; // right support
                    charBitmap[(11 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    charBitmap[(11 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(11 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    charBitmap[(11 * CHAR_SIZE + 9) * 4 + 3] = 255;
                    charBitmap[(12 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    break;
                case 'X': case 'x':
                    // Draw X pattern
                    for (int i = 0; i < 10; i++) {
                        charBitmap[((3 + i) * CHAR_SIZE + (3 + i)) * 4 + 3] = 255; // main diagonal
                        charBitmap[((3 + i) * CHAR_SIZE + (12 - i)) * 4 + 3] = 255; // anti-diagonal
                    }
                    break;
                case 'Y': case 'y':
                    charBitmap[(3 * CHAR_SIZE + 3) * 4 + 3] = 255;  // top left
                    charBitmap[(3 * CHAR_SIZE + 11) * 4 + 3] = 255; // top right
                    charBitmap[(4 * CHAR_SIZE + 4) * 4 + 3] = 255;  // converging
                    charBitmap[(4 * CHAR_SIZE + 10) * 4 + 3] = 255;
                    charBitmap[(5 * CHAR_SIZE + 5) * 4 + 3] = 255;
                    charBitmap[(5 * CHAR_SIZE + 9) * 4 + 3] = 255;
                    charBitmap[(6 * CHAR_SIZE + 6) * 4 + 3] = 255;
                    charBitmap[(6 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    for (int row = 7; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 7) * 4 + 3] = 255; // center vertical
                    }
                    break;
                case 'Z': case 'z':
                    for (int col = 3; col < 12; col++) {
                        charBitmap[(3 * CHAR_SIZE + col) * 4 + 3] = 255;  // top
                        charBitmap[(12 * CHAR_SIZE + col) * 4 + 3] = 255; // bottom
                    }
                    // Diagonal
                    for (int i = 0; i < 9; i++) {
                        charBitmap[((4 + i) * CHAR_SIZE + (11 - i)) * 4 + 3] = 255;
                    }
                    break;
                case ':':
                    charBitmap[(5 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(5 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    charBitmap[(6 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(6 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    charBitmap[(9 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(9 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    charBitmap[(10 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(10 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    break;
                case '!':
                    for (int row = 3; row < 10; row++) {
                        charBitmap[(row * CHAR_SIZE + 7) * 4 + 3] = 255;
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255;
                    }
                    charBitmap[(11 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(11 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    charBitmap[(12 * CHAR_SIZE + 7) * 4 + 3] = 255;
                    charBitmap[(12 * CHAR_SIZE + 8) * 4 + 3] = 255;
                    break;
                case '|':
                    for (int row = 3; row < 13; row++) {
                        charBitmap[(row * CHAR_SIZE + 7) * 4 + 3] = 255;
                        charBitmap[(row * CHAR_SIZE + 8) * 4 + 3] = 255;
                    }
                    break;
                default:
                    // Improved block for unknown characters
                    for (int row = 5; row < 11; row++) {
                        for (int col = 4; col < 12; col++) {
                            if (row == 5 || row == 10 || col == 4 || col == 11) {
                                charBitmap[(row * CHAR_SIZE + col) * 4 + 3] = 255;
                            }
                        }
                    }
                    break;
            }
            
            // Flip the bitmap vertically to fix mirroring issue in enhanced text
            unsigned char flippedBitmap[CHAR_SIZE * CHAR_SIZE * 4];
            for (int row = 0; row < CHAR_SIZE; row++) {
                for (int col = 0; col < CHAR_SIZE; col++) {
                    int srcIndex = (row * CHAR_SIZE + col) * 4;
                    int dstIndex = ((CHAR_SIZE - 1 - row) * CHAR_SIZE + col) * 4;
                    flippedBitmap[dstIndex + 0] = charBitmap[srcIndex + 0];
                    flippedBitmap[dstIndex + 1] = charBitmap[srcIndex + 1];
                    flippedBitmap[dstIndex + 2] = charBitmap[srcIndex + 2];
                    flippedBitmap[dstIndex + 3] = charBitmap[srcIndex + 3];
                }
            }
            
            // Create and render character texture
            GLuint charTexture;
            glGenTextures(1, &charTexture);
            glBindTexture(GL_TEXTURE_2D, charTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHAR_SIZE, CHAR_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, flippedBitmap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(renderX + charWidth/2, renderY + charHeight/2, 0.1f));
            model = glm::scale(model, glm::vec3(charWidth, charHeight, 1.0f));
            
            if (modelLoc >= 0) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, charTexture);
            if (texLoc >= 0) glUniform1i(texLoc, 0);
            
            if (VAO != 0) {
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
            
            glDeleteTextures(1, &charTexture);
            renderX += spacing;
        }
    }
    
    glEnable(GL_DEPTH_TEST);
}

void Renderer::setProjectionMatrix(int width, int height) {
    // Update viewport dimensions for font scaling
    viewportWidth = static_cast<float>(width);
    viewportHeight = static_cast<float>(height);
    
    // Set OpenGL viewport
    glViewport(0, 0, width, height);
}