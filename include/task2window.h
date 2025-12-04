#ifndef TASK2WINDOW_H
#define TASK2WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Task2Window; }
QT_END_NAMESPACE

class Task2Window : public QMainWindow
{
    Q_OBJECT

public:
    Task2Window(QWidget *parent = nullptr);
    ~Task2Window();

private slots:
    void on_actionExit_triggered();

private:
    Ui::Task2Window *ui;
};

#endif // TASK2WINDOW_H
