@echo off

set ESP32_PATH=%cd%
set ESP32_BUILD_PATH=%ESP32_PATH%\build
set ESP32_BUILD_PATH=%ESP32_PATH%\build
set CUR_DIR=%cd%\soc_tools

set PROJ_NAME=ESP32-C3
set BSP_VERSION=V0001

copy %ESP32_BUILD_PATH%\bootloader\bootloader.bin %CUR_DIR%\bootloader.bin
copy %ESP32_BUILD_PATH%\luatos_esp32.bin %CUR_DIR%\luatos_esp32.bin
copy %ESP32_BUILD_PATH%\partition_table\partition-table.bin %CUR_DIR%\partition-table.bin

del %ESP32_PATH%\LuatOS-SoC_%BSP_VERSION%_%PROJ_NAME%.soc
%CUR_DIR%\7z\7za.exe a "%ESP32_PATH%\LuatOS-SoC_%BSP_VERSION%_%PROJ_NAME%.soc" "%CUR_DIR%\*.*bin" "%CUR_DIR%\*.*json" "%CUR_DIR%\*.*exe"
del %CUR_DIR%\*.*bin
pause