@echo off
cd /d %~dp0
title F.E.A.R. Tiny Server Builder

if not exist "ArchiveEdit.exe" (
echo Can't find ArchiveEdit.exe!
pause
exit
)
set /p GameDir="Enter path to the game directory: "
if exist "%GameDir%\FEARServer.exe" (
set FEARServer=FEARServer
set FEAR=FEAR
) else if exist "%GameDir%\FEARServerXP.exe" (
set FEARServer=FEARServerXP
set FEAR=FEAR_XP
) else (
echo Can't find game files!
pause
exit
)

for /f "usebackq tokens=*" %%A in ("%GameDir%\Default.archcfg") do (
if "%%~xA" == ".Arch00" (
ArchiveEdit.exe -Decompress "Archs" "%GameDir%\%%A"
))
robocopy.exe "Archs" "Arch" /NFL /S "*.ini" "*.dat" "*.AnmTree00p" "*.Gamdb00p" "*.Strdb00p" "*.Model00p" "*.Objects00p" "*.World00p" /XF "WorldEdit.ini" /XD "Prefabs" "Release" "ReleaseXP2"
robocopy.exe "Archs" "%FEARServer%" "GameServer.dll"
robocopy.exe "%GameDir%" "%FEARServer%" "%FEARServer%.exe" "EngineServer.dll" "GameDatabase.dll" "Monolith.PropertyGrid.dll" "StringEditRuntime.dll" "MFC71u.dll" "msvcp71.dll" "msvcr71.dll" "enginemsg.txt" "serverreadme.txt"
(
echo %FEAR%.Arch00
echo .
)>"%FEARServer%\Default.archcfg"
echo start "" %FEARServer%.exe -UserDirectory Config>"%FEARServer%\%FEARServer%.cmd"
echo FFFE>"Arch\%FEAR%.Arch00s"
certutil.exe -f -decodehex "Arch\%FEAR%.Arch00s" "Arch\%FEAR%.Arch00s"
cmd.exe /u /c "echo [General]&echo Version=2&echo Packed=..\%FEARServer%\%FEAR%.Arch00&echo Download=False&echo CRC=True&echo [Folder Include]&echo .=Folders:True,Files:True&echo [Extension Include]&echo Arch00=False&echo Arch00s=False">>"Arch\%FEAR%.Arch00s"
ArchiveEdit.exe -Config "Arch\%FEAR%.Arch00s"
rmdir /s /q "Arch","Archs"
