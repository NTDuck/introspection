@echo off
del .\build\8964.exe
mingw32-make -f MakeFile

if errorlevel 1 (
    echo "error: compilation terminated."
    pause
    exit /b 1
)

.\build\8964.exe