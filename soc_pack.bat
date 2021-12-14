@echo off

set ESP32_PATH=%cd%
set ESP32_BUILD_PATH=%ESP32_PATH%\build
set ESP32_BUILD_PATH=%ESP32_PATH%\build
set CUR_DIR=%cd%\soc_tools

set PROJ_NAME=ESP32-C3
set BSP_VERSION=V0001
set MERGE_FILE=luatos_esp32.bin
set PARTITION_ADDR= 0x8000
set CORE_ADDR= 0x10000

copy %ESP32_BUILD_PATH%\bootloader\bootloader.bin %CUR_DIR%\bootloader.bin
copy %ESP32_BUILD_PATH%\partition_table\partition-table.bin %CUR_DIR%\partition.bin
copy %ESP32_BUILD_PATH%\luatos_esp32.bin %CUR_DIR%\application.bin

for %%a in (%CUR_DIR%\bootloader.bin) do (set /a bl_rep_size=%PARTITION_ADDR%-%%~za)
fsutil file createnew %CUR_DIR%\bl_rep.bin %bl_rep_size%

type %CUR_DIR%\bootloader.bin >> %CUR_DIR%\boot.bin
type %CUR_DIR%\bl_rep.bin >> %CUR_DIR%\boot.bin
type %CUR_DIR%\partition.bin >> %CUR_DIR%\boot.bin
del %CUR_DIR%\bootloader.bin %CUR_DIR%\partition.bin %CUR_DIR%\bl_rep.bin

for %%a in (%CUR_DIR%\boot.bin) do (set /a boot_rep_size=%CORE_ADDR%-%%~za)
fsutil file createnew %CUR_DIR%\boot_rep.bin %boot_rep_size%

type %CUR_DIR%\boot.bin >> %CUR_DIR%\%MERGE_FILE%
type %CUR_DIR%\boot_rep.bin >> %CUR_DIR%\%MERGE_FILE%
type %CUR_DIR%\application.bin >> %CUR_DIR%\%MERGE_FILE%
del %CUR_DIR%\application.bin %CUR_DIR%\boot.bin %CUR_DIR%\boot_rep.bin

del %ESP32_PATH%\LuatOS-SoC_%BSP_VERSION%_%PROJ_NAME%.soc
%CUR_DIR%\7z\7za.exe a "%ESP32_PATH%\LuatOS-SoC_%BSP_VERSION%_%PROJ_NAME%.soc" "%CUR_DIR%\*.*bin" "%CUR_DIR%\*.*json" "%CUR_DIR%\*.*exe"
del %CUR_DIR%\*.*bin
pause