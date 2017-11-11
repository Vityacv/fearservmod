# How to compile:
requirements: linux distribution (i use archlinux), clang, mingw-w64-clang wrapper for clang, mingw32 compiler libs, WINE, nasm.

Download splice engine (https://github.com/Vityacv/splice-engine) and compile it with ./build -c. Place it together with fearservmod folder.

compile:
```
./build -w #Initialization of symbolic links for ulink
./build -g #compile precompiled header
./build -c
```
# How to use:
1. Download "F.E.A.R. Platinum" (GOG version)
2. In "FEARXP2" directory rename "StringEditRuntime.dll" to "StringEditRuntime_.dll" (name "StringEditRuntime_.dll" is very important because dll loads it)
3. Move there new "StringEditRuntime.dll" and "Default.archcfg" (make backup of original "Default.archcfg")
4. Now you can start your server with "FEARServerXP.exe" or check avaliable servers (only FEARXP2 servers, if there any) in game browser of "FEARXP2.exe" and play
5. Type random text in CDKey before connect to server

Added "coop mode" support:

F.E.A.R.

F.E.A.R. Extraction Point

F.E.A.R. Perseus Mandate

All 55 SinglePlayer maps will be avaliable in server list. Players can use "Call vote" -> "Next map" or "New map" to select specific map (for scroll list use PageUp/PageDown).
Use "FEARXP2.exe" for play and "FEARServerXP.exe" to start local or public server.
If you want start coop server - add to server config "ServerOptionsXXXX.txt" option:

[Patches]
CoopMode=1

There are another options you can use in usual MP mode:

PreventNoclip - prevents player to go through solid objects (walls for example)

SyncObjects - synchronize all objects on map for all players (bottles, boxes, etc)

PreventSpecialMsg - avoid use of special msgid (OBEJCT_MESSAGE) which allows hacks to get weapons/ammo/health etc.

BotsMP - AI fix for custom user maps
