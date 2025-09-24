#include "Deck.h"
#include <algorithm>
#include <random>
#include <chrono>

Deck::Deck() {
}

Deck::~Deck() {
    cards.clear();
}

void Deck::createPairs(int numPairs) {
    cards.clear();
    cards.reserve(numPairs * 2);
    
    // Create pairs of cards with varied suits and ranks
    for (int i = 0; i < numPairs; i++) {
        int rank = (i % 13) + 1; // 1-13 (Ace to King)
        int suit = i % 4; // Distribute suits evenly: 0-3 (Hearts, Diamonds, Clubs, Spades)
        
        // Create two identical cards (a pair)
        cards.push_back(std::make_unique<Card>(i * 2, rank, suit));
        cards.push_back(std::make_unique<Card>(i * 2 + 1, rank, suit));
    }
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