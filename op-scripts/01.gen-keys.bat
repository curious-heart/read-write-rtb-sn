@echo off

REM del known_hosts file to avoid error msg in ssh cmd.
del /f /q "%USERPROFILE%\.ssh\known_hosts" 2>nul

call 00.rmt-op-info.bat
echo -------------
echo REMOTE_SCPTS_WITH_RMT_DIR: %REMOTE_SCPTS_WITH_RMT_DIR%
echo FILES_SENT_TO_RMT_WITH_LOCAL_DIR: %FILES_SENT_TO_RMT_WITH_LOCAL_DIR%

del /f /q %LOCAL_WORK_DIR%\%AUTH_KEY_FILE_NAME% %LOCAL_WORK_DIR%\%AUTH_PUB_KEY_FILE_NAME%

REM gen pub-priv keys pair.
ssh-keygen -t %AUTH_KEY_TYPE% -f %LOCAL_WORK_DIR%\%AUTH_KEY_FILE_NAME% -N "" -q

REM transport pub key and auth-update script to remote machine.
scp -O %FILES_SENT_TO_RMT_WITH_LOCAL_DIR% %DEVICE_USER%@%DEVICE_IP%:%REMOTE_TMP_DIR%


REM run auth-update script remotely
ssh %DEVICE_USER%@%DEVICE_IP% "chmod +x %REMOTE_SCPTS_WITH_RMT_DIR% && %REMOTE_TMP_DIR%/%UPD_AUTH_SCPT%"

