@echo off
echo Compiling C program...
gcc -I./include -Wall -o program.exe main.c src/login.c
if %errorlevel% equ 0 (
    echo Compilation successful!
    echo Running program...
    program.exe
) else (
    echo Compilation failed! 
    exit \b %errorlevel%
)
pause