@echo off

chcp 65001 >nul

set READ_SN_SCPT=read-sn
set WRITE_SN_SCPT=write-sn
set "usage_str=用法：sn-operator.bat %READ_SN_SCPT% ^| %WRITE_SN_SCPT% sn"
if "%~1"=="" (
    echo %usage_str%
    pause
    exit /B
)

set SN_STR=
if "%~1"=="%WRITE_SN_SCPT%" (
    if "%~2"=="" ( 
	echo %usage_str%
	pause
	exit /B
    )
    set SN_STR="%~2"
)

call 00.dev-info.bat
call 00.key-file-info.bat

set SN_INFO_SCPT=sn-info
set OP_SCPT=%1

set REMOTE_PATH=/tmp

REM echo Uploading %SN_INFO_SCPT% and %OP_SCPT% to %DEVICE_USER%@%DEVICE_IP%:%REMOTE_PATH% ...
scp -O -i %AUTH_KEY_FILE_NAME% %SN_INFO_SCPT% %OP_SCPT% %DEVICE_USER%@%DEVICE_IP%:%REMOTE_PATH% >nul 2>&1
if errorlevel 1 (
    echo Upload failed!
    exit /b 1
)

REM echo Running %REMOTE_PATH%/%OP_SCPT% on device...
ssh -i %AUTH_KEY_FILE_NAME% %DEVICE_USER%@%DEVICE_IP% "chmod +x %REMOTE_PATH%/%OP_SCPT% %REMOTE_PATH%/%SN_INFO_SCPT% && %REMOTE_PATH%/%OP_SCPT% %SN_STR%"
