cd /d %~dp0
xcopy LIB\*.dll SRC\

xcopy GCC\*.dll SRC\

pyinstaller -F -w SRC\ota_package_tool.py

xcopy LIB\Qt5Core.dll dist\
xcopy SRC\UpgradeDesc_sample.json dist\