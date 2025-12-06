#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "task1/task1window.h"
#include "task2/task2window.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnTask1_clicked();
    void on_btnTask2_clicked();
    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
    Task1Window *m_Task1Window;
    Task2Window *m_Task2Window;
};

#endif // MAINWINDOW_H
