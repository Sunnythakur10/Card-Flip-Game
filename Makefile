# Makefile for Memory Card Matching Game

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TARGET = memory_card_game

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Include paths
INCLUDES = -I$(INCLUDE_DIR)

# Library settings (adjust paths as needed for your system)
# For Windows with MSYS2/MinGW:
LIBS = -lglfw3 -lopengl32 -lglew32 -lgdi32

# For Linux:
# LIBS = -lglfw -lGL -lGLEW -lm

# For macOS:
# LIBS = -lglfw -framework OpenGL -lGLEW

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build target
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Rebuild everything
rebuild: clean all

# Run the game
run: $(TARGET)
	./$(TARGET)

# Install dependencies (for different systems)
install-deps-windows:
	@echo "Please install GLFW, GLEW, and GLM using MSYS2 or vcpkg"
	@echo "MSYS2: pacman -S mingw-w64-x86_64-glfw mingw-w64-x86_64-glew mingw-w64-x86_64-glm"

install-deps-ubuntu:
	sudo apt-get update
	sudo apt-get install libglfw3-dev libglew-dev libglm-dev

install-deps-macos:
	brew install glfw glew glm

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build the game (default)"
	@echo "  clean        - Remove build files"
	@echo "  rebuild      - Clean and build"
	@echo "  run          - Build and run the game"
	@echo "  install-deps - Install dependencies (platform-specific)"
	@echo "  help         - Show this help"

# Mark targets as phony
.PHONY: all clean rebuild run install-deps-windows install-deps-ubuntu install-deps-macos help

# Dependencies
$(BUILD_DIR)/main.o: $(INCLUDE_DIR)/GameManager.h $(INCLUDE_DIR)/Renderer.h $(INCLUDE_DIR)/InputManager.h
$(BUILD_DIR)/Card.o: $(INCLUDE_DIR)/Card.h $(INCLUDE_DIR)/Renderer.h
$(BUILD_DIR)/Deck.o: $(INCLUDE_DIR)/Deck.h $(INCLUDE_DIR)/Card.h
$(BUILD_DIR)/GameManager.o: $(INCLUDE_DIR)/GameManager.h $(INCLUDE_DIR)/Deck.h $(INCLUDE_DIR)/Card.h
$(BUILD_DIR)/Renderer.o: $(INCLUDE_DIR)/Renderer.h
$(BUILD_DIR)/InputManager.o: $(INCLUDE_DIR)/InputManager.h