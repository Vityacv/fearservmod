$Host.UI.RawUI.WindowTitle = "F.E.A.R. Tiny Server Builder"

if (!(Test-Path -Path "$PSScriptRoot\ArchiveEdit.exe")){
Write-Host "Can't find ArchiveEdit.exe!"
Start-Sleep -Seconds 5
break
}
$GameDir = Read-Host -Prompt "Enter path to the game directory"
if (Test-Path -Path "$GameDir\FEARServer.exe"){
$FEARServer = "FEARServer"
$FEAR = "FEAR"
} elseif (Test-Path -Path "$GameDir\FEARServerXP.exe"){
$FEARServer = "FEARServerXP"
$FEAR = "FEAR_XP"
} else {
Write-Host "Can't find game files!"
Start-Sleep -Seconds 5
break
}

foreach ($ArchCfg in (Get-Content -Force -Path "$GameDir\Default.archcfg") -like "*.Arch00"){
Start-Process -Wait -FilePath "$PSScriptRoot\ArchiveEdit.exe" -ArgumentList "-Decompress ""$PSScriptRoot\Archs"" ""$GameDir\$ArchCfg"""
}

Get-ChildItem -Force -Recurse "$PSScriptRoot\Archs" | ForEach-Object {
if ($_.Name -like "*.ini" -or $_.Name -like "*.dat" -or $_.Name -like "*.AnmTree00p" -or $_.Name -like "*.Gamdb00p" -or $_.Name -like "*.Strdb00p" -or $_.Name -like "*.Model00p" -or $_.Name -like "*.World00p" -and $_.Name -notlike "WorldEdit.ini" -and $_.DirectoryName -notlike "*\Prefabs\*" -and $_.DirectoryName -notlike "*\Worlds\Release" -and $_.DirectoryName -notlike "*\Worlds\ReleaseXP2"){
New-Item -Force -ItemType Directory -Path ("$PSScriptRoot\Arch"+$_.DirectoryName.Substring(("$PSScriptRoot\Archs").Length)) | Out-Null
Copy-Item -Force -Path $_.FullName -Destination ("$PSScriptRoot\Arch"+$_.FullName.Substring(("$PSScriptRoot\Archs").Length))
}}
New-Item -Force -ItemType Directory -Path "$PSScriptRoot\$FEARServer" | Out-Null
Copy-Item -Force -Path "$PSScriptRoot\Archs\GameServer.dll" -Destination "$PSScriptRoot\$FEARServer\GameServer.dll"
Copy-Item -Force -Path "$GameDir\*" -Destination "$PSScriptRoot\$FEARServer" -Include "$FEARServer.exe","EngineServer.dll","GameDatabase.dll","Monolith.PropertyGrid.dll","StringEditRuntime.dll","MFC71u.dll","msvcp71.dll","msvcr71.dll","enginemsg.txt","serverreadme.txt"

$File = @"
[General]
Version=2
Packed=..\$FEARServer\$FEAR.Arch00
Download=False
CRC=True
[Folder Include]
.=Folders:True,Files:True
[Extension Include]
Arch00=False
Arch00s=False
"@
Set-Content -Force -Encoding unicode -Path "$PSScriptRoot\Arch\$FEAR.Arch00s" -Value $File
Start-Process -Wait -FilePath "$PSScriptRoot\ArchiveEdit.exe" -ArgumentList "-Config ""$PSScriptRoot\Arch\$FEAR.Arch00s"""

$File = @"
$FEAR.Arch00
.
"@
Set-Content -Force -Encoding ascii -Path "$FEARServer\Default.archcfg" -Value $File

$File = @"
start "" $FEARServer.exe -UserDirectory Config
"@
Set-Content -Force -Encoding ascii -Path "$FEARServer\$FEARServer.cmd" -Value $File

Remove-Item -Force -Recurse -Path "$PSScriptRoot\Arch","$PSScriptRoot\Archs"
