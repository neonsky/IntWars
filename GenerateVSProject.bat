@echo off
set PATH=%PATH%;C:\MinGW\bin
echo Generate IntWars Makefiles
echo ------------------------------
IF EXIST .\vsbuild\ (
GOTO BUILDDIR
) ELSE (
GOTO MAIN
)

:BUILDDIR
echo Build directory exists, removing...
rmdir /s /q vsbuild
GOTO MAIN

:MAIN
echo Recreating build directory...
mkdir vsbuild
cd vsbuild
echo Creating Visual Studio 2013 Makefiles...
echo ------------------------------
cmake .. -G "Visual Studio 12 2013"
cd ..
pause