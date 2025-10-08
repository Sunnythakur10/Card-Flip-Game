# Memory Card Matching Game - Portable PowerShell Build Script
Write-Host "Memory Card Matching Game - Portable Build Script" -ForegroundColor Green
Write-Host "=================================================" -ForegroundColor Green

# Common MSYS2 installation paths
$msys2Paths = @(
    "C:\msys64",
    "D:\msys64", 
    "C:\tools\msys64",
    "$env:USERPROFILE\msys64",
    "C:\msys32"
)

$msys2Found = $false
$msys2Path = ""

# Search for MSYS2 installation
foreach ($path in $msys2Paths) {
    $gppPath = Join-Path $path "mingw64\bin\g++.exe"
    if (Test-Path $gppPath) {
        Write-Host "Found MSYS2 at: $path" -ForegroundColor Green
        $msys2Path = $path
        $msys2Found = $true
        break
    }
}

if (-not $msys2Found) {
    Write-Host "ERROR: MSYS2 not found in common locations!" -ForegroundColor Red
    Write-Host "Searched in:" -ForegroundColor Yellow
    foreach ($path in $msys2Paths) {
        Write-Host "  - $path" -ForegroundColor White
    }
    Write-Host ""
    Write-Host "Please install MSYS2 from https://www.msys2.org/" -ForegroundColor Yellow
    Write-Host "Or run this script from an MSYS2 terminal." -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

# Add MSYS2 to PATH for this session
$mingwBin = Join-Path $msys2Path "mingw64\bin"
$env:PATH = "$mingwBin;$env:PATH"

Write-Host "Using MSYS2 from: $msys2Path" -ForegroundColor Cyan

# Test compiler
try {
    $gppVersion = & "$mingwBin\g++.exe" --version 2>&1
    Write-Host "Compiler found:" -ForegroundColor Green
    Write-Host ($gppVersion | Select-Object -First 1) -ForegroundColor White
} catch {
    Write-Host "ERROR: Compiler not accessible" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Create build directory
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Name "build" | Out-Null
    Write-Host "Created build directory" -ForegroundColor Green
}

# Compiler settings
$cxxFlags = @("-std=c++17", "-Wall", "-Wextra", "-O2")
$includes = @("-Iinclude")
$libs = @("-lglfw3", "-lopengl32", "-lglew32", "-lgdi32")

# Get source files
$sources = Get-ChildItem -Path "src" -Filter "*.cpp"
$objects = @()

Write-Host "Compiling source files..." -ForegroundColor Yellow

# Compile each source file
foreach ($source in $sources) {
    $basename = $source.BaseName
    $object = "build\$basename.o"
    $objects += $object
    
    Write-Host "  Compiling $($source.Name)..." -ForegroundColor White
    
    $compileArgs = $cxxFlags + $includes + @("-c", $source.FullName, "-o", $object)
    
    try {
        & "$mingwBin\g++.exe" $compileArgs
        if ($LASTEXITCODE -ne 0) {
            throw "Compilation failed"
        }
    } catch {
        Write-Host "ERROR: Compilation failed for $($source.Name)" -ForegroundColor Red
        Read-Host "Press Enter to exit"
        exit 1
    }
}

# Link the executable
Write-Host "Linking executable..." -ForegroundColor Yellow

$linkArgs = $objects + @("-o", "memory_card_game.exe") + $libs

try {
    & "$mingwBin\g++.exe" $linkArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Linking failed"
    }
} catch {
    Write-Host "ERROR: Linking failed!" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "Build successful! Created memory_card_game.exe" -ForegroundColor Green
Write-Host "Run with: .\memory_card_game.exe" -ForegroundColor Yellow
Write-Host ""
Read-Host "Press Enter to continue"