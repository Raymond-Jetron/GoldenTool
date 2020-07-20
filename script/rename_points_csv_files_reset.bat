rem 循环重命名一个文件夹下的所有csv文件，去掉前缀[OK]，表示未完成，用于重新同步
setlocal EnableDelayedExpansion
for /f "delims=" %%i in ('dir /b [OK]*.csv') do (
set var=%%i
set var=!var:[OK]=!
ren "%%i" "!var!"
)
