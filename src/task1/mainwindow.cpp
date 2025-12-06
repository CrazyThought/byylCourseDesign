#include "task1/mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_Task1Window(nullptr)
    , m_Task2Window(nullptr)
{
    ui->setupUi(this);
}

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

void MainWindow::on_actionExit_triggered()
{
    close();
}
