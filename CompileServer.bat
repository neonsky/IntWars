@echo off
echo Building IntWars Server.
echo ------------------------------
cd codeblocksbuild
mingw32-make -j4 all
cd ..
pause
