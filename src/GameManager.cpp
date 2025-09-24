#include "GameManager.h"
#include <iostream>

const float MATCH_CHECK_DELAY = 1.0f; // seconds to show cards before checking match

GameManager::GameManager() 
    : currentState(GameState::IDLE), firstCard(nullptr), secondCard(nullptr),
      score(0), moves(0), stateTimer(0.0f), matchDelay(MATCH_CHECK_DELAY),
      gameWon(false) {
}

GameManager::~GameManager() {
}

void GameManager::initialize(int numPairs) {
    // Reset game state
    currentState = GameState::IDLE;
    firstCard = nullptr;
    secondCard = nullptr;
    score = 0;
    moves = 0;
    stateTimer = 0.0f;
    gameWon = false;
    
    // Create and shuffle deck
    deck.createPairs(numPairs);
    deck.shuffle();
    
    // Layout cards in a grid
    int cols = 4;
    int rows = (numPairs * 2 + cols - 1) / cols; // Ceiling division
    deck.layoutCards(rows, cols, 80.0f, 120.0f, 10.0f);
    
    // Record start time
    gameStartTime = std::chrono::high_resolution_clock::now();
}

void GameManager::update(float deltaTime) {
    // Update all cards
    for (auto& card : deck.getCards()) {
        card->update(deltaTime);
    }
    
    // Update state timer
    stateTimer += deltaTime;
    
    // Handle state machine
    switch (currentState) {
        case GameState::IDLE:
            // Waiting for user input
            break;
            
        case GameState::FLIPPING_FIRST:
            // Wait for first card to finish flipping
            if (firstCard && !firstCard->getIsFlipping()) {
                setState(GameState::IDLE);
            }
            break;
            
        case GameState::FLIPPING_SECOND:
            // Wait for second card to finish flipping
            if (secondCard && !secondCard->getIsFlipping()) {
                setState(GameState::CHECK_MATCH);
                stateTimer = 0.0f;
            }
            break;
            
        case GameState::CHECK_MATCH:
            // Show both cards for a brief moment before checking match
            if (stateTimer >= matchDelay) {
                checkForMatch();
            }
            break;
            
        case GameState::RESOLVE:
            // Wait for resolution animation to complete
            if (stateTimer >= 0.5f) { // Short delay for visual feedback
                firstCard = nullptr;
                secondCard = nullptr;
                setState(GameState::IDLE);
                checkWinCondition();
            }
            break;
            
        case GameState::WIN:
            // Game won state - could add celebration animation here
            break;
    }
}

void GameManager::handleCardClick(Card* clickedCard) {
    if (!clickedCard || clickedCard->getIsFlipping()) {
        return;
    }
    
    switch (currentState) {
        case GameState::IDLE:
            if (clickedCard->getState() == CardState::FACE_DOWN) {
                clickedCard->onClick();
                
                if (!firstCard) {
                    firstCard = clickedCard;
                    setState(GameState::FLIPPING_FIRST);
                } else if (clickedCard != firstCard) {
                    secondCard = clickedCard;
                    setState(GameState::FLIPPING_SECOND);
                    moves++;
                }
            }
            break;
            
        case GameState::FLIPPING_FIRST:
            // Only allow selecting second card if it's different from first
            if (clickedCard->getState() == CardState::FACE_DOWN && clickedCard != firstCard) {
                clickedCard->onClick();
                secondCard = clickedCard;
                setState(GameState::FLIPPING_SECOND);
                moves++;
            }
            break;
            
        default:
            // Ignore clicks during other states
            break;
    }
}

void GameManager::render(GLuint shaderProgram, glm::mat4 viewMatrix, glm::mat4 projMatrix, Renderer* renderer) {
    // Render all cards
    for (auto& card : deck.getCards()) {
        card->draw(shaderProgram, viewMatrix, projMatrix);
    }
    
    // Render UI elements on screen
    if (renderer) {
        // Calculate elapsed time
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - gameStartTime);
        int elapsedSeconds = elapsed.count();
        int minutes = elapsedSeconds / 60;
        int seconds = elapsedSeconds % 60;
        
        // Display time in top-left corner
        std::string timeText = "Time: " + std::to_string(minutes) + ":" + 
                              (seconds < 10 ? "0" : "") + std::to_string(seconds);
        renderer->renderText(timeText, -0.95f, 0.9f, 1.0f);
        
        // Display moves in top-right corner
        std::string movesText = "Moves: " + std::to_string(moves);
        renderer->renderText(movesText, 0.5f, 0.9f, 1.0f);
        
        // Display score
        std::string scoreText = "Score: " + std::to_string(score);
        renderer->renderText(scoreText, -0.95f, 0.8f, 1.0f);
        
        // Display win message if game is won
        if (gameWon) {
            std::string winText = "YOU WON!";
            renderer->renderText(winText, -0.2f, 0.0f, 2.0f);
        }
    }
}

void GameManager::setState(GameState newState) {
    currentState = newState;
    stateTimer = 0.0f;
}

void GameManager::checkForMatch() {
    if (firstCard && secondCard) {
        if (firstCard->matches(*secondCard)) {
            // Match found!
            score += 10;
            // Set cards to matched state (they stay face up)
            firstCard->setState(CardState::MATCHED);
            secondCard->setState(CardState::MATCHED);
        } else {
            // No match - flip cards back to face down
            firstCard->setState(CardState::FLIPPING_TO_FACE_DOWN);
            secondCard->setState(CardState::FLIPPING_TO_FACE_DOWN);
            firstCard->startFlip();
            secondCard->startFlip();
        }
    }
    setState(GameState::RESOLVE);
}

void GameManager::resolveMatch() {
    // This would set the matched cards to a permanent face-up state
    // and possibly add visual effects like highlighting
}

void GameManager::checkWinCondition() {
    // Check if all cards are matched
    bool allMatched = true;
    for (auto& card : deck.getCards()) {
        if (card->getState() != CardState::MATCHED && card->getState() != CardState::FACE_UP) {
            allMatched = false;
            break;
        }
    }
    
    if (allMatched) {
        gameWon = true;
        setState(GameState::WIN);
    }
}

void GameManager::handleMouseClick(float x, float y) {
    Card* clickedCard = deck.getCardAt(x, y);
    if (clickedCard) {
        handleCardClick(clickedCard);
    }
}

float GameManager::getElapsedTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - gameStartTime);
    return duration.count() / 1000.0f; // Convert to seconds
}

void GameManager::reset(int numPairs) {
    initialize(numPairs);
}