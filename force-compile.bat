@echo off
for %%f in (.\build\*.exe) do (del "%%f")

call compile.bat