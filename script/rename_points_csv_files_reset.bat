rem ѭ��������һ���ļ����µ�����csv�ļ���ȥ��ǰ׺[OK]����ʾδ��ɣ���������ͬ��
setlocal EnableDelayedExpansion
for /f "delims=" %%i in ('dir /b [OK]*.csv') do (
set var=%%i
set var=!var:[OK]=!
ren "%%i" "!var!"
)
