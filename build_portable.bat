@echo off
echo Memory Card Matching Game - Portable Build Script
echo =================================================

REM Try different MSYS2 installation paths
set MSYS2_PATHS=C:\msys64;D:\msys64;C:\tools\msys64;%USERPROFILE%\msys64

for %%P in (%MSYS2_PATHS%) do (
    if exist "%%P\mingw64\bin\g++.exe" (
        echo Found MSYS2 at: %%P
        set MSYS2_PATH=%%P
        goto :found_msys2
    )
)

echo ERROR: MSYS2 not found in common locations!
echo Please install MSYS2 from https://www.msys2.org/
echo Or specify the correct path manually.
pause
exit /b 1

:found_msys2
echo Using MSYS2 from: %MSYS2_PATH%

REM Temporarily add MSYS2 to PATH for this session
set "PATH=%MSYS2_PATH%\mingw64\bin;%PATH%"

REM Test if compiler is now available
g++ --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Compiler still not accessible
    pause
    exit /b 1
)

echo Compiler found, starting build...

REM Create build directory
if not exist build mkdir build

REM Compile source files
echo Compiling source files...
for %%f in (src\*.cpp) do (
    echo Compiling %%f...
    g++ -std=c++17 -Wall -Wextra -O2 -Iinclude -c "%%f" -o "build\%%~nf.o"
    if errorlevel 1 (
        echo Compilation failed for %%f
        pause
        exit /b 1
    )
)

REM Link executable
echo Linking executable...
g++ build\*.o -o memory_card_game.exe -lglfw3 -lopengl32 -lglew32 -lgdi32
if errorlevel 1 (
    echo Linking failed!
    pause
    exit /b 1
)

echo Build successful! Created memory_card_game.exe
echo Run with: memory_card_game.exe
pause