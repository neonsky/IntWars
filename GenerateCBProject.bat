@echo off
echo Generate IntWars Makefiles
echo ------------------------------
IF EXIST cbbuild GOTO BUILDDIR
ELSE GOTO MAIN
 
:BUILDDIR
echo Build directory exists, removing...
rmdir /s /q cbbuild
GOTO MAIN
 
:MAIN
echo Recreating build directory...
mkdir cbbuild
cd cbbuild
echo Creating MinGW Makefiles...
echo ------------------------------
cmake .. -G "CodeBlocks - MinGW Makefiles"
cd ..
pause