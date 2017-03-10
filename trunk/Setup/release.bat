@echo off

set path=%path%;D:\Python27;

set DST_PATH=.\deploy_release
set RELEASE_PATH=..\..\Release
set TOOLS_PATH=.\tools

echo ==build_rev==
python %TOOLS_PATH%\build_rev.py ..\ %DST_PATH%\XenaManager\version.xml

REM zip tmp.zip
python %TOOLS_PATH%\zip_folder.py %DST_PATH%\XenaManager

REM rename tmp folder and tmp.zip to current version
python %TOOLS_PATH%\rename.py %DST_PATH%\XenaManager.zip %DST_PATH%\XenaManager\version.xml

move %DST_PATH%\*.zip %RELEASE_PATH%

pause