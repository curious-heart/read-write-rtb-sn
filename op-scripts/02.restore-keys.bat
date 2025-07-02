@echo off

call 00.rmt-op-info.bat

REM run auth-restore script remotely
ssh -i %LOCAL_WORK_DIR%\%AUTH_KEY_FILE_NAME% %DEVICE_USER%@%DEVICE_IP% "%REMOTE_TMP_DIR%/%RESTORE_AUTH_SCPT%"
