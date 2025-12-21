@echo off
rem Package portable folder with main.exe and assets into calculadora_portable.zip
setlocal enableextensions enabledelayedexpansion
@echo off
rem Package portable folder with main.exe and assets into calculadora_portable.zip
setlocal enableextensions enabledelayedexpansion

:: Directory of this script (project root)
set SRC_DIR=%~dp0
set DIST=%SRC_DIR%dist

:: Remove old dist if exists
if exist "%DIST%" rd /s /q "%DIST%"
mkdir "%DIST%"

:: Copy exe
if exist "%SRC_DIR%main.exe" (
	copy /Y "%SRC_DIR%main.exe" "%DIST%" >nul
) else (
	echo main.exe not found in %SRC_DIR%
)

:: Copy assets folder recursively
if exist "%SRC_DIR%assets" (
	xcopy "%SRC_DIR%assets" "%DIST%\assets" /E /I /Y >nul
) else (
	echo assets folder not found in %SRC_DIR%
)

:: Copy historial.txt if exists
if exist "%SRC_DIR%historial.txt" (
	copy /Y "%SRC_DIR%historial.txt" "%DIST%" >nul
)

:: If APPDATA master historial exists, copy it into package as well
for %%a in ("%APPDATA%\CalculadoraMoya\historial.txt") do (
	if exist %%~a (
		copy /Y "%%~a" "%DIST%\historial_appdata.txt" >nul
	)
)

:: Copy any DLLs present next to exe (raylib.dll etc.)
for %%f in ("%SRC_DIR%*.dll") do (
	copy /Y "%%~f" "%DIST%" >nul
)

:: Create zip via PowerShell (portable on Windows 10+)
powershell -Command "if(Test-Path '%SRC_DIR%calculadora_portable.zip'){Remove-Item '%SRC_DIR%calculadora_portable.zip'}; Compress-Archive -Path '%DIST%\*' -DestinationPath '%SRC_DIR%calculadora_portable.zip'"

echo Packaged into calculadora_portable.zip
pause


pause
necho Packaged into calculadora_portable.zippowershell -Command "if(Test-Path '%SRC_DIR%calculadora_portable.zip'){Remove-Item '%SRC_DIR%calculadora_portable.zip'}; Compress-Archive -Path '%DIST%\*' -DestinationPath '%SRC_DIR%calculadora_portable.zip'"
:: Create zip via PowerShell (portable on Windows 10+))    copy /Y "%%~f" "%DIST%" >nulfor %%f in ("%SRC_DIR%*.dll") do (
:: Copy any DLLs present next to exe (raylib.dll etc.))    echo assets folder not found in %SRC_DIR%) else (    xcopy "%SRC_DIR%assets" "%DIST%\assets" /E /I /Y >nulif exist "%SRC_DIR%assets" (
:: Copy assets folder recursively)    echo main.exe not found in %SRC_DIR%) else (    copy /Y "%SRC_DIR%main.exe" "%DIST%" >nulif exist "%SRC_DIR%main.exe" (
:: Copy exemkdir "%DIST%"if exist "%DIST%" rd /s /q "%DIST%"
:: Remove old dist if existsset DIST=%SRC_DIR%distset SRC_DIR=%~dp0:: Directory of this script (project root)