@echo ��������

@echo off
REM win,el6,el7,ubuntu16
set os_type=win
echo os_type = %os_type%

REM x86_64
set cpu=x86_64
echo cpu = %cpu%

REM Major.Minor.Patch
set version=1.0.8
echo version = %version%

echo ==============================
REM ����HistoryDataSync
call publish-HistoryDataSync.bat %os_type% %cpu% %version%

echo ==============================
REM ����MakeData
call publish-MakeData.bat %os_type% %cpu% %version%

echo ==============================
REM ����QueryData
call publish-QueryData.bat %os_type% %cpu% %version%
 
echo �������
 
pause
