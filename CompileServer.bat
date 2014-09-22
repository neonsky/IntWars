@echo off
echo Building IntWars Server.
echo ------------------------------
cd build
mingw32-make -j8 all
cd ..
pause
