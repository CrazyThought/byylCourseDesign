/********************************************************************************
** Form generated from reading UI file 'task2window.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TASK2WINDOW_H
#define UI_TASK2WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Task2Window
{
public:
    QAction *actionExit;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTextBrowser *textBrowser;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Task2Window)
    {
        if (Task2Window->objectName().isEmpty())
            Task2Window->setObjectName("Task2Window");
        Task2Window->resize(1000, 800);
        actionExit = new QAction(Task2Window);
        actionExit->setObjectName("actionExit");
        centralwidget = new QWidget(Task2Window);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);

        textBrowser = new QTextBrowser(centralwidget);
        textBrowser->setObjectName("textBrowser");

        verticalLayout->addWidget(textBrowser);

        Task2Window->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Task2Window);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1000, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        Task2Window->setMenuBar(menubar);
        statusbar = new QStatusBar(Task2Window);
        statusbar->setObjectName("statusbar");
        Task2Window->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionExit);

        retranslateUi(Task2Window);

        QMetaObject::connectSlotsByName(Task2Window);
    } // setupUi

    void retranslateUi(QMainWindow *Task2Window)
    {
        Task2Window->setWindowTitle(QCoreApplication::translate("Task2Window", "\345\256\236\351\252\214\344\272\214 - \346\234\252\346\235\245\346\211\251\345\261\225", nullptr));
        actionExit->setText(QCoreApplication::translate("Task2Window", "\351\200\200\345\207\272", nullptr));
        label->setText(QCoreApplication::translate("Task2Window", "\345\256\236\351\252\214\344\272\214 - \346\234\252\346\235\245\346\211\251\345\261\225", nullptr));
        textBrowser->setHtml(QCoreApplication::translate("Task2Window", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
" p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">\346\255\244\347\252\227\345\217\243\347\224\250\344\272\216\345\256\236\351\252\214\344\272\214\347\232\204\346\211\251\345\261\225\343\200\202</p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">\345\217\257\344\273\245\345\234\250\346\255\244\346\267\273\345\212\240\346\226\260\347\232\204\346\240\207\347\255\276\351\241\265\345\222\214\345\212\237\350\203\275\343\200\202</p></body></html>", nullptr));
        menuFile->setTitle(QCoreApplication::translate("Task2Window", "\346\226\207\344\273\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Task2Window: public Ui_Task2Window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TASK2WINDOW_H
