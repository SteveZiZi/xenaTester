rem
rem 部署应用程序, 按目录安排好, 
rem 需要先编译好工程, 最后执行这个部署
rem

rem
rem 建立部署的目录
rem
del .\deploy_release\*.* /s /q
rmdir .\deploy_release /s /q
md .\deploy_release

md .\deploy_release\XenaManager


rem
rem 拷贝运行时目录结构, 包括所有配置文件
rem
copy ..\version.xml               .\deploy_release\XenaManager
copy ..\change.log                .\deploy_release\XenaManager

copy ..\bin\licencesLib.dll       .\deploy_release\XenaManager  
copy ..\bin\sclManager.dll        .\deploy_release\XenaManager 
copy ..\bin\XenaManager.exe       .\deploy_release\XenaManager 

xcopy ..\runtime\*.*              .\deploy_release\XenaManager\ /e /y

rem
rem  拷贝QT运行时库
rem
set QT_DISK=D:
set QT_VERSION=4.8.1
set QT_DIR=%QT_DISK%\Qt\%QT_VERSION%\bin
copy %QT_DIR%\QtCore4.dll         .\deploy_release\XenaManager
copy %QT_DIR%\QtGui4.dll          .\deploy_release\XenaManager
copy %QT_DIR%\QtNetwork4.dll      .\deploy_release\XenaManager
copy %QT_DIR%\QtXml4.dll          .\deploy_release\XenaManager

