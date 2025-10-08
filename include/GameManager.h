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

// Scoring constants
struct ScoringConstants {
    static constexpr int BASE_SCORE = 1000;
    static constexpr float TIME_PENALTY_PER_SEC = 1.5f;
    static constexpr int MOVE_PENALTY = 5;
    static constexpr int MATCH_BONUS = 25;
    static constexpr int STREAK_BONUS_STEP = 10;
    static constexpr int MIN_SCORE = 0;
};

class GameManager {
private:
    GameState currentState;
    Deck deck;
    Card* firstCard;
    Card* secondCard;
    int score;
    int moves;
    int totalMatches;
    int currentStreak;
    int totalStreakBonus;
    float stateTimer;
    float matchDelay;
    bool gameWon;
    bool animating; // Guard for preventing clicks during animations
    
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
    int getTotalMatches() const { return totalMatches; }
    int getCurrentStreak() const { return currentStreak; }
    bool isGameWon() const { return gameWon; }
    bool isAnimating() const { return animating; }
    float getElapsedTime() const;
    
    // Scoring
    struct ScoreBreakdown {
        int base;
        int penalties;
        int bonuses;
        int finalScore;
        int stars;
    };
    ScoreBreakdown calculateFinalScore() const;
    
    // Reset game
    void reset(int numPairs);
};