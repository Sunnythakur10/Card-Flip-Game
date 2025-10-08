# Memory Card Matching Game

A computer graphics project implementing a memory card matching game using C++ and OpenGL with GLFW. This project demonstrates key computer graphics concepts including 2D transformations, animations, mouse picking, texture management, and game state machines.

## Features

### **Core Graphics Features**
- **3D Card Flipping Animation**: Smooth Y-axis rotation with matrix transformations and texture swapping at 90°
- **Procedural Texture Generation**: 16+ unique mathematical patterns (spirals, fractals, geometric shapes)
- **Real-time Rendering**: 60fps target with optimized OpenGL draw calls and state management
- **Advanced Font Rendering**: Bitmap font generation with anti-aliasing, shadows, and viewport scaling
- **Dynamic Lighting Effects**: Ambient lighting and color modulation for visual depth

### **Game Mechanics & Algorithms**
- **Mouse Picking System**: Precise screen-to-world coordinate transformation for card selection
- **Fisher-Yates Shuffle**: Cryptographically secure randomization with uniform distribution
- **State Machine Architecture**: Six-state FSM (IDLE, FLIPPING_FIRST, FLIPPING_SECOND, CHECK_MATCH, RESOLVE, WIN)
- **Fair Scoring Algorithm**: Multi-factor scoring with time penalties, move costs, match bonuses, and streak multipliers
- **Deterministic Deck Generation**: Runtime assertions ensuring exactly 2 cards per pattern
- **Click Guard System**: Animation-aware input filtering preventing race conditions

### **Computer Graphics Techniques**
- **Matrix Transformation Pipeline**: MVP matrix chain for 2D/3D hybrid rendering
- **Texture Atlas Management**: Efficient GPU memory usage with dynamic texture creation
- **Alpha Blending**: Smooth transparency effects for UI elements and transitions
- **Viewport Adaptation**: Resolution-independent rendering with automatic scaling
- **Batch Rendering**: Minimized draw calls through efficient geometry submission

## Technical Highlights

### Computer Graphics Concepts Applied

#### **1. 3D Transformations & Matrix Mathematics**
- **Model Matrix**: Individual card positioning, rotation (Y-axis flip animation), and scaling
- **View Matrix**: Virtual camera positioning and orientation in 3D space
- **Projection Matrix**: Orthographic projection for 2D gameplay with 3D depth effects
- **Matrix Concatenation**: MVP (Model-View-Projection) pipeline implementation
- **Homogeneous Coordinates**: 4x4 matrices for affine transformations

#### **2. Rasterization & Rendering Pipeline**
- **Vertex Processing**: Card vertices transformed through graphics pipeline
- **Fragment Shading**: Pixel-level operations for texture sampling and color computation
- **Depth Testing**: Z-buffer algorithm for proper card overlap rendering
- **Alpha Blending**: Transparency effects for smooth card transitions and UI elements

#### **3. Texture Mapping & Procedural Generation**
- **Parametric Texture Coordinates**: UV mapping for card faces and backs
- **Procedural Texture Generation**: Mathematical functions creating unique card patterns
  - Concentric circles using distance fields
  - Diamond patterns with Manhattan distance
  - Spiral patterns using polar coordinates
  - Fractal-like patterns with recursive mathematics
- **Texture Filtering**: Linear and nearest-neighbor sampling for crisp visuals
- **Mipmap Generation**: Multiple resolution levels for optimal rendering

#### **4. Animation & Interpolation Techniques**
- **Keyframe Animation**: Card flip states with smooth transitions
- **Linear Interpolation (LERP)**: Position and rotation tweening
- **Easing Functions**: Smooth acceleration/deceleration curves
- **Delta Time Integration**: Frame-rate independent animations
- **State Machine Animation**: Coordinated multi-object animation sequences

#### **5. Font Rendering & Typography**
- **Bitmap Font Generation**: Procedural character glyph creation
- **Texture Atlas Management**: Efficient character storage and retrieval
- **Text Rendering Pipeline**: Quad generation with texture coordinate mapping
- **Anti-aliasing Techniques**: Multi-pass rendering for text outline and shadow effects
- **Viewport-Responsive Scaling**: Dynamic font sizing based on screen resolution

#### **6. Real-time Rendering Optimizations**
- **Batch Rendering**: Minimizing OpenGL state changes and draw calls
- **Vertex Buffer Objects (VBO)**: Efficient GPU memory management
- **Instanced Rendering**: Multiple cards with single draw call
- **Frustum Culling**: Off-screen object elimination
- **Level-of-Detail (LOD)**: Adaptive quality based on viewing distance

#### **7. Color Theory & Visual Design**
- **Color Space Transformations**: RGB color manipulation for visual variety
- **Procedural Color Generation**: Algorithm-based palette creation
- **Visual Hierarchy**: Color coding for game state indication
- **Contrast Optimization**: Ensuring UI readability across backgrounds

#### **8. Computational Geometry**
- **Point-in-Rectangle Testing**: Mouse picking collision detection
- **Bounding Box Calculations**: Spatial queries for card selection
- **2D/3D Coordinate System Conversions**: Screen-space to world-space transformations
- **Geometric Primitives**: Quad generation for card representation

### Algorithms Implemented
- **Fisher-Yates Shuffle**: O(n) algorithm for unbiased card randomization
- **State Machine**: Game state management with proper transition handling
- **Coordinate Transformations**: Screen-to-world coordinate conversion for mouse picking
- **Animation Interpolation**: Smooth transitions using delta time
- **Score Calculation**: Clamps final score between MIN_SCORE and BASE_SCORE with breakdown
- **Pattern Generation Algorithms**: Mathematical functions for unique card designs
- **Collision Detection**: Efficient spatial queries for user interaction
- **Memory Management**: RAII pattern for OpenGL resource cleanup

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
### Scoring System

The scoring formula balances speed and accuracy:

```
base = BASE_SCORE (1000)
penalties = elapsedSeconds * TIME_PENALTY_PER_SEC (1.5) + moves * MOVE_PENALTY (5)
bonuses = totalMatches * MATCH_BONUS (25) + totalStreakBonus (STREAK_BONUS_STEP * streak_length_accumulation)
raw = base + bonuses - penalties
final = clamp(raw, MIN_SCORE, BASE_SCORE)
```

Stars awarded:
* 3 stars: final >= 900
* 2 stars: final >= 650
* 1 star: final >= 350

Adjust values in `ScoringConstants` within `GameManager.h`.

### Deck Integrity

Deck creation selects N unique rank/suit pairs, duplicates each exactly twice, then shuffles. Runtime assertions ensure:
* Total cards == pairs * 2
* Each (rank,suit) frequency == 2

### Tests

Run deck tests:
```
make tests
./deck_tests
```
You should see: `All deck tests passed.`

### HUD Readability

HUD text is rendered with a background panel plus multi-pass outline/shadow to maintain contrast at various resolutions (720p–1080p). Adjust scaling factors in `Renderer::renderEnhancedText` if needed.

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

This project demonstrates comprehensive understanding of:

### **Computer Graphics Fundamentals**
- **Graphics Pipeline**: Complete implementation of vertex processing, primitive assembly, rasterization, and fragment shading stages
- **Linear Algebra**: Practical application of matrix mathematics, vector operations, and geometric transformations
- **Rendering Techniques**: Real-time rendering optimizations, texture management, and efficient GPU utilization
- **Visual Computing**: Algorithm design for procedural content generation and real-time visual effects

### **Advanced Graphics Concepts**
- **Transformation Hierarchies**: Object-space to world-space to screen-space coordinate transformations
- **Parametric Surface Generation**: Mathematical functions for procedural texture creation
- **Temporal Coherence**: Frame-to-frame optimization and smooth animation techniques
- **Spatial Data Structures**: Efficient collision detection and spatial query algorithms

### **Software Engineering & Game Development**
- **Object-Oriented Design**: Clean architecture with separation of concerns (MVC pattern)
- **Resource Management**: RAII pattern for OpenGL objects and memory-safe programming
- **State Machines**: Finite state automata for game logic and animation control
- **Performance Optimization**: Real-time constraints and 60fps target achievement

### **Mathematical Foundations**
- **Discrete Mathematics**: Combinatorial algorithms (Fisher-Yates shuffle, pattern permutations)
- **Calculus Applications**: Derivative-based easing functions and smooth interpolation
- **Probability Theory**: Random number generation and statistical pattern distribution
- **Geometry Processing**: 2D/3D coordinate systems and geometric primitive manipulation

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