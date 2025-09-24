#include "Card.h"
#include "Renderer.h"
#include <iostream>
#include <cmath>

const float CARD_WIDTH = 80.0f;
const float CARD_HEIGHT = 120.0f;
const float FLIP_SPEED = 720.0f; // degrees per second

Card::Card(int cardId, int cardRank, int cardSuit) 
    : id(cardId), rank(cardRank), suit(cardSuit), state(CardState::FACE_DOWN),
      position(0.0f, 0.0f), targetPosition(0.0f, 0.0f), angleY(0.0f),
      flipProgress(0.0f), flipSpeed(FLIP_SPEED), isFlipping(false),
      frontTexture(0), backTexture(0) {
    
    // Create front texture (detailed card design)
    frontTexture = createDetailedCardTexture(rank, suit, true);
    
    // Create back texture (decorative pattern)
    backTexture = createDetailedCardTexture(rank, suit, false);
}

Card::~Card() {
    if (frontTexture != 0) {
        glDeleteTextures(1, &frontTexture);
    }
    if (backTexture != 0) {
        glDeleteTextures(1, &backTexture);
    }
}

void Card::update(float deltaTime) {
    // Update position animation
    if (glm::distance(position, targetPosition) > 0.1f) {
        glm::vec2 direction = targetPosition - position;
        position += direction * deltaTime * 2.0f; // Move towards target
    }
    
    // Update flip animation
    if (isFlipping) {
        flipProgress += deltaTime * flipSpeed;
        angleY = flipProgress;
        
        // Check if flip is halfway (90 degrees) to swap texture
        if (angleY >= 90.0f && angleY < 180.0f) {
            if (state == CardState::FLIPPING_TO_FACE_UP) {
                // Show front texture
            } else if (state == CardState::FLIPPING_TO_FACE_DOWN) {
                // Show back texture
            }
        }
        
        // Check if flip is complete
        if (angleY >= 180.0f) {
            angleY = 0.0f;
            flipProgress = 0.0f;
            isFlipping = false;
            
            if (state == CardState::FLIPPING_TO_FACE_UP) {
                state = CardState::FACE_UP;
            } else if (state == CardState::FLIPPING_TO_FACE_DOWN) {
                state = CardState::FACE_DOWN;
            }
        }
    }
}

void Card::draw(GLuint shaderProgram, glm::mat4 viewMatrix, glm::mat4 projMatrix) {
    glUseProgram(shaderProgram);
    
    // Calculate model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
    model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(CARD_WIDTH, CARD_HEIGHT, 1.0f));
    
    // Set uniforms
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    
    if (modelLoc >= 0) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    if (viewLoc >= 0) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMatrix[0][0]);
    if (projLoc >= 0) glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projMatrix[0][0]);
    
    // Determine which texture to use
    GLuint textureToUse = backTexture;
    if (state == CardState::FACE_UP || state == CardState::MATCHED ||
        (isFlipping && angleY >= 90.0f && state == CardState::FLIPPING_TO_FACE_UP)) {
        textureToUse = frontTexture;
    }
    
    // Bind texture and set uniform
    if (textureToUse != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureToUse);
        GLint texLoc = glGetUniformLocation(shaderProgram, "cardTexture");
        if (texLoc >= 0) glUniform1i(texLoc, 0);
    }
    
    // We need to access the renderer's VAO to actually draw
    // This is a temporary fix - ideally we'd pass the renderer or VAO
    extern GLuint g_cardVAO; // We'll define this in the renderer
    if (g_cardVAO != 0) {
        glBindVertexArray(g_cardVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void Card::onClick() {
    if (state == CardState::FACE_DOWN && !isFlipping) {
        startFlip();
        state = CardState::FLIPPING_TO_FACE_UP;
    }
}

void Card::startFlip() {
    isFlipping = true;
    flipProgress = 0.0f;
    angleY = 0.0f;
}

void Card::setPosition(float x, float y) {
    position = glm::vec2(x, y);
}

void Card::setTargetPosition(float x, float y) {
    targetPosition = glm::vec2(x, y);
}

void Card::setState(CardState newState) {
    state = newState;
}

bool Card::contains(float x, float y) const {
    return (x >= position.x - CARD_WIDTH/2 && x <= position.x + CARD_WIDTH/2 &&
            y >= position.y - CARD_HEIGHT/2 && y <= position.y + CARD_HEIGHT/2);
}

bool Card::matches(const Card& other) const {
    return (rank == other.rank && suit == other.suit && id != other.id);
}

GLuint Card::createDetailedCardTexture(int rank, int suit, bool isFront) {
    const int SIZE = 128; // Increased to 128x128 for more detail
    std::vector<unsigned char> textureData(SIZE * SIZE * 4); // RGBA
    
    if (isFront) {
        // Create front card design with enhanced patterns
        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                int index = (y * SIZE + x) * 4;
                
                // Default cream white background
                textureData[index + 0] = 250; // R
                textureData[index + 1] = 248; // G
                textureData[index + 2] = 245; // B
                textureData[index + 3] = 255; // A
                
                // Outer border (4 pixels thick)
                if (x < 4 || x >= SIZE-4 || y < 4 || y >= SIZE-4) {
                    textureData[index + 0] = 40;  // Dark border
                    textureData[index + 1] = 40;
                    textureData[index + 2] = 40;
                }
                // Inner gold border (2 pixels inside)
                else if (x < 6 || x >= SIZE-6 || y < 6 || y >= SIZE-6) {
                    textureData[index + 0] = 200; // Gold border
                    textureData[index + 1] = 170;
                    textureData[index + 2] = 100;
                }
                
                // Corner decorations (ornamental patterns)
                if ((x < 20 && y < 20) || (x >= SIZE-20 && y < 20) || 
                    (x < 20 && y >= SIZE-20) || (x >= SIZE-20 && y >= SIZE-20)) {
                    if ((x % 4 == 0 || y % 4 == 0) && (x % 8 != 0 && y % 8 != 0)) {
                        textureData[index + 0] = 180;
                        textureData[index + 1] = 180;
                        textureData[index + 2] = 180;
                    }
                }
                
                // Large central suit symbol
                int centerX = SIZE / 2;
                int centerY = SIZE / 2;
                int dx = x - centerX;
                int dy = y - centerY;
                int dist = dx * dx + dy * dy;
                
                // Main suit symbol (larger circle)
                if (dist < 400) { 
                    switch (suit) {
                        case 0: // Hearts (Red) - create heart shape
                            if (dist < 350) {
                                textureData[index + 0] = 220;
                                textureData[index + 1] = 50;
                                textureData[index + 2] = 50;
                            }
                            break;
                        case 1: // Diamonds (Orange) - diamond shape
                            if (abs(dx) + abs(dy) < 20) {
                                textureData[index + 0] = 255;
                                textureData[index + 1] = 140;
                                textureData[index + 2] = 0;
                            }
                            break;
                        case 2: // Clubs (Black) - club shape
                            if (dist < 300 || (abs(dx) < 3 && dy > 0 && dy < 15)) {
                                textureData[index + 0] = 30;
                                textureData[index + 1] = 30;
                                textureData[index + 2] = 30;
                            }
                            break;
                        case 3: // Spades (Blue) - spade shape  
                            if ((dist < 300 && dy <= 0) || (abs(dx) < 3 && dy > 0 && dy < 15)) {
                                textureData[index + 0] = 30;
                                textureData[index + 1] = 50;
                                textureData[index + 2] = 150;
                            }
                            break;
                    }
                }
                
                // Rank numbers in corners (more detailed)
                if (x < 24 && y < 30) { // Top-left corner
                    int rankPattern = (x / 3) + (y / 3) * 8;
                    if (rankPattern % 13 == rank % 13 && rankPattern % 7 == 0) {
                        textureData[index + 0] = 60;
                        textureData[index + 1] = 60; 
                        textureData[index + 2] = 60;
                    }
                }
                if (x >= SIZE-24 && y >= SIZE-30) { // Bottom-right corner (upside down)
                    int rankPattern = ((SIZE-x) / 3) + ((SIZE-y) / 3) * 8;
                    if (rankPattern % 13 == rank % 13 && rankPattern % 7 == 0) {
                        textureData[index + 0] = 60;
                        textureData[index + 1] = 60;
                        textureData[index + 2] = 60;
                    }
                }
                
                // Subtle background pattern
                if ((x + y) % 16 == 0 && x > 20 && x < SIZE-20 && y > 20 && y < SIZE-20) {
                    textureData[index + 0] = std::max(0, (int)textureData[index + 0] - 5);
                    textureData[index + 1] = std::max(0, (int)textureData[index + 1] - 5);  
                    textureData[index + 2] = std::max(0, (int)textureData[index + 2] - 5);
                }
            }
        }
    } else {
        // Create enhanced back card design
        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                int index = (y * SIZE + x) * 4;
                
                // Rich blue background with gradient
                int gradientFactor = (x + y) / 4;
                textureData[index + 0] = 40 + (gradientFactor % 30);   // R
                textureData[index + 1] = 60 + (gradientFactor % 40);   // G  
                textureData[index + 2] = 120 + (gradientFactor % 50);  // B
                textureData[index + 3] = 255; // A
                
                // Decorative border
                if (x < 6 || x >= SIZE-6 || y < 6 || y >= SIZE-6) {
                    textureData[index + 0] = 20;  // Darker border
                    textureData[index + 1] = 30;
                    textureData[index + 2] = 60;
                }
                
                // Diamond lattice pattern
                if (((x + y) % 16 < 2) || ((x - y + SIZE) % 16 < 2)) {
                    textureData[index + 0] += 30;
                    textureData[index + 1] += 40;
                    textureData[index + 2] += 30;
                }
                
                // Central ornamental design
                int centerX = SIZE / 2;
                int centerY = SIZE / 2;
                int dx = x - centerX;
                int dy = y - centerY;
                
                // Circular pattern in center
                int dist = dx * dx + dy * dy;
                if (dist < 1000 && dist > 400) {
                    if ((dx * dx + dy * dy) % 50 < 10) {
                        textureData[index + 0] = 150;
                        textureData[index + 1] = 180;
                        textureData[index + 2] = 200;
                    }
                }
                
                // Cross pattern
                if ((abs(dx) < 3 && abs(dy) < 30) || (abs(dy) < 3 && abs(dx) < 30)) {
                    textureData[index + 0] = 100;
                    textureData[index + 1] = 130;
                    textureData[index + 2] = 180;
                }
            }
        }
    }
    
    // Create and upload texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SIZE, SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData.data());
    
    return texture;
}