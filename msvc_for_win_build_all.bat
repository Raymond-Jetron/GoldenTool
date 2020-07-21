@echo 初始化环境
 
@set OldPath=%cd%
 
::x86_amd64的路径要自己找
 
@cd /d C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build
 
@if not %errorlevel% == 0 goto :end
 
 
 
call vcvarsx86_amd64.bat
 
@if not %errorlevel% == 0 goto :end
 
 
 
cd /d %OldPath%
 
@if not %errorlevel% == 0 goto :end
 
 
 
@echo 清理项目
 
devenv ./RTDBToolW.sln /Clean
 
@if not %errorlevel% == 0 goto :errBuild
 
 
 
@echo 开始编译
 
devenv ./RTDBToolW.sln /ReBuild "Release|x86"
 
@if not %errorlevel% == 0 goto :errBuild
 
 
 
devenv ./RTDBToolW.sln /ReBuild "Release|x64"
 
@if not %errorlevel% == 0 goto :errBuild
 
 
 
@rem 头文件
 
::如果需要拷贝文件就看下copy命令
 
::如果需要删除文件就看下del命令
 
@echo 同步完成
 
@pause
 
goto :end
 
@pause
 
:errBuild
 
@echo 编译项目出错
 
@pause
 
:end
