#pragma once
#include "Deck.h"
#include "Renderer.h"
#include <chrono>

enum class GameState {
    IDLE,
    FLIPPING_FIRST,
    FLIPPING_SECOND,
    CHECK_MATCH,
    RESOLVE,
    WIN
};

class GameManager {
private:
    GameState currentState;
    Deck deck;
    Card* firstCard;
    Card* secondCard;
    int score;
    int moves;
    float stateTimer;
    float matchDelay;
    bool gameWon;
    
    std::chrono::high_resolution_clock::time_point gameStartTime;
    
public:
    GameManager();
    ~GameManager();
    
    void initialize(int numPairs);
    void update(float deltaTime);
    void handleCardClick(Card* clickedCard);
    void render(GLuint shaderProgram, glm::mat4 viewMatrix, glm::mat4 projMatrix, Renderer* renderer);
    
    // State management
    void setState(GameState newState);
    void checkForMatch();
    void resolveMatch();
    void checkWinCondition();
    
    // Input handling
    void handleMouseClick(float x, float y);
    
    // Getters
    GameState getCurrentState() const { return currentState; }
    int getScore() const { return score; }
    int getMoves() const { return moves; }
    bool isGameWon() const { return gameWon; }
    float getElapsedTime() const;
    
    // Reset game
    void reset(int numPairs);
};