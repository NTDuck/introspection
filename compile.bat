@echo off
mingw32-make -f Makefile

if errorlevel 1 (
    echo "error: compilation terminated."
    pause
    exit /b 1
)

.\build\8964.exe