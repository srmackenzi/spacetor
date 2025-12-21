@echo off
g++ main.cpp -o programa.exe -I"C:\raylib\include" -L"C:\raylib\lib" -lraylib -lopengl32 -lgdi32 -lwinmm
pause
