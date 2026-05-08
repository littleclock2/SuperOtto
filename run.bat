@echo off
echo Starting SuperOtto...

REM Copy resources if not already next to exe
set EXEDIR=build\Release
if not exist "%EXEDIR%\music" xcopy /E /I /Q resources\music "%EXEDIR%\music"
if not exist "%EXEDIR%\pictures" xcopy /E /I /Q resources\pictures "%EXEDIR%\pictures"
if not exist "%EXEDIR%\music" xcopy /E /I /Q resources\music "%EXEDIR%\music"
if not exist "%EXEDIR%\pictures" xcopy /E /I /Q resources\pictures "%EXEDIR%\pictures"

start "" "%EXEDIR%\SuperOtto.exe"
