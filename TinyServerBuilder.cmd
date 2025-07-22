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
ArchiveEdit.exe -Decompress "Decompress" "%GameDir%\%%A"
))
robocopy.exe "Decompress" "Data" "*.ini" "*.dat" "*.AnmTree00p" "*.Gamdb00p" "*.Strdb00p" "*.Model00p" "*.World00p" /NFL /S /XF "WorldEdit.ini" /XD "Prefabs" "Release" "ReleaseXP2"
robocopy.exe "Decompress" "%FEARServer%" "GameServer.dll"
robocopy.exe "%GameDir%" "%FEARServer%" "%FEARServer%.exe" "EngineServer.dll" "GameDatabase.dll" "LTMemory.dll" "Monolith.PropertyGrid.dll" "StringEditRuntime.dll" "MFC71u.dll" "msvcp71.dll" "msvcr71.dll" "enginemsg.txt" "serverreadme.txt"
(
echo %FEAR%.Arch00
echo .
)>"%FEARServer%\Default.archcfg"
echo start "" %FEARServer%.exe -UserDirectory Config>"%FEARServer%\%FEARServer%.cmd"
echo FFFE>"Data\Data.Arch00s"
certutil.exe -f -decodehex "Data\Data.Arch00s" "Data\Data.Arch00s"
cmd.exe /u /c "echo [General]&echo Version=2&echo Packed=..\%FEARServer%\%FEAR%.Arch00&echo Download=False&echo CRC=True&echo [Folder Include]&echo .=Folders:True,Files:True&echo [Extension Include]&echo Arch00=False&echo Arch00s=False">>"Data\Data.Arch00s"
ArchiveEdit.exe -Config "Data\Data.Arch00s"
rmdir /s /q "Decompress","Data"
