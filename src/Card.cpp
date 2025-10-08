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
    const int SIZE = 128; // 128x128 for detailed patterns
    std::vector<unsigned char> textureData(SIZE * SIZE * 4); // RGBA
    
    if (isFront) {
        // Create unique pattern based on card's unique ID ensuring each rank-suit combo is unique
        int uniquePatternId = rank + suit * 13; // Creates unique ID: suits 0-3, ranks 1-13
        
        // Base colors for different pattern families
        struct PatternColor {
            unsigned char r, g, b;
        } patternColors[] = {
            {220, 50, 50},   // Red
            {50, 220, 50},   // Green  
            {50, 50, 220},   // Blue
            {220, 220, 50},  // Yellow
            {220, 50, 220},  // Magenta
            {50, 220, 220},  // Cyan
            {255, 140, 0},   // Orange
            {150, 75, 200},  // Purple
            {255, 100, 150}, // Pink
            {100, 255, 100}, // Light Green
            {100, 100, 255}, // Light Blue
            {255, 200, 100}, // Light Orange
            {200, 100, 255}, // Light Purple
            {100, 255, 200}, // Mint
            {255, 255, 100}, // Light Yellow
            {200, 200, 200}  // Light Gray
        };
        
        PatternColor mainColor = patternColors[uniquePatternId % 16];
        
        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                int index = (y * SIZE + x) * 4;
                
                // White background
                textureData[index + 0] = 250; // R
                textureData[index + 1] = 250; // G
                textureData[index + 2] = 250; // B
                textureData[index + 3] = 255; // A
                
                // Black border
                if (x < 3 || x >= SIZE-3 || y < 3 || y >= SIZE-3) {
                    textureData[index + 0] = 20;
                    textureData[index + 1] = 20;
                    textureData[index + 2] = 20;
                }
                // Gold inner border
                else if (x < 6 || x >= SIZE-6 || y < 6 || y >= SIZE-6) {
                    textureData[index + 0] = 200;
                    textureData[index + 1] = 170;
                    textureData[index + 2] = 100;
                }
                else {
                    // Create unique patterns based on the pattern ID
                    int centerX = SIZE / 2;
                    int centerY = SIZE / 2;
                    int dx = x - centerX;
                    int dy = y - centerY;
                    int dist = dx * dx + dy * dy;
                    
                    bool drawPattern = false;
                    
                    // Use a larger variety of patterns to ensure uniqueness for 52 different cards
                    switch (uniquePatternId % 16) {
                        case 0: // Concentric circles
                            drawPattern = (dist % 400 < 50) && (dist > 100) && (dist < 2500);
                            break;
                        case 1: // Diamond pattern
                            drawPattern = ((abs(dx) + abs(dy)) % 30 < 5) && (abs(dx) + abs(dy) < 50);
                            break;
                        case 2: // Cross pattern
                            drawPattern = (abs(dx) < 8 && abs(dy) < 40) || (abs(dy) < 8 && abs(dx) < 40);
                            break;
                        case 3: // Star pattern
                            drawPattern = (abs(dx) < 3 && abs(dy) < 35) || 
                                         (abs(dy) < 3 && abs(dx) < 35) ||
                                         (abs(dx - dy) < 3 && abs(dx) < 25) ||
                                         (abs(dx + dy) < 3 && abs(dx) < 25);
                            break;
                        case 4: // Spiral pattern
                            {
                                float angle = atan2(dy, dx);
                                float radius = sqrt(dist);
                                drawPattern = (int(angle * 3 + radius * 0.1) % 10) < 2 && radius < 45;
                            }
                            break;
                        case 5: // Triangle pattern
                            drawPattern = (dy > 0) && (abs(dx) < (50 - dy)) && (dy < 45);
                            break;
                        case 6: // Hexagon pattern
                            {
                                float hexDist = abs(dx) + abs(dy * 0.866f + dx * 0.5f) + abs(dy * 0.866f - dx * 0.5f);
                                drawPattern = (hexDist > 30 && hexDist < 40) || (hexDist > 15 && hexDist < 20);
                            }
                            break;
                        case 7: // Flower pattern
                            {
                                float angle = atan2(dy, dx);
                                float radius = sqrt(dist);
                                float petalRadius = 25 + 10 * cos(angle * 6);
                                drawPattern = (radius > petalRadius - 3 && radius < petalRadius + 3) && radius < 45;
                            }
                            break;
                        case 8: // Grid pattern
                            drawPattern = ((dx % 15 < 3) || (dy % 15 < 3)) && (abs(dx) < 40 && abs(dy) < 40);
                            break;
                        case 9: // Waves pattern
                            {
                                float wave = sin(dx * 0.2f) * 8;
                                drawPattern = (abs(dy - wave) < 4) && (abs(dx) < 45);
                            }
                            break;
                        case 10: // Checkerboard
                            drawPattern = ((dx/10 + dy/10) % 2 == 0) && (abs(dx) < 40 && abs(dy) < 40);
                            break;
                        case 11: // Concentric squares
                            {
                                int maxDist = std::max(abs(dx), abs(dy));
                                drawPattern = (maxDist % 12 < 3) && (maxDist > 10) && (maxDist < 45);
                            }
                            break;
                        case 12: // Plus signs
                            drawPattern = ((abs(dx) % 20 < 3 && abs(dy) % 20 < 12) || 
                                         (abs(dy) % 20 < 3 && abs(dx) % 20 < 12)) && 
                                         (abs(dx) < 45 && abs(dy) < 45);
                            break;
                        case 13: // Zigzag pattern
                            {
                                int zigzag = (dx + dy) % 30;
                                drawPattern = (zigzag < 6 || zigzag > 24) && (abs(dx) < 45 && abs(dy) < 45);
                            }
                            break;
                        case 14: // Circular dots
                            {
                                int dotX = ((dx + 60) / 20) * 20 - 60;
                                int dotY = ((dy + 60) / 20) * 20 - 60;
                                int dotDist = (dx - dotX) * (dx - dotX) + (dy - dotY) * (dy - dotY);
                                drawPattern = (dotDist < 25) && (abs(dx) < 45 && abs(dy) < 45);
                            }
                            break;
                        case 15: // Diagonal stripes
                            drawPattern = ((dx - dy) % 15 < 4) && (abs(dx) < 45 && abs(dy) < 45);
                            break;
                    }
                    
                    if (drawPattern) {
                        textureData[index + 0] = mainColor.r;
                        textureData[index + 1] = mainColor.g;
                        textureData[index + 2] = mainColor.b;
                    }
                    
                    // Add rank and suit specific decorative elements for uniqueness
                    if (rank <= 4) {
                        // Small dots in corners for low ranks
                        if (((x-15)*(x-15) + (y-15)*(y-15) < 16) ||
                            ((x-(SIZE-15))*(x-(SIZE-15)) + (y-15)*(y-15) < 16) ||
                            ((x-15)*(x-15) + (y-(SIZE-15))*(y-(SIZE-15)) < 16) ||
                            ((x-(SIZE-15))*(x-(SIZE-15)) + (y-(SIZE-15))*(y-(SIZE-15)) < 16)) {
                            textureData[index + 0] = mainColor.r / 2;
                            textureData[index + 1] = mainColor.g / 2;
                            textureData[index + 2] = mainColor.b / 2;
                        }
                    } else if (rank <= 8) {
                        // Lines in corners for middle ranks
                        if ((x < 20 && (y < 20 || y > SIZE-20)) || 
                            (x > SIZE-20 && (y < 20 || y > SIZE-20))) {
                            if ((x + y) % 4 < 2) {
                                textureData[index + 0] = mainColor.r / 3;
                                textureData[index + 1] = mainColor.g / 3;
                                textureData[index + 2] = mainColor.b / 3;
                            }
                        }
                    } else {
                        // Squares in corners for high ranks
                        if ((x < 18 && y < 18) || (x > SIZE-18 && y < 18) ||
                            (x < 18 && y > SIZE-18) || (x > SIZE-18 && y > SIZE-18)) {
                            textureData[index + 0] = mainColor.r / 4;
                            textureData[index + 1] = mainColor.g / 4;
                            textureData[index + 2] = mainColor.b / 4;
                        }
                    }
                }
                
                // Rank and suit indicators in corners for better identification
                if ((x < 25 && y < 30)) {
                    // Top-left corner: rank indicator
                    int rankPattern = (x/3 + y/3) % 13;
                    if (rankPattern == ((rank - 1) % 13)) {
                        textureData[index + 0] = 20;
                        textureData[index + 1] = 20;
                        textureData[index + 2] = 20;
                    }
                } else if ((x >= SIZE-25 && y >= SIZE-30)) {
                    // Bottom-right corner: suit indicator
                    int suitPattern = (x/4 + y/4) % 4;
                    if (suitPattern == suit) {
                        textureData[index + 0] = 60;
                        textureData[index + 1] = 60;
                        textureData[index + 2] = 60;
                    }
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