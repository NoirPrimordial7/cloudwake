@echo off
cd /d "%~dp0"
"tools\blender\blender-4.2.9-windows-x64\blender.exe" --background --python tools/export-harbor-v2.py
