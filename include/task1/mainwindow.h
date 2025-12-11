/*
 * @file mainwindow.h
 * @id mainwindow-h
 * @brief 主窗口类，用于管理和切换任务1和任务2的窗口
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "task1/task1window.h"
#include "task2/task2window.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief 主窗口类
 * 
 * 应用程序的主窗口，用于管理任务1和任务2的窗口切换
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * 
     * @param parent 父窗口指针，默认为nullptr
     */
    MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~MainWindow();

private slots:
    /**
     * @brief 任务1按钮点击事件槽
     * 
     * 处理任务1按钮的点击事件，打开任务1窗口
     */
    void on_btnTask1_clicked();
    
    /**
     * @brief 任务2按钮点击事件槽
     * 
     * 处理任务2按钮的点击事件，打开任务2窗口
     */
    void on_btnTask2_clicked();
    
    /**
     * @brief 退出菜单项触发事件槽
     * 
     * 处理退出菜单项的触发事件，关闭应用程序
     */
    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;               ///< UI界面指针
    Task1Window *m_Task1Window;       ///< 任务1窗口指针
    Task2Window *m_Task2Window;       ///< 任务2窗口指针
};

#endif // MAINWINDOW_H
