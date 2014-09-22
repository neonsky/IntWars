# Compiling on Mac
Make sure you have `CMake`, `make` and a compiler (bundled with XCode) installed. After that, run the following commands to setup the makefiles:

    IntWars root$ mkdir build
    IntWars root$ cd build
    IntWars root$ cmake ..
Once you have done this, you can compile IntWars at any time by going into the build folder (`cd build`) and running

    IntWars root$ make
The compiled executable will be in `IntWars/build/gamed` (named `gamed`).

# Running the server on Mac
After you have compiled the server, you can run it simply by executing the following command in the folder the `gamed` executable is in:

    ./gamed

*IntWars complains about missing `filearchives`?*

If IntWars complains about missing filearchives, you forgot to copy them over from the official LoL client.
They can be found using the following method:
- Right-click on your `League of Legends.app` (usually in /Applications) and choose _Show package contents_
- Navigate to __Contents/LoL/RADS/projects/lol_game_client__
- Copy the `filearchives` folder to __IntWars/build/gamed__ (or wherever your executable is located)

# Connecting to the server on Mac
Open up __Terminal__ and navigate to your `League of Legends.app` location (usually /Applications):

    cd /Applications/League\ of\ Legends.app
After that, navigate to the `lol_game_client` folder:

    cd Contents/LoL/RADS/projects/lol_game_client/releases
Then, press __*TAB*__ to autocomplete the current version (0.0.0.137 at the time of writing)

    cd 0.0.0.137/
Almost there! Now navigate to the `Leagueoflegends` executable:

    cd deploy/LeagueOfLegends.app/Contents/MacOS/

Now rests us just running the `Leagueoflegends` executable!

    riot_launched=true ./Leagueoflegends 8394 LoLLauncher "" "IP_OF_SERVER PORT_OF_SERVER KEY_OF_SERVER PLAYER_ID"

For localhost:

    riot_launched=true ./Leagueoflegends 8394 LoLLauncher "" "127.0.0.1 5119 17BLOhi6KZsTtldTsizvHg== 1"


This all can be combined into one command:

    cd /Applications/League\ of\ Legends.app/Contents/LoL/RADS/projects/lol_game_client/releases/CLIENT_VERSION/deploy/LeagueOfLegends.app/Contents/MacOS && riot_launched=true ./Leagueoflegends 8394 LoLLauncher "" "IP_OF_SERVER PORT_OF_SERVER KEY_OF_SERVER PLAYER_ID"
