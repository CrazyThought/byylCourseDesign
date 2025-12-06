#include "task1/mainwindow.h"

#include <QApplication>

// Console application main function
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
