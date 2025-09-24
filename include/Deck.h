#pragma once
#include "Card.h"
#include <vector>
#include <memory>

class Deck {
private:
    std::vector<std::unique_ptr<Card>> cards;
    
public:
    Deck();
    ~Deck();
    
    void createPairs(int numPairs);
    void shuffle();
    void layoutCards(int rows, int cols, float cardWidth, float cardHeight, float spacing);
    
    std::vector<std::unique_ptr<Card>>& getCards() { return cards; }
    const std::vector<std::unique_ptr<Card>>& getCards() const { return cards; }
    
    // Find card at screen position
    Card* getCardAt(float x, float y);
    
    // Get cards in specific states
    std::vector<Card*> getCardsInState(CardState state);
};