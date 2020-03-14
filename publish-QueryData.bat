@echo off
set app_name=QueryData
REM win,el6,el7,ubuntu16
set os_type=win
REM x86_64,arm
set cpu=x86_64
REM user define
set version=1.0.6
REM user define
set publish_root=publish

REM 不同程序公共部分
REM =================================
echo Publish %app_name%

REM 不同系统不同文件后缀
if "%os_type%" == "win" (
set exe_name=%app_name%
set test_file_extension=bat
) else (
set exe_name=%app_name%
set test_file_extension=sh
)

REM 清空发布目录
set publish_path=%publish_root%\%app_name%-%version%-%os_type%-%cpu%
echo %publish_path%
echo clear publish path
rd /s /q %publish_path%
md %publish_path%
echo .

REM 复制测试脚本
echo copy test
md %publish_path%\test
xcopy /y /s /e test\%app_name%*.%test_file_extension% %publish_path%\test
echo .

REM 复制执行程序
if "%os_type%" == "win" (
echo copy %exe_name%
md %publish_path%\%exe_name%\x64
md %publish_path%\%exe_name%\x86
xcopy /y /s /e bin\%exe_name%\x64_win\Release\%exe_name%.exe %publish_path%\%exe_name%\x64
xcopy /y /s /e bin\%exe_name%\x64_win\Release\rtdbapi64.dll %publish_path%\%exe_name%\x64
xcopy /y /s /e depend\win %publish_path%\%exe_name%\x64
xcopy /y /s /e bin\%exe_name%\x86_win\Release\%exe_name%.exe %publish_path%\%exe_name%\x86
xcopy /y /s /e bin\%exe_name%\x86_win\Release\rtdbapi.dll %publish_path%\%exe_name%\x86
xcopy /y /s /e depend\win %publish_path%\%exe_name%\x86
echo .

) else (
echo copy %exe_name%
md %publish_path%\%exe_name%\x64
md %publish_path%\%exe_name%\x86
xcopy /y /s /e %exe_name%\bin\x64\Release %publish_path%\%exe_name%\x64
xcopy /y /s /e %exe_name%\bin\x86\Release %publish_path%\%exe_name%\x86
echo .

)

REM 复制README
echo copy README
copy /y README\%app_name%\README.md %publish_path%\README.md
echo .

REM =================================


REM 不同程序自定义内容
REM =================================
REM 创建script目录
echo copy script
md %publish_path%\script
xcopy /y /s /e script %publish_path%\script
echo .

REM 创建result目录
echo create dir result
md %publish_path%\result
echo .

REM =================================

echo Complete!

pause