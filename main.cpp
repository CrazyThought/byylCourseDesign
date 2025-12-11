/*
 * @file main.cpp
 * @id main-cpp
 * @brief 应用程序入口文件
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */

#include "task1/mainwindow.h"

#include <QApplication>

/**
 * @brief 应用程序入口函数
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 应用程序退出状态码
 * @details 创建Qt应用程序实例，初始化主窗口并显示，进入事件循环
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // 创建Qt应用程序实例
    MainWindow w; // 创建主窗口实例
    w.show(); // 显示主窗口
    return a.exec(); // 进入事件循环，等待用户交互
}
