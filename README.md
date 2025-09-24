# Memory Card Matching Game

A computer graphics project implementing a memory card matching game using C++ and OpenGL with GLFW. This project demonstrates key computer graphics concepts including 2D transformations, animations, mouse picking, texture management, and game state machines.

## Features

- **Card Flipping Animation**: Smooth 3D rotation animations using matrix transformations
- **Mouse Picking**: Click detection to interact with cards using coordinate transformations
- **Game State Management**: Finite state machine controlling game flow and card interactions
- **Shuffle Algorithm**: Fisher-Yates shuffle implementation for randomizing card positions
- **OpenGL Rendering**: Modern OpenGL with shader programs for efficient rendering
- **Real-time Gameplay**: Delta-time based animations and input handling

## Technical Highlights

### Graphics Programming Concepts
- **Transformations**: Model, view, and projection matrices for 2D rendering
- **Animations**: Time-based interpolation for smooth card flipping
- **Shaders**: Vertex and fragment shaders for customizable rendering
- **Texture Mapping**: Dynamic texture generation and binding

### Algorithms Implemented
- **Fisher-Yates Shuffle**: O(n) algorithm for unbiased card randomization
- **State Machine**: Game state management with proper transition handling
- **Coordinate Transformations**: Screen-to-world coordinate conversion for mouse picking
- **Animation Interpolation**: Smooth transitions using delta time

## Project Structure

```
CG Project/
├── src/                    # Source files
│   ├── main.cpp           # Main application and game loop
│   ├── Card.cpp           # Card class implementation
│   ├── Deck.cpp           # Deck management and shuffling
│   ├── GameManager.cpp    # Game logic and state machine
│   ├── Renderer.cpp       # OpenGL rendering system
│   └── InputManager.cpp   # Input handling and mouse picking
├── include/                # Header files
│   ├── Card.h
│   ├── Deck.h
│   ├── GameManager.h
│   ├── Renderer.h
│   └── InputManager.h
├── shaders/                # GLSL shader files
│   ├── card_vertex.glsl   # Vertex shader for card rendering
│   └── card_fragment.glsl # Fragment shader with border effects
├── assets/                 # Game assets (textures, fonts)
├── build/                  # Build output directory
├── Makefile               # Build configuration
└── README.md              # This file
```

## Dependencies

- **GLFW 3.x**: Window management and input handling
- **GLEW**: OpenGL extension loading
- **GLM**: Mathematics library for graphics
- **C++17**: Modern C++ standard

## Build Instructions

### Windows (MSYS2/MinGW)

1. Install MSYS2 and dependencies:
```bash
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-glfw
pacman -S mingw-w64-x86_64-glew
pacman -S mingw-w64-x86_64-glm
```

2. Build the project:
```bash
make
```

3. Run the game:
```bash
make run
```

### Linux (Ubuntu/Debian)

1. Install dependencies:
```bash
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install libglfw3-dev libglew-dev libglm-dev
```

2. Update Makefile to use Linux libraries (uncomment Linux LIBS line)

3. Build and run:
```bash
make
make run
```

### macOS

1. Install dependencies using Homebrew:
```bash
brew install glfw glew glm
```

2. Update Makefile to use macOS libraries (uncomment macOS LIBS line)

3. Build and run:
```bash
make
make run
```

## Gameplay Instructions

1. **Objective**: Match all pairs of cards to win the game
2. **Controls**:
   - **Left Click**: Flip a card
   - **R Key**: Restart the game
   - **ESC Key**: Exit the game

3. **Rules**:
   - Click on face-down cards to reveal them
   - Only two cards can be revealed at a time
   - If cards match, they stay face-up
   - If cards don't match, they flip back face-down
   - Game ends when all pairs are matched

## Technical Implementation Details

### Card Flipping Animation

The card flip animation uses Y-axis rotation with the following approach:

```cpp
// Update flip animation
flipProgress += deltaTime * flipSpeed;
angleY = flipProgress;

// At 90 degrees, swap texture (front/back)
if (angleY >= 90.0f && !textureSwapped) {
    swapTexture();
    textureSwapped = true;
}

// Complete flip at 180 degrees
if (angleY >= 180.0f) {
    completeFlip();
}
```

### Mouse Picking Algorithm

Screen-to-world coordinate conversion:

```cpp
glm::vec2 screenToWorld(glm::vec2 screenPos, int width, int height) {
    float worldX = screenPos.x - width / 2.0f;
    float worldY = height / 2.0f - screenPos.y;
    return glm::vec2(worldX, worldY);
}
```

### State Machine

The game uses a finite state machine with the following states:

- **IDLE**: Waiting for user input
- **FLIPPING_FIRST**: First card is flipping
- **FLIPPING_SECOND**: Second card is flipping
- **CHECK_MATCH**: Checking if cards match
- **RESOLVE**: Resolving match/no-match
- **WIN**: All pairs matched

### Shuffle Algorithm

Fisher-Yates shuffle implementation for unbiased randomization:

```cpp
void Deck::shuffle() {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    for (int i = cards.size() - 1; i > 0; i--) {
        std::uniform_int_distribution<> dis(0, i);
        int j = dis(gen);
        std::swap(cards[i], cards[j]);
    }
}
```

## Code Architecture

### Class Hierarchy

- **Card**: Represents individual playing cards with state and animation
- **Deck**: Manages collection of cards, shuffling, and layout
- **GameManager**: Controls game logic, state machine, and win conditions
- **Renderer**: Handles OpenGL rendering, shaders, and textures
- **InputManager**: Processes user input and coordinate transformations

### Design Patterns Used

- **State Pattern**: Game state management
- **Observer Pattern**: Input event handling
- **Factory Pattern**: Texture and shader creation
- **RAII**: Resource management for OpenGL objects

## Performance Considerations

- **Batch Rendering**: Minimizes OpenGL state changes
- **Delta Time**: Frame-rate independent animations
- **Memory Management**: Proper cleanup of OpenGL resources
- **Efficient Algorithms**: O(n) shuffle, O(1) card lookup

## Future Enhancements

- **3D Perspective**: Add depth and perspective projection
- **Advanced Shaders**: Lighting, shadows, and post-processing effects
- **Sound Effects**: Audio feedback for interactions
- **Difficulty Levels**: Different grid sizes and card counts
- **High Score System**: Persistent score tracking
- **Particle Effects**: Visual effects for matches and wins

## Development Notes

### Common Issues and Solutions

1. **Linking Errors**: Ensure all required libraries are installed and properly linked
2. **Shader Compilation**: Check GLSL version compatibility
3. **Texture Loading**: Verify asset file paths and formats
4. **Input Lag**: Use proper delta time for smooth animations

### Debugging Tips

1. Enable OpenGL debug output for detailed error messages
2. Use console output for game state debugging
3. Verify matrix calculations with simple test cases
4. Check coordinate system consistency

## Academic Context

This project demonstrates understanding of:

- **Computer Graphics Pipeline**: Vertex processing, rasterization, fragment shading
- **Linear Algebra**: Matrix transformations, coordinate systems
- **Game Programming**: State machines, input handling, animation systems
- **Software Engineering**: Object-oriented design, resource management

## Contributing

This is an academic project. For improvements or extensions:

1. Fork the repository
2. Create a feature branch
3. Implement changes with proper documentation
4. Test thoroughly on target platforms
5. Submit pull request with detailed description

## License

This project is created for educational purposes. Feel free to use and modify for learning and academic projects.

## Acknowledgments

- OpenGL community for excellent documentation and examples
- GLFW and GLEW developers for essential graphics libraries
- Computer graphics textbooks and online resources
- Academic instructors and teaching assistants for guidance

---

**Author**: [Your Name]  
**Course**: Computer Graphics  
**Date**: [Current Date]  
**Version**: 1.0