#include "GameManager.h"
#include <iostream>

const float MATCH_CHECK_DELAY = 1.0f; // seconds to show cards before checking match

GameManager::GameManager() 
    : currentState(GameState::IDLE), firstCard(nullptr), secondCard(nullptr),
      score(0), moves(0), totalMatches(0), currentStreak(0), totalStreakBonus(0),
      stateTimer(0.0f), matchDelay(MATCH_CHECK_DELAY), gameWon(false), animating(false) {
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
    totalMatches = 0;
    currentStreak = 0;
    totalStreakBonus = 0;
    stateTimer = 0.0f;
    gameWon = false;
    animating = false;
    
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
                animating = false; // Clear animation guard
                setState(GameState::IDLE);
            }
            break;
            
        case GameState::FLIPPING_SECOND:
            // Wait for second card to finish flipping
            if (secondCard && !secondCard->getIsFlipping()) {
                setState(GameState::CHECK_MATCH);
                stateTimer = 0.0f;
                // Keep animating = true during match check delay
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
                animating = false; // Clear animation guard
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
    // Click guards: ignore clicks during animations or invalid states
    if (!clickedCard || animating || clickedCard->getIsFlipping()) {
        return;
    }
    
    // Ignore clicks on already matched or face-up cards
    if (clickedCard->getState() == CardState::MATCHED || 
        clickedCard->getState() == CardState::FACE_UP) {
        return;
    }
    
    // Prevent same-card double-click
    if (firstCard && firstCard->getId() == clickedCard->getId()) {
        return;
    }
    
    switch (currentState) {
        case GameState::IDLE:
            if (clickedCard->getState() == CardState::FACE_DOWN) {
                animating = true; // Set animation guard
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
            // Ignore clicks during other states (already handled by animating guard)
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
        
        // Enhanced HUD with better readability and score breakdown
        std::string timeText = "Time: " + std::to_string(minutes) + ":" + 
                              (seconds < 10 ? "0" : "") + std::to_string(seconds);
        renderer->renderEnhancedText(timeText, -0.95f, 0.9f, 1.2f);
        
        std::string movesText = "Moves: " + std::to_string(moves);
        renderer->renderEnhancedText(movesText, -0.95f, 0.8f, 1.2f);
        
        std::string scoreText = "Score: " + std::to_string(score);
        renderer->renderEnhancedText(scoreText, -0.95f, 0.7f, 1.2f);
        
        // Display current streak if active
        if (currentStreak > 0) {
            std::string streakText = "Streak: " + std::to_string(currentStreak);
            renderer->renderEnhancedText(streakText, -0.95f, 0.6f, 1.0f);
        }
        
        // Display match count
        std::string matchText = "Matches: " + std::to_string(totalMatches);
        renderer->renderEnhancedText(matchText, -0.95f, 0.5f, 1.0f);
        
        // Display win message with score breakdown
        if (gameWon) {
            ScoreBreakdown breakdown = calculateFinalScore();
            
            renderer->renderEnhancedText("YOU WON!", -0.3f, 0.2f, 2.5f);
            
            // Score breakdown
            std::string finalScoreText = "Final Score: " + std::to_string(breakdown.finalScore);
            renderer->renderEnhancedText(finalScoreText, -0.4f, 0.0f, 1.5f);
            
            std::string baseText = "Base: " + std::to_string(breakdown.base);
            renderer->renderEnhancedText(baseText, -0.4f, -0.1f, 1.0f);
            
            std::string bonusText = "Bonuses: +" + std::to_string(breakdown.bonuses);
            renderer->renderEnhancedText(bonusText, -0.4f, -0.2f, 1.0f);
            
            std::string penaltyText = "Penalties: -" + std::to_string(breakdown.penalties);
            renderer->renderEnhancedText(penaltyText, -0.4f, -0.3f, 1.0f);
            
            // Display stars
            std::string starsText = "Stars: ";
            for (int i = 0; i < breakdown.stars; i++) {
                starsText += "*";
            }
            for (int i = breakdown.stars; i < 3; i++) {
                starsText += "-";
            }
            renderer->renderEnhancedText(starsText, -0.4f, -0.4f, 1.2f);
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
            totalMatches++;
            currentStreak++;
            
            // Calculate match bonus with streak multiplier
            int matchBonus = ScoringConstants::MATCH_BONUS;
            int streakBonus = ScoringConstants::STREAK_BONUS_STEP * currentStreak;
            totalStreakBonus += streakBonus;
            score += matchBonus + streakBonus;
            
            // Set cards to matched state (they stay face up)
            firstCard->setState(CardState::MATCHED);
            secondCard->setState(CardState::MATCHED);
            
            std::cout << "Match found! Streak: " << currentStreak 
                      << ", Bonus: " << (matchBonus + streakBonus) << std::endl;
        } else {
            // No match - reset streak and flip cards back
            currentStreak = 0;
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

GameManager::ScoreBreakdown GameManager::calculateFinalScore() const {
    float elapsedSeconds = getElapsedTime();
    
    int base = ScoringConstants::BASE_SCORE;
    int penalties = static_cast<int>(elapsedSeconds * ScoringConstants::TIME_PENALTY_PER_SEC) + 
                   (moves * ScoringConstants::MOVE_PENALTY);
    int bonuses = (totalMatches * ScoringConstants::MATCH_BONUS) + totalStreakBonus;
    
    int rawScore = base + bonuses - penalties;
    int finalScore = std::max(ScoringConstants::MIN_SCORE, 
                             std::min(ScoringConstants::BASE_SCORE, rawScore));

    // Persist the clamped final score when game is won
    if (gameWon) {
        // (mutable state change in const context avoided; kept read-only)
    }
    
    // Calculate stars based on final score
    int stars = 0;
    if (finalScore >= 900) stars = 3;
    else if (finalScore >= 650) stars = 2;
    else if (finalScore >= 350) stars = 1;
    
    return {base, penalties, bonuses, finalScore, stars};
}