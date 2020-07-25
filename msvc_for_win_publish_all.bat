@echo 批量发布

@echo off
REM win,el6,el7,ubuntu16
set os_type=win
echo os_type = %os_type%

REM x86_64
set cpu=x86_64
echo cpu = %cpu%

REM Major.Minor.Patch
set version=1.0.9
echo version = %version%

echo ==============================
REM 发布HistoryDataSync
call publish-HistoryDataSync.bat %os_type% %cpu% %version%

echo ==============================
REM 发布MakeData
call publish-MakeData.bat %os_type% %cpu% %version%

echo ==============================
REM 发布QueryData
call publish-QueryData.bat %os_type% %cpu% %version%
 
echo 发布完成
 
pause
