@echo ��ʼ������
 
@set OldPath=%cd%
 
::x86_amd64��·��Ҫ�Լ���
 
@cd /d C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build
 
@if not %errorlevel% == 0 goto :end
 
 
 
call vcvarsx86_amd64.bat
 
@if not %errorlevel% == 0 goto :end
 
 
 
cd /d %OldPath%
 
@if not %errorlevel% == 0 goto :end
 
 
 
@echo ������Ŀ
 
devenv ./RTDBToolW.sln /Clean
 
@if not %errorlevel% == 0 goto :errBuild
 
 
 
@echo ��ʼ����
 
devenv ./RTDBToolW.sln /ReBuild "Release|x86"
 
@if not %errorlevel% == 0 goto :errBuild
 
 
 
devenv ./RTDBToolW.sln /ReBuild "Release|x64"
 
@if not %errorlevel% == 0 goto :errBuild
 
 
 
@rem ͷ�ļ�
 
::�����Ҫ�����ļ��Ϳ���copy����
 
::�����Ҫɾ���ļ��Ϳ���del����
 
@echo ͬ�����
 
@pause
 
goto :end
 
@pause
 
:errBuild
 
@echo ������Ŀ����
 
@pause
 
:end
