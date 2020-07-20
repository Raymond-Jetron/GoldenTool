rem 循环重命名一个文件夹下的所有csv文件，加上前缀[OK]，表示全部完成
setlocal EnableDelayedExpansion
for /f "delims=" %%i in ('dir /b *.csv') do (
set var=%%i
set var=[OK]!var!
ren "%%i" "!var!"
)