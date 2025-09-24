#pragma once
#include <GL/glew.h>
#include <string>

class Renderer {
private:
    GLuint VAO, VBO, EBO;
    GLuint cardShaderProgram;
    GLuint textShaderProgram;
    
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
    
    // Matrix utilities
    void setProjectionMatrix(int width, int height);
    
    // Getters
    GLuint getCardShaderProgram() const { return cardShaderProgram; }
    GLuint getTextShaderProgram() const { return textShaderProgram; }
};