#!/bin/bash

echo "物联网设备监控系统构建脚本"
echo "================================"

# 检查Qt环境
if ! command -v qmake &> /dev/null; then
    echo "错误: 未找到qmake，请确保Qt环境已正确安装并添加到PATH"
    exit 1
fi

echo "清理之前的构建文件..."
rm -f Makefile Makefile.Debug Makefile.Release
rm -f moc_*.cpp ui_*.h qrc_*.cpp
rm -f *.o
rm -rf debug release

echo "生成Makefile..."
qmake InternetMonitoring.pro

if [ $? -ne 0 ]; then
    echo "错误: qmake执行失败"
    exit 1
fi

echo "编译项目..."
make

if [ $? -ne 0 ]; then
    echo "错误: 编译失败"
    exit 1
fi

echo "构建完成！"
echo "可执行文件位置: InternetMonitoring"

# 询问是否运行程序
read -p "是否立即运行程序？(y/n): " choice
if [[ $choice == "y" || $choice == "Y" ]]; then
    if [ -f "InternetMonitoring" ]; then
        echo "运行程序..."
        ./InternetMonitoring
    else
        echo "错误: 未找到可执行文件"
    fi
fi 