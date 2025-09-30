# FearServMod is a client and server mod for
F.E.A.R. v1.08<br>
F.E.A.R. Combat v1.08<br>
F.E.A.R. Perseus Mandate v1.0

# Features list
Multiplayer works again thanks to openspy.net<br>
Client side fixes (fps drops, flickering)<br>
Server side fixes (anti-cheat, crash hacks)<br>
Allows coop mode based on F.E.A.R. Platinum (contains 55 maps)<br>
Allows to play all singleplayer maps F.E.A.R. Platinum in F.E.A.R. Perseus Mandate<br>
Allows play on servers created on retail version to free version and vice versa<br>
Allows to create 64 players servers<br>
Allows play on servers created by TinyServerBuilder<br>

# How to play F.E.A.R. v1.08
1. Install a game
2. Replace FEAR.exe and FEARMP.exe to FEAR.exe from "No-CD patch" https://github.com/anzz1/openspy-client
3. Put https://github.com/Vityacv/fearservmod/blob/master/bin/winmm.dll to game folder
4. Launch the game and select Multiplayer - Client Settings - CD Key - type random text (should be unique for every player in same server)

# How to play F.E.A.R. Combat v1.08
1. Find free game on https://github.com/anzz1/openspy-client
2. Install a game and apply v1.08 update
3. Replace FEARMP.exe to FEAR.exe from "No-CD patch" https://github.com/anzz1/openspy-client
4. Put https://github.com/Vityacv/fearservmod/blob/master/bin/winmm.dll to game folder
5. Launch the game and select Multiplayer - Client Settings - CD Key - type random text (should be unique for every player in same server)

# How to play F.E.A.R. Perseus Mandate v1.0
1. Install a game
2. Replace FEARXP2.exe from "No-CD patch" https://github.com/anzz1/openspy-client
3. Put https://github.com/Vityacv/fearservmod/blob/master/bin/winmm.dll to game folder
4. Launch the game and select Multiplayer - Client Settings - CD Key - type random text (should be unique for every player in same server)

# How to play Coop Mode based on F.E.A.R. Platinum
1. Install a game
2. Replace FEARXP2.exe from "No-CD patch" https://github.com/anzz1/openspy-client
3. Put https://github.com/Vityacv/fearservmod/blob/master/bin/winmm.dll to game folder
4. Open Default.archcfg in notepad and replace content with:
```
; FEAR ARCHIVES
..\FEAR.Arch00
..\FEARA.Arch00
..\FEARL.Arch00
..\FEARE.Arch00
..\FEAR_1.Arch00
..\FEARA_1.Arch00
..\FEARL_1.Arch00
..\FEARE_1.Arch00
..\FEAR_2.Arch00
..\FEARA_2.Arch00
..\FEARL_2.Arch00
..\FEARE_2.Arch00
..\FEAR_3.Arch00
..\FEARA_3.Arch00
..\FEARL_3.Arch00
..\FEARE_3.Arch00
..\FEAR_4.Arch00
..\FEARA_4.Arch00
..\FEARL_4.Arch00
..\FEARE_4.Arch00
..\FEAR_5.Arch00
..\FEARA_5.Arch00
..\FEARL_5.Arch00
..\FEARE_5.Arch00
..\FEAR_6.Arch00
..\FEARA_6.Arch00
..\FEARL_6.Arch00
..\FEARE_6.Arch00
..\FEAR_7.Arch00
..\FEARA_7.Arch00
..\FEARL_7.Arch00
..\FEARE_7.Arch00
..\FEAR_8.Arch00
..\FEARA_8.Arch00
..\FEARL_8.Arch00
..\FEARE_8.Arch00

; FEAR XP ARCHIVES
..\FEARXP\FEAR_XP.Arch00
..\FEARXP\FEARA_XP.Arch00
..\FEARXP\FEARL_XP.Arch00
..\FEARXP\FEARE_XP.Arch00

; FEAR XP2 ARCHIVES
FEAR_XP.Arch00
FEARM_XP.Arch00
FEARA_XP.Arch00
FEARL_XP.Arch00
FEARE_XP.Arch00
```
5. Launch the game and change following settings:<br>
    • Multiplayer - Host - Bandwidth - T3 (avoid lags)<br>
    • Multiplayer - Client Settings - CD Key - type random text (should be unique for every player in same server)<br>
    • Multiplayer - Client Settings - Bandwidth - T3 (avoid lags)<br>
6. Open ServerOptions0000.txt and add on top of it:
```
[Extra]
CoopMode=1

[SinglePlayer]
SessionName=Coop Mode
```
7. Use FEARXP2.exe to play and FEARServerXP.exe to start local or public server<br>

Players can use "Call vote" -> "Next map" or "New map" to select specific map (to scroll list use PageUp/PageDown)<br>

#### Additional options in ServerOptions0000.txt for all type of game servers
1. Synchronize all objects on map for all players (bottles, boxes, etc.)
```
[Extra]
SyncObjects=1
```

#### OpenSpy is default master server, but if you want use other master server, add following lines to gamecfg.txt

```
[Client]
;ShowIntro=1
Master=http://master.fear-combat.org/api/serverlist-ingame.php
;MOTD=http://motd.gamespy.com/motd/vercheck.asp?userid=%d&productid=%d&versionuniqueid=%s&distid=%d&uniqueid=%s&gamename=%s

[Server]
Master=master.fear-combat.org
Available=master.fear-combat.org
NS1=n1.fear-combat.org
NS2=n2.fear-combat.org

```
#### Portable mode and Window mode
If you want to run a game in portable mode, create cmd file:<br>
start "" FEARMP.exe -UserDirectory Config<br>
start "" FEARServer.exe -UserDirectory Config<br>

If you want to run game in window mode:<br>
start "" FEARMP.exe -UserDirectory Config +Windowed 1 +ScreenWidth 1024 +ScreenHeight 768

# Tiny Server Builder
This script creates tiny server build where stored only required files to run server, useful for servers with limited storage. Find the ArchiveEdit in official public SDK, it also needs F.E.A.R. Combat to be installed first<br>
<details>
  <summary>Original installers SHA256 hashes</summary>

| Name | SHA256 Hash |
|----------------------------------|------------------------------------------------------------------|
| fearcombat_en_107.exe | 28d12729d866fc36d13ab3761c8826edc4eedb3c76d0c823302a4a25cb265bed |
| fearcombat_update_en_107_108.exe | b016866880353a608f5f37184e833e7a25be12227cb671457a1b600b9f1b350b |
| fear_publictools_108.exe | 11aaa4128528403f7bc9ea5119c68051c62b92a99e6411dfd749af55e9b19df8 |

</details>

1. Put batch script https://github.com/Vityacv/fearservmod/blob/master/TinyServerBuilder.cmd to writable folder with enough disk space<br>
2. Add to folder with a script ArchiveEdit.exe, MFC71u.dll, msvcp71.dll, msvcr71.dll<br>
3. Run script and specify game folder (right click to paste path)<br>
4. Wait till script completes and find new server builds in FEARServer or FEARServerXP folders<br>

Also, tiny coop mode server is supported, in the script replace the filter "Release" "ReleaseXP2" to "ReleaseMultiplayer" "MultiplayerXP2"

# List of fixes
Client DRM-free FEAR.exe as FEARMP.exe for better performance<br>
Client dinput random high drop fps<br>
Client SetWindowsHookExA input lag<br>
Client explosion flickering on high refresh rate monitors, like 144hz etc.<br>
Client disable load of ICMP.dll, PBCL.dll, PBSV.dll<br>
Client crash on pressing ESC while downloading content<br>
Client crash on pressing TAB when server have more than 16 players<br>
Client run multiple instances<br>
Client no pause render when unfocused<br>
Client window style<br>
Client antialiasing in windowed mode<br>
Client fonts distortion in windowed mode<br>
Client disable intro on start game<br>
Client grenade throw +5 fake when move +drop +C<br>
Client controls reset if select non-English keyboard and quit<br>
Client connect from favorites to remote servers<br>
Client XP2 MOTD link click<br>
Server retail, free, tiny versions allows connection to all type of clients<br>
Server XP2 AI spawn on coop maps<br>
Server disable load of PBSV.dll<br>
Server DoS fake players spam<br>
Server DoS clients chat spam hack<br>
Server DoS clients team broadcast spam hack<br>
Server vote hack<br>
Server vote new map crash clients without of index hack<br>
Server vote clear after new map loaded<br>
Server crash early respawn<br>
Server crash animation speed hack<br>
Server crash invalid skey hack<br>
Server crash invalid guid hack<br>
Server crash negative fRate hack<br>
Server crash force load on new map hack<br>
Server crash render stimulus hack<br>
Server crash object alpha hack<br>
Server crash after failed initialization<br>
Server hang when joined 2 players with name 0xFFFF<br>
Server empty config file name on first startup<br>
Server missing creation of AdditionalContent folder<br>
Server run speed hack v1<br>
Server ladder animation glitch<br>
Server multiple kills of same target<br>
Server chat string size limit<br>
Server host options hack<br>
Server pause hack<br>
Server frag self unlimited ammo<br>
Server drop grenade unlimited ammo<br>
Server get weapon hack<br>
Server get health hack<br>
Server get armor hack<br>
Server get gear hack<br>
Server invalid skin hack<br>
Server bulletproof hack v1 and v2<br>
Server immortal player hack<br>
Server send damage hack<br>
Server send sounds hack<br>
Server pickup distance hack (2m limit)<br>
Server invalid team hack<br>
Server slowmo activate without recharge hack<br>
Server slowmo activate without recharger hack<br>
Server teleport to players hack<br>
Server teleport to respawn checkpoints hack<br>
Server teleport using SetObjectPos hack<br>
Server player no clip hack<br>
Server player early respawn glitch<br>
Server stealth join hack<br>
Server weapon fast switch glitch<br>
Server weapon fake fire position hack<br>
Server weapon rapid fire timer in future hack<br>
Server weapon rapid fire invalid msg hack<br>
Server weapon rapid fire unarmed hack<br>
Server weapon fire drop ammo hack<br>
Server weapon switch drop ammo hack<br>
Server weapon reload drop ammo hack<br>
Server weapon melee ammo mod hack<br>
Server remote flashlight hack<br>
Server pickup objects of enemy or team hack<br>
Server log spam FindObjectsCb<br>
Server log show client ip:port<br>
Server enhanced vote delay<br>
Server enhanced vote ban<br>
Server platform type "L" when running in linux wine<br>

# How to launch game or server on Linux using Wine
To launch FEARMP.exe you will need to instruct wine to load winmm.dll from game folder first and then load system winmm.dll using environment variable:
```
WINEDLLOVERRIDES="winmm.dll=n,b" wine FEARMP.exe
```
Same variable is needed to run FEARServer.exe, but you also can disable load of gecko engine and .net:
```
WINEDLLOVERRIDES="winmm.dll=n,b;mscoree.dll;mshtml.dll=" wine FEARServer.exe
```

# How to compile
Requirements: linux distribution (I use Arch Linux), mingw-w64, nasm.

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
For clang builds:
```
PATH=/opt/llvm-mingw/llvm-mingw-msvcrt/bin/:$PATH i686-w64-mingw32-cmake -DCMAKE_BUILD_TYPE=Release ..
```
