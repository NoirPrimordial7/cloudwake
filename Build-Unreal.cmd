@echo off
call "E:\UE_5.8\Engine\Build\BatchFiles\Build.bat" CloudwakeEditor Win64 Development "%~dp0Cloudwake.uproject" -WaitMutex -NoHotReloadFromIDE
pause
