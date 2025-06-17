@echo off
echo 物联网设备监控系统构建脚本
echo ================================

REM 检查Qt环境
where qmake >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误: 未找到qmake，请确保Qt环境已正确安装并添加到PATH
    pause
    exit /b 1
)

echo 清理之前的构建文件...
if exist Makefile del Makefile
if exist Makefile.Debug del Makefile.Debug
if exist Makefile.Release del Makefile.Release
if exist moc_*.cpp del moc_*.cpp
if exist ui_*.h del ui_*.h
if exist qrc_*.cpp del qrc_*.cpp
if exist *.o del *.o
if exist debug\*.exe del debug\*.exe
if exist release\*.exe del release\*.exe

echo 生成Makefile...
qmake InternetMonitoring.pro

if %errorlevel% neq 0 (
    echo 错误: qmake执行失败
    pause
    exit /b 1
)

echo 编译项目...
nmake

if %errorlevel% neq 0 (
    echo 错误: 编译失败
    pause
    exit /b 1
)

echo 构建完成！
echo 可执行文件位置: debug\InternetMonitoring.exe 或 release\InternetMonitoring.exe

REM 询问是否运行程序
set /p choice="是否立即运行程序？(y/n): "
if /i "%choice%"=="y" (
    if exist debug\InternetMonitoring.exe (
        echo 运行程序...
        debug\InternetMonitoring.exe
    ) else if exist release\InternetMonitoring.exe (
        echo 运行程序...
        release\InternetMonitoring.exe
    ) else (
        echo 错误: 未找到可执行文件
    )
)

pause 