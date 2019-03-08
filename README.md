# Server fixes list
https://raw.githubusercontent.com/Vityacv/fearservmod/master/src/fixes

# How to compile:
requirements: linux distribution (i use archlinux), mingw-w64, WINE 3+, nasm.

Download https://github.com/Vityacv/libsplice and https://github.com/Vityacv/libmisc. Compile it. Place it together with fearservmod folder.

compile:
```
./build -c
```
# How to use F.E.A.R. Perseus Mandate:
1. Download "F.E.A.R. Platinum" (GOG version)
2. In "FEARXP2" directory rename "StringEditRuntime.dll" to "StringEditRuntime_.dll" (name "StringEditRuntime_.dll" is very important because dll loads it)
3. Move there new "StringEditRuntime.dll" https://github.com/Vityacv/fearservmod/blob/master/bin/StringEditRuntime.dll and "Default.archcfg" https://github.com/Vityacv/fearservmod/blob/master/extra/Default.archcfg (make backup of original "Default.archcfg")
4. Now you can start your server with "FEARServerXP.exe" or check avaliable servers (only FEARXP2 servers, if there any) in game browser of "FEARXP2.exe" and play
5. Type random text in CDKey before connect to server

# How to use F.E.A.R. Combat 1.08:
1. Download FEAR Combat v1.07 Free Full Game from here http://www.gamershell.com/pc/fear_combat/ and while install do not accept punkbuster license (cdkey you should find in comments)
2. Apply patch "FEAR Combat v1.08 US Patch"
3. In game directory rename "StringEditRuntime.dll" to "StringEditRuntime_.dll"
4. Move there new "StringEditRuntime.dll" https://github.com/Vityacv/fearservmod/blob/master/bin/StringEditRuntime.dll
5. unpack files from archive (replace original files) https://www.upload.ee/files/9672698/fixfiles.7z.html (password: fix) to game directory 
6. Type random text in CDKey "Multiplayer->Client settings" before connect to server OR cdkey from https://fear-community.org - it needed to be put in Key.ini file that in profile folder
To launch server with fear-combat.org as master you also need to add following lines to gamecfg.txt:

	[Server]
	NS1=n1.fear-combat.org
	NS2=n2.fear-combat.org
	Available=fear.avail.fear-combat.org
	Master=fear.m.fear-combat.org

Added "coop mode" support:

F.E.A.R.

F.E.A.R. Extraction Point

F.E.A.R. Perseus Mandate

All 55 SinglePlayer maps will be avaliable in server list. Players can use "Call vote" -> "Next map" or "New map" to select specific map (for scroll list use PageUp/PageDown).
Use "FEARXP2.exe" for play and "FEARServerXP.exe" to start local or public server.
If you want start coop server - add to server config "ServerOptionsXXXX.txt" option:

	[Extra]
	CoopMode=1

There are another options you can use in usual MP mode:

PreventNoclip - prevents player to go through solid objects (walls for example)

SyncObjects - synchronize all objects on map for all players (bottles, boxes, etc)

BotsMP - AI fix for custom user maps
