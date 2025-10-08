#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

// Simple FontGlyph (future expansion if real TTF rasterization added)
struct FontGlyph {
    GLuint texture = 0;
    int width = 0;
    int height = 0;
    int advance = 0;
};

class Renderer {
private:
    GLuint VAO, VBO, EBO;
    GLuint cardShaderProgram;
    GLuint textShaderProgram;
    float viewportWidth = 1280.0f;
    float viewportHeight = 720.0f;
    
public:
    Renderer();
    ~Renderer();
    
    bool initialize();
    void cleanup();
    
    // Shader management
    GLuint loadShader(const std::string& vertexPath, const std::string& fragmentPath);
    GLuint compileShader(const std::string& source, GLenum shaderType);
    
    // Texture management
    GLuint loadTexture(const std::string& path);
    GLuint createCardTexture(int rank, int suit, bool isFront = true);
    
    // Rendering functions
    void renderCard(float x, float y, float width, float height, float rotation, GLuint texture);
    void renderText(const std::string& text, float x, float y, float scale);
    void renderEnhancedText(const std::string& text, float x, float y, float scale);
    
    // Matrix utilities
    void setProjectionMatrix(int width, int height);
    void setViewport(float w, float h) { viewportWidth = w; viewportHeight = h; }
    float getViewportWidth() const { return viewportWidth; }
    float getViewportHeight() const { return viewportHeight; }
    
    // Getters
    GLuint getCardShaderProgram() const { return cardShaderProgram; }
    GLuint getTextShaderProgram() const { return textShaderProgram; }
};