@echo off
mingw32-make -j %NUMBER_OF_PROCESSORS% -f Makefile
@REM `-j$(nproc)` on Linux & macOS

if errorlevel 1 (
    echo "error: compilation terminated."
    pause
    exit /b 1
)

.\build\8964.exe