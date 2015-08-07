@echo off
echo Generate IntWars Makefiles
echo ------------------------------
IF EXIST .\build\ (
GOTO BUILDDIR
) ELSE (
GOTO MAIN
)

:BUILDDIR
echo Build directory exists, removing...
rmdir /s /q build
GOTO MAIN

:MAIN
echo Recreating build directory...
mkdir build
cd build
echo Creating MinGW Makefiles...
echo ------------------------------
cmake .. -G "MinGW Makefiles"
cd ..

echo Building IntWars Server.
echo ------------------------------
cd build
mingw32-make -j4 all
cd ..

setlocal enabledelayedexpansion
echo -----------------------
echo IntWars Solo Launcher
echo -----------------------

start /d "build\gamed" intwars.exe && (
   echo Server started^^!
) || (
   echo Server not found^^!
   echo You have to compile it by yourself, please read a tutorial^^!
   pause
   exit
)

set RADS_PATH=
echo ===================
echo Finding LoL Directory
echo ===================
	SET RADS_PATH="D:\GitHub\League of Legends\RADS"
	goto runApp
)
cls
goto cannotFind
:runApp
set RADS_PATH=%RADS_PATH:/=\%
@cd /d "%RADS_PATH%\solutions\lol_game_client_sln\releases"

set init=0
set v0=0&set v1=0&set v2=0&set v3=0
for /f "delims=" %%F in ('dir * /b ^| findstr ^[0-9]*\.[0-9]*\.[0-9]*\.[0-9]*$') do (
	for /F "tokens=1,2,3,4 delims=." %%i in ("%%F") do (
		if !init! equ 0 ( set init=1&set flag=1 ) else (
			set flag=0
			
			if %%i gtr !v0! ( set flag=1 ) else (
				if %%j gtr !v1! ( set flag=1 ) else (
					if %%k gtr !v2! ( set flag=1 ) else (
						if %%l gtr !v3! ( set flag=1 )
					)
				)
			)
		)
		
		if !flag! gtr 0 (
			set v0=%%i&set v1=%%j&set v2=%%k&set v3=%%l
		)
	)
)

if !init! equ 0 goto cannotFind
set lolver=!v0!.!v1!.!v2!.!v3!

@cd /d "!RADS_PATH!\solutions\lol_game_client_sln\releases\!lolver!\deploy"
if exist "League of Legends.exe" (
	@start "" "League of Legends.exe" "8394" "LoLLauncher.exe" "" "127.0.0.1 5119 17BLOhi6KZsTtldTsizvHg== 1"
	goto exit
)
:cannotFind
echo ===================
echo Cannot find LoL path automatically :(
echo ===================
@pause
goto exit
:exit