# FearServMod is a client and server mod for
F.E.A.R. v1.08<br>
F.E.A.R. Combat v1.08<br>
F.E.A.R. Perseus Mandate v1.0

# Features list
Multiplayer works again thanks to openspy.net<br>
Client side fixes (fps drops, flickering)<br>
Server side fixes (anti-cheat and server crash hacks)<br>
Allows coop mode based on F.E.A.R. Platinum<br>
Allows to play all singleplayer maps F.E.A.R. Platinum in F.E.A.R. Perseus Mandate<br>
Allows connect to servers created on retail version to free version and vice versa<br>
Allows to create 64 players servers<br>
Allows to run small servers by edited game archives (about 600 mb)<br>

# How to play F.E.A.R. v1.08
1. Install game
2. Replace FEAR.exe and FEARMP.exe to FEAR.exe from "No-CD patch" https://github.com/anzz1/openspy-client
3. Put winmm.dll https://github.com/Vityacv/fearservmod/blob/master/bin/winmm.dll to game folder
4. Run game and in client settings type random text into CDKey (shold be unique for every player in same server)

# How to play F.E.A.R. Combat v1.08
1. Find free game on https://github.com/anzz1/openspy-client
2. Install game and apply v1.08 update
3. Replace FEARMP.exe to FEAR.exe from "No-CD patch" https://github.com/anzz1/openspy-client
4. Put winmm.dll https://github.com/Vityacv/fearservmod/blob/master/bin/winmm.dll to game folder
5. Run game and in client settings type random text into CDKey (shold be unique for every player in same server)

# How to play F.E.A.R. Perseus Mandate v1.0
1. Install game
2. Replace FEARXP2.exe from "No-CD patch" https://github.com/anzz1/openspy-client
3. Put winmm.dll https://github.com/Vityacv/fearservmod/blob/master/bin/winmm.dll to game folder
4. Run game and in client settings type random text into CDKey (shold be unique for every player in same server)

# How to play Coop Mode based on F.E.A.R. Platinum
1. Install game
2. Replace FEARXP2.exe from "No-CD patch" https://github.com/anzz1/openspy-client
3. Put winmm.dll https://github.com/Vityacv/fearservmod/blob/master/bin/winmm.dll to game folder
4. Open Default.archcfg in notepad and replace content with:
```
; FEAR ARCHIVES
../FEAR.Arch00
../FEARA.Arch00
../FEARL.Arch00
../FEARA_2.Arch00
../FEARA_3.Arch00
../FEARL_3.Arch00
../FEARL_5.Arch00
../FEARA_7.Arch00
../FEARL_7.Arch00
../FEARA_8.Arch00
../FEARL_8.Arch00

; FEAR XP ARCHIVES
../FEARXP/FEAR_XP.Arch00
../FEARXP/FEARA_XP.Arch00
../FEARXP/FEARL_XP.Arch00

; FEAR XP2 ARCHIVES
FEAR_XP.Arch00
FEARM_XP.Arch00
FEARA_XP.Arch00
FEARL_XP.Arch00
FEARE_XP.Arch00
```
5. Run server and close, open ServerOptionsXXXX.txt and add on top of it:
```
[Extra]
CoopMode=1
```
6. Use FEARXP2.exe to play and FEARServerXP.exe to start local or public server<br>
7. Run game and in client settings type random text into CDKey (shold be unique for every player in same server)

Coop mode includes all 55 maps from:<br>
F.E.A.R.<br>
F.E.A.R. Extraction Point<br>
F.E.A.R. Perseus Mandate

Players can use "Call vote" -> "Next map" or "New map" to select specific map (to scroll list use PageUp/PageDown)<br>

#### Additional options in ServerOptionsXXXX.txt for all type of game servers:
1. Fix AI for custom user maps
```
[Extra]
BotsMP=1
```
2. Synchronize all objects on map for all players (bottles, boxes, etc)
```
[Extra]
SyncObjects=1
```

#### OpenSpy is default master server, but if you want connect to alternative server, add following lines to gamecfg.txt:

```
[Client]
;ShowIntro=1
Master=http://master.fear-combat.org/api/serverlist-ingame.php
;MOTD=http://motd.gamespy.com/motd/vercheck.asp?userid=%d&productid=%d&versionuniqueid=%s&distid=%d&uniqueid=%s&gamename=%s
[Server]
NS1=n1.fear-combat.org
NS2=n2.fear-combat.org
Available=fear.avail.fear-combat.org
Master=fear.m.fear-combat.org
```

#### Portable mode and Window mode:
If you want to run a game in portable mode, create cmd file:<br>
Start /b FEARMP.exe -UserDirectory Config<br>
Start /b FEARServer.exe -UserDirectory Config<br>

If you want run game in window mode:<br>
Start /b FEARMP.exe -UserDirectory Config +Windowed 1 +ScreenWidth 1024 +ScreenHeight 768

# List of fixes
Use DRM-free FEAR.exe as FEARMP.exe for better performance<br>
Random high drop fps from dinput<br>
Input lag from SetWindowsHookExA<br>
Explosion flickering on high refresh rate monitors, like 144hz etc<br>
Disable load of ICMP.dll, PBCL.dll, PBSV.dll<br>
Crash on pressing ESC while downloading content<br>
Crash on pressing TAB when server have more than 16 players<br>
Run multiple instances<br>
No render pause when unfocused<br>
Disable intro on start game<br>
Fast switch weapon bug<br>
Grenade throw +5 fake when move +drop +C<br>
Controls reset if select non english keyboard and close game<br>
Retail version can connect to Combat servers<br>
Combat version can connect to Retail servers<br>
Connect from favorites to remote servers<br>
XP2 MOTD link click<br>
Server DoS fake players spam<br>
Client DoS chat spam<br>
Client DoS team broadcast spam<br>
Vote hack<br>
Vote new map crash clients with out of index hack<br>
Multiple kills of same target bug<br>
Clear vote process after map loaded<br>
Early respawn crash<br>
Animation speed crash<br>
RunSpeed hack v1<br>
Server invalid skey crash<br>
Server invalid guid crash<br>
Negative fRate crash<br>
Force load on new map crash<br>
Server hang when joined 2 players with name 0xFFFF<br>
Server FindObjectsCb log spam<br>
Server crash after Unable to load the selected map<br>
Empty server config file name on first startup<br>
Missing creation of AdditionalContent folder<br>
Player early respawn bug<br>
Ladder animation glitch<br>
Chat string size limit<br>
Style window mode<br>
Host options hack<br>
Pause server hack<br>
Render stimulus crash hack<br>
Object alpha crash hack<br>
Frag self unlimited ammo hack<br>
Unlimited drop grenade hack<br>
Get weapon hack<br>
Get health hack<br>
Get armor hack<br>
Get gear hack<br>
Fake fire position hack<br>
Invalid skin hack<br>
Bulletproof hack v1 and v2<br>
Immortal player hack<br>
Send damage hack<br>
Send sounds hack<br>
Pickup distance hack (2m limit)<br>
Invalid team hack<br>
Slowmo activate without recharge hack<br>
Slowmo activate without recharger hack<br>
Teleport to players hack<br>
Teleport to respawn checkpoints hack<br>
Teleport using SetObjectPos hack<br>
Stealth join server hack<br>
Rapid fire timer in future hack<br>
Rapid fire invalid msg hack<br>
Rapid fire unarmed hack<br>
Remote flashlight hack<br>
Melee ammo mod hack<br>
Weapon fire drop ammo hack<br>
Weapon switch drop ammo hack<br>
Weapon reload drop ammo hack<br>
Pickup objects of enemy or team hack<br>
Enhanced vote ban<br>
Enhanced server log<br>
Server platform type "L" when running in linux wine<br>

# How to launch game or server in linux using wine
To launch FEARMP.exe you will need to instruct wine to load winmm.dll from game folder first and then load system winmm.dll using environment variable:
```
WINEDLLOVERRIDES="winmm=n,b" wine FEARMP
```
Same variable is needed to run FEARServer.exe, but you also can disable load of gecko engine and .net:
```
WINEDLLOVERRIDES="winmm=n,b;mscoree;mshtml=" wine FEARServer
```

# How to compile
Requirements: linux distribution (I use archlinux), mingw-w64, nasm.

Build steps:
```
git clone https://github.com/Vityacv/fearservmod.git
cd fearservmod
git submodule update --init --recursive
mkdir build
cd build
i686-w64-mingw32-cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
for clang builds could use
```
PATH=/opt/llvm-mingw/llvm-mingw-msvcrt/bin/:$PATH i686-w64-mingw32-cmake -DCMAKE_BUILD_TYPE=Release ..
```