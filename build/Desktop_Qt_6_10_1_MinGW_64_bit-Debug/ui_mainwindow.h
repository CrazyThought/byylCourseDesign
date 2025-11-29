/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpenRegex;
    QAction *actionSaveRegex;
    QAction *actionExit;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tabRegex;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *textEditRegex;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnOpenRegex;
    QPushButton *btnSaveRegex;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnParseRegex;
    QWidget *tabNFA;
    QVBoxLayout *verticalLayout_3;
    QTableWidget *tableNFA;
    QPushButton *btnGenerateNFA;
    QWidget *tabDFA;
    QVBoxLayout *verticalLayout_4;
    QTableWidget *tableDFA;
    QPushButton *btnGenerateDFA;
    QWidget *tabMinDFA;
    QVBoxLayout *verticalLayout_5;
    QTableWidget *tableMinDFA;
    QPushButton *btnMinimizeDFA;
    QWidget *tabLexer;
    QVBoxLayout *verticalLayout_6;
    QTextEdit *textEditLexerCode;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btnGenerateLexer;
    QPushButton *btnSaveLexer;
    QWidget *tabTest;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_3;
    QTextEdit *textEditTestInput;
    QTableWidget *tableTestOutput;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *btnOpenTestFile;
    QPushButton *btnRunLexer;
    QPushButton *btnSaveTestOutput;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1000, 800);
        actionOpenRegex = new QAction(MainWindow);
        actionOpenRegex->setObjectName("actionOpenRegex");
        actionSaveRegex = new QAction(MainWindow);
        actionSaveRegex->setObjectName("actionSaveRegex");
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabRegex = new QWidget();
        tabRegex->setObjectName("tabRegex");
        verticalLayout_2 = new QVBoxLayout(tabRegex);
        verticalLayout_2->setObjectName("verticalLayout_2");
        textEditRegex = new QTextEdit(tabRegex);
        textEditRegex->setObjectName("textEditRegex");

        verticalLayout_2->addWidget(textEditRegex);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        btnOpenRegex = new QPushButton(tabRegex);
        btnOpenRegex->setObjectName("btnOpenRegex");

        horizontalLayout->addWidget(btnOpenRegex);

        btnSaveRegex = new QPushButton(tabRegex);
        btnSaveRegex->setObjectName("btnSaveRegex");

        horizontalLayout->addWidget(btnSaveRegex);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnParseRegex = new QPushButton(tabRegex);
        btnParseRegex->setObjectName("btnParseRegex");

        horizontalLayout->addWidget(btnParseRegex);


        verticalLayout_2->addLayout(horizontalLayout);

        tabWidget->addTab(tabRegex, QString());
        tabNFA = new QWidget();
        tabNFA->setObjectName("tabNFA");
        verticalLayout_3 = new QVBoxLayout(tabNFA);
        verticalLayout_3->setObjectName("verticalLayout_3");
        tableNFA = new QTableWidget(tabNFA);
        if (tableNFA->columnCount() < 3)
            tableNFA->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableNFA->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableNFA->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableNFA->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tableNFA->setObjectName("tableNFA");
        tableNFA->setAlternatingRowColors(true);

        verticalLayout_3->addWidget(tableNFA);

        btnGenerateNFA = new QPushButton(tabNFA);
        btnGenerateNFA->setObjectName("btnGenerateNFA");

        verticalLayout_3->addWidget(btnGenerateNFA);

        tabWidget->addTab(tabNFA, QString());
        tabDFA = new QWidget();
        tabDFA->setObjectName("tabDFA");
        verticalLayout_4 = new QVBoxLayout(tabDFA);
        verticalLayout_4->setObjectName("verticalLayout_4");
        tableDFA = new QTableWidget(tabDFA);
        if (tableDFA->columnCount() < 3)
            tableDFA->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableDFA->setHorizontalHeaderItem(0, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableDFA->setHorizontalHeaderItem(1, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableDFA->setHorizontalHeaderItem(2, __qtablewidgetitem5);
        tableDFA->setObjectName("tableDFA");
        tableDFA->setAlternatingRowColors(true);

        verticalLayout_4->addWidget(tableDFA);

        btnGenerateDFA = new QPushButton(tabDFA);
        btnGenerateDFA->setObjectName("btnGenerateDFA");

        verticalLayout_4->addWidget(btnGenerateDFA);

        tabWidget->addTab(tabDFA, QString());
        tabMinDFA = new QWidget();
        tabMinDFA->setObjectName("tabMinDFA");
        verticalLayout_5 = new QVBoxLayout(tabMinDFA);
        verticalLayout_5->setObjectName("verticalLayout_5");
        tableMinDFA = new QTableWidget(tabMinDFA);
        if (tableMinDFA->columnCount() < 3)
            tableMinDFA->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableMinDFA->setHorizontalHeaderItem(0, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableMinDFA->setHorizontalHeaderItem(1, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableMinDFA->setHorizontalHeaderItem(2, __qtablewidgetitem8);
        tableMinDFA->setObjectName("tableMinDFA");
        tableMinDFA->setAlternatingRowColors(true);

        verticalLayout_5->addWidget(tableMinDFA);

        btnMinimizeDFA = new QPushButton(tabMinDFA);
        btnMinimizeDFA->setObjectName("btnMinimizeDFA");

        verticalLayout_5->addWidget(btnMinimizeDFA);

        tabWidget->addTab(tabMinDFA, QString());
        tabLexer = new QWidget();
        tabLexer->setObjectName("tabLexer");
        verticalLayout_6 = new QVBoxLayout(tabLexer);
        verticalLayout_6->setObjectName("verticalLayout_6");
        textEditLexerCode = new QTextEdit(tabLexer);
        textEditLexerCode->setObjectName("textEditLexerCode");

        verticalLayout_6->addWidget(textEditLexerCode);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        btnGenerateLexer = new QPushButton(tabLexer);
        btnGenerateLexer->setObjectName("btnGenerateLexer");

        horizontalLayout_2->addWidget(btnGenerateLexer);

        btnSaveLexer = new QPushButton(tabLexer);
        btnSaveLexer->setObjectName("btnSaveLexer");

        horizontalLayout_2->addWidget(btnSaveLexer);


        verticalLayout_6->addLayout(horizontalLayout_2);

        tabWidget->addTab(tabLexer, QString());
        tabTest = new QWidget();
        tabTest->setObjectName("tabTest");
        verticalLayout_7 = new QVBoxLayout(tabTest);
        verticalLayout_7->setObjectName("verticalLayout_7");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        textEditTestInput = new QTextEdit(tabTest);
        textEditTestInput->setObjectName("textEditTestInput");
        textEditTestInput->setMaximumSize(QSize(400, 16777215));

        horizontalLayout_3->addWidget(textEditTestInput);

        tableTestOutput = new QTableWidget(tabTest);
        if (tableTestOutput->columnCount() < 3)
            tableTestOutput->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableTestOutput->setHorizontalHeaderItem(0, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableTestOutput->setHorizontalHeaderItem(1, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableTestOutput->setHorizontalHeaderItem(2, __qtablewidgetitem11);
        tableTestOutput->setObjectName("tableTestOutput");
        tableTestOutput->setAlternatingRowColors(true);

        horizontalLayout_3->addWidget(tableTestOutput);


        verticalLayout_7->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        btnOpenTestFile = new QPushButton(tabTest);
        btnOpenTestFile->setObjectName("btnOpenTestFile");

        horizontalLayout_4->addWidget(btnOpenTestFile);

        btnRunLexer = new QPushButton(tabTest);
        btnRunLexer->setObjectName("btnRunLexer");

        horizontalLayout_4->addWidget(btnRunLexer);

        btnSaveTestOutput = new QPushButton(tabTest);
        btnSaveTestOutput->setObjectName("btnSaveTestOutput");

        horizontalLayout_4->addWidget(btnSaveTestOutput);


        verticalLayout_7->addLayout(horizontalLayout_4);

        tabWidget->addTab(tabTest, QString());

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1000, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpenRegex);
        menuFile->addAction(actionSaveRegex);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\350\257\215\346\263\225\345\210\206\346\236\220\345\231\250\347\224\237\346\210\220\345\231\250", nullptr));
        actionOpenRegex->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217", nullptr));
        actionSaveRegex->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272", nullptr));
        btnOpenRegex->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200", nullptr));
        btnSaveRegex->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        btnParseRegex->setText(QCoreApplication::translate("MainWindow", "\350\247\243\346\236\220\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabRegex), QCoreApplication::translate("MainWindow", "\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableNFA->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "\347\212\266\346\200\201", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableNFA->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\345\255\227\347\254\246", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableNFA->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "\347\233\256\346\240\207\347\212\266\346\200\201", nullptr));
        btnGenerateNFA->setText(QCoreApplication::translate("MainWindow", "\347\224\237\346\210\220NFA", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabNFA), QCoreApplication::translate("MainWindow", "NFA", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableDFA->horizontalHeaderItem(0);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "\347\212\266\346\200\201\351\233\206\345\220\210", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableDFA->horizontalHeaderItem(1);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\345\255\227\347\254\246", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableDFA->horizontalHeaderItem(2);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "\347\233\256\346\240\207\347\212\266\346\200\201\351\233\206\345\220\210", nullptr));
        btnGenerateDFA->setText(QCoreApplication::translate("MainWindow", "\347\224\237\346\210\220DFA", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabDFA), QCoreApplication::translate("MainWindow", "DFA", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tableMinDFA->horizontalHeaderItem(0);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "\347\212\266\346\200\201", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tableMinDFA->horizontalHeaderItem(1);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\345\255\227\347\254\246", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tableMinDFA->horizontalHeaderItem(2);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "\347\233\256\346\240\207\347\212\266\346\200\201", nullptr));
        btnMinimizeDFA->setText(QCoreApplication::translate("MainWindow", "\346\234\200\345\260\217\345\214\226DFA", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabMinDFA), QCoreApplication::translate("MainWindow", "\346\234\200\345\260\217\345\214\226DFA", nullptr));
        btnGenerateLexer->setText(QCoreApplication::translate("MainWindow", "\347\224\237\346\210\220\350\257\215\346\263\225\345\210\206\346\236\220\345\231\250", nullptr));
        btnSaveLexer->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\344\270\272.cpp", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabLexer), QCoreApplication::translate("MainWindow", "\350\257\215\346\263\225\345\210\206\346\236\220\345\231\250", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = tableTestOutput->horizontalHeaderItem(0);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("MainWindow", "\350\241\214\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tableTestOutput->horizontalHeaderItem(1);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("MainWindow", "\345\215\225\350\257\215", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = tableTestOutput->horizontalHeaderItem(2);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("MainWindow", "\347\274\226\347\240\201", nullptr));
        btnOpenTestFile->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\346\265\213\350\257\225\346\226\207\344\273\266", nullptr));
        btnRunLexer->setText(QCoreApplication::translate("MainWindow", "\350\277\220\350\241\214\350\257\215\346\263\225\345\210\206\346\236\220", nullptr));
        btnSaveTestOutput->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\347\273\223\346\236\234", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabTest), QCoreApplication::translate("MainWindow", "\346\265\213\350\257\225", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
