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
    // Simple bitmap-style text rendering using OpenGL rectangles
    // Each character is rendered as colored rectangles forming letter shapes
    
    glDisable(GL_DEPTH_TEST); // Render UI on top
    
    float charWidth = 0.05f * scale;
    float charHeight = 0.08f * scale;
    float spacing = charWidth * 1.2f;
    
    // Use simple colored rectangles for each character
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        if (c == ' ') {
            x += spacing;
            continue;
        }
        
        // Set up vertices for a character rectangle
        float vertices[] = {
            x, y, 0.0f,
            x + charWidth, y, 0.0f,
            x + charWidth, y + charHeight, 0.0f,
            x, y + charHeight, 0.0f
        };
        
        unsigned int indices[] = {0, 1, 2, 2, 3, 0};
        
        // Create temporary VAO for this character
        GLuint charVAO, charVBO, charEBO;
        glGenVertexArrays(1, &charVAO);
        glGenBuffers(1, &charVBO);
        glGenBuffers(1, &charEBO);
        
        glBindVertexArray(charVAO);
        glBindBuffer(GL_ARRAY_BUFFER, charVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, charEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Simple character rendering with basic shapes based on ASCII
        glUseProgram(cardShaderProgram); // Reuse card shader for simplicity
        
        // Create a simple 1x1 white texture for text
        unsigned char whitePixel[] = {255, 255, 255, 255};
        GLuint textTexture;
        glGenTextures(1, &textTexture);
        glBindTexture(GL_TEXTURE_2D, textTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        // Set uniform values for white color
        GLint textureLocation = glGetUniformLocation(cardShaderProgram, "ourTexture");
        if (textureLocation != -1) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textTexture);
            glUniform1i(textureLocation, 0);
        }
        
        // Draw the character
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        // Cleanup
        glDeleteTextures(1, &textTexture);
        glDeleteVertexArrays(1, &charVAO);
        glDeleteBuffers(1, &charVBO);
        glDeleteBuffers(1, &charEBO);
        
        x += spacing;
    }
    
    glEnable(GL_DEPTH_TEST); // Re-enable depth testing
}

void Renderer::setProjectionMatrix(int width, int height) {
    // This would set up the projection matrix for the shaders
    // For now, this is handled in the main render loop
}