@echo off
set PATH=%PATH%;C:\MinGW\bin
echo Generate IntWars Makefiles
echo ------------------------------
IF EXIST .\codeblocksbuild\ (
GOTO BUILDDIR
) ELSE (
GOTO MAIN
)

:BUILDDIR
echo Build directory exists, removing...
rmdir /s /q codeblocksbuild
GOTO MAIN

:MAIN
echo Recreating build directory...
mkdir codeblocksbuild
cd codeblocksbuild
echo Creating CodeBlocks MinGW Makefiles...
echo ------------------------------
cmake .. -G "CodeBlocks - MinGW Makefiles"
cd ..
pause