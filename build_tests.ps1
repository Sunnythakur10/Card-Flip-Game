Write-Host "Building deck tests..."
# Locate MSYS2 like build_portable
$possiblePaths = @(
    "$Env:USERPROFILE\\msys64",
    "C:\\msys64"
)
$msysPath = $null
foreach ($p in $possiblePaths) { if (Test-Path $p) { $msysPath = $p; break } }
if (-not $msysPath) { Write-Error "MSYS2 not found"; exit 1 }
$env:PATH = "$msysPath\\mingw64\\bin;$env:PATH"
$srcDir = "src"
$includes = "-Iinclude"
$cppFiles = Get-ChildItem $srcDir -Filter *.cpp | Where-Object { $_.Name -ne 'main.cpp' }
$objects = @()
foreach ($f in $cppFiles) {
  $obj = "build/test_$($f.BaseName).o"
  if (-not (Test-Path build)) { New-Item -ItemType Directory build | Out-Null }
  Write-Host "Compiling $($f.Name)..."
  g++ -std=c++17 -O2 -Wall -Wextra $includes -DRUN_TESTS_MAIN -c $f.FullName -o $obj
  if ($LASTEXITCODE -ne 0) { exit 1 }
  $objects += $obj
}
Write-Host "Linking deck_tests.exe"
g++ $objects -o deck_tests.exe -lglfw3 -lopengl32 -lglew32 -lgdi32
if ($LASTEXITCODE -ne 0) { exit 1 }
Write-Host "Running tests..."
./deck_tests.exe
