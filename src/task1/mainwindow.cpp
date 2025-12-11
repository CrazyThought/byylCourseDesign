/*
 * @file mainwindow.cpp
 * @id mainwindow-cpp
 * @brief 主窗口实现文件，用于管理应用程序的主要界面和任务切换
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */

#include "task1/mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

/**
 * @brief MainWindow构造函数
 * @param parent 父窗口指针
 * @details 初始化主窗口UI和任务窗口指针
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_Task1Window(nullptr)
    , m_Task2Window(nullptr)
{
    ui->setupUi(this);
}

/**
 * @brief MainWindow析构函数
 * @details 释放UI资源和任务窗口资源
 */
MainWindow::~MainWindow()
{
    delete ui;
    if (m_Task1Window) {
        delete m_Task1Window;
    }
    if (m_Task2Window) {
        delete m_Task2Window;
    }
}

/**
 * @brief 任务1按钮点击事件处理函数
 * @details 创建并显示任务1窗口，用于词法分析器相关功能
 */
void MainWindow::on_btnTask1_clicked()
{
    if (!m_Task1Window) {
        m_Task1Window = new Task1Window(this);
        m_Task1Window->setWindowTitle("Task 1");
    }
    m_Task1Window->show();
    m_Task1Window->raise();
    m_Task1Window->activateWindow();
}

/**
 * @brief 任务2按钮点击事件处理函数
 * @details 创建并显示任务2窗口，用于语法分析器相关功能
 */
void MainWindow::on_btnTask2_clicked()
{
    if (!m_Task2Window) {
        m_Task2Window = new Task2Window(this);
        m_Task2Window->setWindowTitle("Task 2");
    }
    m_Task2Window->show();
    m_Task2Window->raise();
    m_Task2Window->activateWindow();
}

/**
 * @brief 退出菜单项触发事件处理函数
 * @details 关闭应用程序主窗口
 */
void MainWindow::on_actionExit_triggered()
{
    close();
}
