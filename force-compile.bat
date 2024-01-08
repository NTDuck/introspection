@echo off
@REM for %%f in (.\build\*.exe) do (del "%%f")
mingw32-make clean
call compile.bat