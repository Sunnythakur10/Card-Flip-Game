#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// Global VAO for rendering (defined in Renderer.cpp)
extern GLuint g_cardVAO;

enum class CardState {
    FACE_DOWN,
    FLIPPING_TO_FACE_UP,
    FACE_UP,
    FLIPPING_TO_FACE_DOWN,
    MATCHED
};

class Card {
private:
    int id;
    int rank;
    int suit;
    CardState state;
    glm::vec2 position;
    glm::vec2 targetPosition;
    float angleY;
    float flipProgress;
    float flipSpeed;
    bool isFlipping;
    GLuint frontTexture;
    GLuint backTexture;
    
    // Helper function to create detailed card textures
    GLuint createDetailedCardTexture(int rank, int suit, bool isFront);

public:
    Card(int cardId, int cardRank, int cardSuit);
    ~Card();
    
    void update(float deltaTime);
    void draw(GLuint shaderProgram, glm::mat4 viewMatrix, glm::mat4 projMatrix);
    void onClick();
    void startFlip();
    void setPosition(float x, float y);
    void setTargetPosition(float x, float y);
    void setState(CardState newState);
    
    // Getters
    int getId() const { return id; }
    int getRank() const { return rank; }
    int getSuit() const { return suit; }
    CardState getState() const { return state; }
    glm::vec2 getPosition() const { return position; }
    bool getIsFlipping() const { return isFlipping; }
    
    // Check if point is inside card bounds
    bool contains(float x, float y) const;
    
    // Match check
    bool matches(const Card& other) const;
};