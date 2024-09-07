# FearServMod is a client and server mod for
F.E.A.R. v1.08<br>
F.E.A.R. Combat v1.08<br>
F.E.A.R. Perseus Mandate v1.0

# Feature list:
Multiplayer works again thanks to openspy.net<br>
Client side fixes (random fps drops, flickering)<br>
Server side fixes (anti-cheat and server crash hacks)<br>
Allows coop mode based on "F.E.A.R. Platinum"<br>
Allows to play all singleplayer maps "F.E.A.R. Platinum" in "F.E.A.R. Perseus Mandate"<br>
Allows connect to servers created on retail version to combat version and vice versa

# How to play F.E.A.R. Combat v1.08:
1. Find game on https://github.com/anzz1/openspy-client
2. Install game and apply v1.08 update
3. Replace FEARMP.exe to FEAR.exe from "No-CD patch" https://github.com/anzz1/openspy-client
4. Put winmm.dll (https://github.com/Vityacv/fearservmod/blob/master/bin/winmm.dll) game mod to game folder
5. Run game and in client settings type random text into CDKey (shold be unique for every player in same server)

# How to play F.E.A.R. v1.08:
1. Use "F.E.A.R. Platinum" version
2. Install game
3. Replace FEAR.exe and FEARMP.exe to FEAR.exe from "No-CD patch" https://github.com/anzz1/openspy-client
4. Put winmm.dll (https://github.com/Vityacv/fearservmod/blob/master/bin/winmm.dll) game mod to game folder
5. Run game and in client settings type random text into CDKey (shold be unique for every player in same server)

# How to play F.E.A.R. Perseus Mandate v1.0:
1. Use "F.E.A.R. Platinum" version
2. Install game
3. Replace FEARXP2.exe from "No-CD patch" https://github.com/anzz1/openspy-client
4. Put winmm.dll (https://github.com/Vityacv/fearservmod/blob/master/bin/winmm.dll) game mod to game folder
5. Open Default.archcfg in notepad and replace content with:

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

6. Run game and in client settings type random text into CDKey (shold be unique for every player in same server)

Now you can connect to avaliable servers or configure new server and start it.

# How to use coop mode in "F.E.A.R. Platinum" based on F.E.A.R. Perseus Mandate v1.0:
1. Open ServerOptionsXXXX.txt and add:
```
[Extra]
CoopMode=1
```
2. Start server<br>

# CoopMode supports:<br>
F.E.A.R.<br>
F.E.A.R. Extraction Point<br>
F.E.A.R. Perseus Mandate

All 55 SinglePlayer maps will be avaliable in server list. Players can use "Call vote" -> "Next map" or "New map" to select specific map (for scroll list use PageUp/PageDown).<br>
Use "FEARXP2.exe" for play and "FEARServerXP.exe" to start local or public server.


Additional options in ServerOptionsXXXX.txt for all game servers:
1. AI fix for custom user maps
```
[Extra]
BotsMP=1
```
2. Synchronize all objects on map for all players (bottles, boxes, etc)
```
[Extra]
SyncObjects=1
```

OpenSpy is default master server, but if you want connect to alternative server, add lines to gamecfg.txt:

```
[Client]
Master=http://cpp.mx/api/serverlist-ingame.php
; MOTD=http://motd.gamespy.com/motd/vercheck.asp?userid=%d&productid=%d&versionuniqueid=%s&distid=%d&uniqueid=%s&gamename=%s
[Server]
NS1=n1.fear-combat.org
NS2=n2.fear-combat.org
Available=fear.avail.fear-combat.org
Master=fear.m.fear-combat.org
```

# Portable mode and Window mode:
If you want enable portable mode, create cmd file:<br>
start /b FEARMP.exe -userdirectory Config<br>
start /b FEARServer.exe -userdirectory Config

If you want run game in window mode, set in settings.cfg;<br>
"Windowed" "1"<br>
and make it read only to avoid reset

# List of fixes:
Allow use of FEAR.exe as FEARMP.exe to bypass DRM crash<br>
Fix dinput random high drop fps<br>
Fix explosion flickering on high refresh rate monitors, like 144hz+<br>
Disable load of ICMP.dll, PBCL.dll, PBSV.dll<br>
Fix crash on pressing ESC while downloading content<br>
Allow run of multiple instances and no render pause when unfocused<br>
Disable intro on start game<br>
Without weapon (fast switch bug)<br>
Grenade throw (+5 when move +drop +C)<br>
Keyboard reset if non english on close game<br>
Retail version can connect to Combat servers<br>
Combat version can connect to Retail servers<br>
Connect from favorites to remote servers<br>
DoS fake players spam hack<br>
Vote new map crash clients hack (out of index)<br>
Vote hack<br>
Multiple kills of same target bug<br>
Clear vote process after map loaded<br>
Early respawn crash<br>
Ainimation speed crash<br>
Invalid skey and guid crash<br>
Negative fRate crash<br>
Force load on new map crash<br>
Login x3 players with nickname 0xFFFF client crash<br>
FindObjectsCb log spam<br>
Player respawn bug (force default mp rules)<br>
Chat string size limit<br>
Host options hack<br>
Server pause hack<br>
Render stimulus crash hack<br>
Object alpha crash hack<br>
Frag self hack<br>
Get weapon hack<br>
Get health hack<br>
Get armor hack<br>
Get gear hack<br>
Unlimited drop grenade hack<br>
Fake fire position hack<br>
Invalid skin hack<br>
Immortal player hack<br>
Send damage hack<br>
Send sounds hack<br>
Pickup distance hack (2m limit)<br>
Invalid team hack<br>
Activate slowmo without recharger or not charged hack<br>
Teleport to players hack<br>
Teleport to respawn checkpoints hack<br>
Teleport using SetObjectPos hack<br>
Stealth join hack<br>
Rapid fire timer in future hack<br>
Rapid fire invalid msg hack<br>
Rapid fire unarmed hack<br>
Ladder animation glitch<br>
Remote flashlight hack<br>
Melee ammo mod hack<br>
Drop ammo weapon fire hack<br>
Drop ammo weapon switch hack<br>
Pickup enemy or team pickups hack<br>
Enhanced server log<br>

# How to compile:
requirements: linux distribution (I use archlinux), mingw-w64, nasm.

build steps:
```
git clone https://github.com/Vityacv/fearservmod.git
cd fearservmod
git submodule update --init --recursive
mkdir build
cd build
i686-w64-mingw32-cmake -DCMAKE_BUILD_TYPE=Release ..
make
```