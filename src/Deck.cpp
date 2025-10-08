#include "Deck.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <map>
#include <iostream>
#include <stdexcept>

Deck::Deck() {
}

Deck::~Deck() {
    cards.clear();
}

void Deck::createPairs(int numPairs) {
    cards.clear();
    cards.reserve(numPairs * 2);
    
    // Create unique pattern pool - ensure we have enough unique patterns
    std::vector<std::pair<int, int>> patternPool;
    patternPool.reserve(numPairs);
    
    // Generate unique rank-suit combinations to ensure distinct patterns
    int patternCount = 0;
    for (int suit = 0; suit < 4 && patternCount < numPairs; suit++) {
        for (int rank = 1; rank <= 13 && patternCount < numPairs; rank++) {
            patternPool.push_back({rank, suit});
            patternCount++;
        }
    }
    
    // Shuffle the pattern pool to randomize selection
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(patternPool.begin(), patternPool.end(), gen);
    
    // Select first numPairs patterns (no replacement)
    std::vector<std::pair<int, int>> selectedPatterns(patternPool.begin(), patternPool.begin() + numPairs);
    
    // Create exactly two cards for each selected pattern
    for (int i = 0; i < numPairs; i++) {
        int rank = selectedPatterns[i].first;
        int suit = selectedPatterns[i].second;
        
        // Create pair with unique IDs but identical pattern
        cards.push_back(std::make_unique<Card>(i * 2, rank, suit));
        cards.push_back(std::make_unique<Card>(i * 2 + 1, rank, suit));
    }
    
    // Runtime assertion: verify exactly 2 cards per pattern
    std::map<std::pair<int, int>, int> patternFreq;
    for (const auto& card : cards) {
        std::pair<int, int> pattern = {card->getRank(), card->getSuit()};
        patternFreq[pattern]++;
    }
    
    // Assert each pattern appears exactly twice
    for (const auto& freq : patternFreq) {
        if (freq.second != 2) {
            std::cerr << "ERROR: Pattern (" << freq.first.first << "," << freq.first.second 
                      << ") appears " << freq.second << " times instead of 2!" << std::endl;
            throw std::runtime_error("Deck creation failed: pattern frequency error");
        }
    }
    
    // Assert total card count
    if (cards.size() != static_cast<size_t>(numPairs * 2)) {
        throw std::runtime_error("Deck creation failed: incorrect total card count");
    }
    
    std::cout << "Deck created successfully: " << numPairs << " unique patterns, " 
              << cards.size() << " total cards" << std::endl;
}

void Deck::shuffle() {
    // Use Fisher-Yates shuffle algorithm
    std::random_device rd;
    std::mt19937 gen(rd());
    
    for (int i = cards.size() - 1; i > 0; i--) {
        std::uniform_int_distribution<> dis(0, i);
        int j = dis(gen);
        std::swap(cards[i], cards[j]);
    }
}

void Deck::layoutCards(int rows, int cols, float cardWidth, float cardHeight, float spacing) {
    // Calculate total grid dimensions
    float totalWidth = cols * cardWidth + (cols - 1) * spacing;
    float totalHeight = rows * cardHeight + (rows - 1) * spacing;
    
    // Calculate starting position (center the grid)
    float startX = -totalWidth / 2.0f + cardWidth / 2.0f;
    float startY = totalHeight / 2.0f - cardHeight / 2.0f;
    
    int cardIndex = 0;
    for (int row = 0; row < rows && cardIndex < cards.size(); row++) {
        for (int col = 0; col < cols && cardIndex < cards.size(); col++) {
            float x = startX + col * (cardWidth + spacing);
            float y = startY - row * (cardHeight + spacing);
            
            cards[cardIndex]->setPosition(x, y);
            cards[cardIndex]->setTargetPosition(x, y);
            cardIndex++;
        }
    }
}

Card* Deck::getCardAt(float x, float y) {
    for (auto& card : cards) {
        if (card->contains(x, y)) {
            return card.get();
        }
    }
    return nullptr;
}

std::vector<Card*> Deck::getCardsInState(CardState state) {
    std::vector<Card*> result;
    for (auto& card : cards) {
        if (card->getState() == state) {
            result.push_back(card.get());
        }
    }
    return result;
}