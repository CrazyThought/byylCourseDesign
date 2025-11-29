@echo off

REM 设置Qt安装路径（根据工作空间路径自动设置）
set QT_PATH=E:\Qt\Tools\QtCreator\bin

REM 添加Qt到系统路径
set PATH=%QT_PATH%;%PATH%

REM 检查qmake是否可用
qmake --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo 错误：未找到qmake命令。请检查Qt安装路径是否正确。
    echo 当前设置的Qt路径：%QT_PATH%
    pause
    exit /b 1
)

REM 创建构建目录（如果不存在）
if not exist "build" mkdir build

REM 进入构建目录
cd build

REM 生成Makefile
echo 正在生成Makefile...
qmake ..
if %ERRORLEVEL% NEQ 0 (
    echo 错误：qmake执行失败。
    pause
    exit /b 1
)

REM 编译项目
echo 正在编译项目...
mingw32-make
if %ERRORLEVEL% NEQ 0 (
    echo 错误：编译失败。
    pause
    exit /b 1
)

REM 运行项目
echo 正在运行项目...
start byylCD.exe

REM 返回到项目根目录
cd ..

echo 项目已成功运行！
pause