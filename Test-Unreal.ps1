param([string]$EngineRoot = 'E:\UE_5.8')
$ErrorActionPreference = 'Stop'
$projectPath = Join-Path $PSScriptRoot 'Cloudwake.uproject'
$editorPath = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
foreach ($mode in @('BHTest', 'BHWalk', 'BHSaveTest')) {
    $logPath = Join-Path $PSScriptRoot "Saved\Logs\$mode.log"
    & $editorPath $projectPath /Game/Maps/Bellheart -game "-$mode" -unattended -nullrhi -nosplash "-abslog=$logPath"
    $result = $LASTEXITCODE
    Select-String -Path $logPath -Pattern 'BH_TEST_|BH_WALK_|BH_SAVE_TEST_' | ForEach-Object { $_.Line }
    if ($result -ne 0) { throw "$mode failed with exit code $result. See $logPath" }
    $marker = switch ($mode) { 'BHTest' { 'BH_TEST_COMPLETE' } 'BHWalk' { 'BH_WALK_COMPLETE' } 'BHSaveTest' { 'BH_SAVE_TEST_COMPLETE' } }
    if (-not (Select-String -Path $logPath -Pattern $marker -Quiet)) { throw "$mode did not reach completion." }
}
