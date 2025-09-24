# Build script for Windows PowerShell
# Memory Card Matching Game

Write-Host "Memory Card Matching Game - Build Script" -ForegroundColor Green
Write-Host "=======================================" -ForegroundColor Green

# Check if required tools are available
$gppAvailable = Get-Command g++ -ErrorAction SilentlyContinue
$clAvailable = Get-Command cl -ErrorAction SilentlyContinue

if (-not $gppAvailable -and -not $clAvailable) {
    Write-Host "ERROR: No C++ compiler found!" -ForegroundColor Red
    Write-Host "Please install one of the following:" -ForegroundColor Yellow
    Write-Host "1. Visual Studio 2019/2022 with C++ workload" -ForegroundColor White
    Write-Host "2. MSYS2 with MinGW-w64" -ForegroundColor White
    Write-Host "3. Visual Studio Build Tools" -ForegroundColor White
    Write-Host ""
    Write-Host "For MSYS2 installation:" -ForegroundColor Yellow
    Write-Host "1. Download and install MSYS2 from https://www.msys2.org/" -ForegroundColor White
    Write-Host "2. Run these commands in MSYS2 terminal:" -ForegroundColor White
    Write-Host "   pacman -S mingw-w64-x86_64-gcc" -ForegroundColor Cyan
    Write-Host "   pacman -S mingw-w64-x86_64-glfw" -ForegroundColor Cyan
    Write-Host "   pacman -S mingw-w64-x86_64-glew" -ForegroundColor Cyan
    Write-Host "   pacman -S mingw-w64-x86_64-glm" -ForegroundColor Cyan
    Write-Host "   pacman -S make" -ForegroundColor Cyan
    Write-Host "3. Add C:\msys64\mingw64\bin to your PATH" -ForegroundColor White
    exit 1
}

# Create build directory
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Name "build"
    Write-Host "Created build directory" -ForegroundColor Green
}

# Compiler settings
$cxxFlags = "-std=c++17 -Wall -Wextra -O2"
$includes = "-Iinclude"
$libs = "-lglfw3 -lopengl32 -lglew32 -lgdi32"

# Source files
$sources = Get-ChildItem -Path "src" -Filter "*.cpp" | ForEach-Object { $_.FullName }
$objects = @()

Write-Host "Compiling source files..." -ForegroundColor Yellow

# Compile each source file
foreach ($source in $sources) {
    $basename = [System.IO.Path]::GetFileNameWithoutExtension($source)
    $object = "build\$basename.o"
    $objects += $object
    
    Write-Host "Compiling $basename.cpp..." -ForegroundColor White
    
    if ($gppAvailable) {
        & g++ $cxxFlags.Split() $includes.Split() -c $source -o $object
    } else {
        Write-Host "Using Visual C++ compiler not implemented in this script" -ForegroundColor Red
        exit 1
    }
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Compilation failed for $basename.cpp" -ForegroundColor Red
        exit 1
    }
}

# Link the executable
Write-Host "Linking executable..." -ForegroundColor Yellow
$objectsString = $objects -join " "

if ($gppAvailable) {
    & g++ $objects -o "memory_card_game.exe" $libs.Split()
} else {
    Write-Host "Using Visual C++ linker not implemented in this script" -ForegroundColor Red
    exit 1
}

if ($LASTEXITCODE -ne 0) {
    Write-Host "Linking failed!" -ForegroundColor Red
    exit 1
}

Write-Host "Build successful! Created memory_card_game.exe" -ForegroundColor Green
Write-Host "Run with: .\memory_card_game.exe" -ForegroundColor Yellow