rem ѭ��������һ���ļ����µ�����csv�ļ�������ǰ׺[OK]����ʾȫ�����
setlocal EnableDelayedExpansion
for /f "delims=" %%i in ('dir /b *.csv') do (
set var=%%i
set var=[OK]!var!
ren "%%i" "!var!"
)