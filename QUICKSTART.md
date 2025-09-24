# Quick Start Guide - Memory Card Matching Game

## Current Status: ✅ Project Created, ⚠️ Needs Development Tools

## To Run This Project:

### Step 1: Install Development Environment

**Option A: MSYS2 (Recommended for Windows)**
1. Download MSYS2 from https://www.msys2.org/
2. Install MSYS2
3. Open MSYS2 terminal and run:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-glfw
   pacman -S mingw-w64-x86_64-glew  
   pacman -S mingw-w64-x86_64-glm
   pacman -S make
   ```
4. Add `C:\msys64\mingw64\bin` to your Windows PATH
5. Restart VS Code

**Option B: Visual Studio 2022**
1. Install Visual Studio 2022 with C++ workload
2. Install vcpkg package manager
3. Install dependencies via vcpkg:
   ```cmd
   vcpkg install glfw3:x64-windows
   vcpkg install glew:x64-windows
   vcpkg install glm:x64-windows
   ```

### Step 2: Build the Project

**Using VS Code Tasks:**
- Press `Ctrl+Shift+P` → "Tasks: Run Task" → "Build Memory Card Game"

**Using Terminal:**
```bash
make
```

**Using PowerShell Script:**
```powershell
powershell -ExecutionPolicy Bypass -File build.ps1
```

### Step 3: Run the Game

**Using VS Code:**
- Press `Ctrl+Shift+P` → "Tasks: Run Task" → "Run Memory Card Game"

**Using Terminal:**
```bash
./memory_card_game.exe
```

## Game Controls:
- **Left Click**: Flip cards
- **R**: Restart game  
- **ESC**: Exit

## Troubleshooting:

**"make: command not found"**
- Install MSYS2 and add to PATH

**"g++: command not found"** 
- Install MinGW-w64 via MSYS2

**Library linking errors**
- Ensure GLFW, GLEW, GLM are installed
- Check library paths in Makefile

## Project Files Created:
- ✅ Complete C++ source code (6 classes)
- ✅ OpenGL shaders (vertex + fragment)
- ✅ Build system (Makefile + PowerShell script)
- ✅ VS Code tasks configuration
- ✅ Comprehensive documentation

**Next**: Install development tools → Build → Run → Enjoy! 🎮