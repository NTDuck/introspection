@echo off
for %%f in (.\build\*.exe) do (del "%%f")
@REM mingw32-make clean
call compile.bat