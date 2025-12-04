#include "task2window.h"
#include "ui_task2window.h"
#include <QMessageBox>

Task2Window::Task2Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Task2Window)
{
    ui->setupUi(this);
    setWindowTitle(tr("实验二 - 未来扩展"));
}

Task2Window::~Task2Window()
{
    delete ui;
}

void Task2Window::on_actionExit_triggered()
{
    close();
}
