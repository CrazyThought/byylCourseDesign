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
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
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

class Ui_Task2Window
{
public:
    QAction *actionExit;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tabRegexInput;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox_Input;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *textEditRegexInput;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButtonParse;
    QPushButton *pushButtonLoadExample;
    QPushButton *pushButtonClear;
    QGroupBox *groupBox_ParseResult;
    QVBoxLayout *verticalLayout_4;
    QTableWidget *tableWidgetParseResult;
    QLabel *labelErrorMessage;
    QWidget *tabNFA;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButtonBuildNFA;
    QPushButton *pushButtonExportNFA;
    QSpacerItem *horizontalSpacer;
    QLabel *labelNFAInfo;
    QTableWidget *tableWidgetNFA;
    QWidget *tabDFA;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButtonConvertDFA;
    QPushButton *pushButtonExportDFA;
    QSpacerItem *horizontalSpacer_2;
    QLabel *labelDFAInfo;
    QTableWidget *tableWidgetDFA;
    QWidget *tabMinDFA;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButtonMinimizeDFA;
    QPushButton *pushButtonExportMinDFA;
    QSpacerItem *horizontalSpacer_3;
    QLabel *labelMinDFAInfo;
    QTableWidget *tableWidgetMinDFA;
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
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabRegexInput = new QWidget();
        tabRegexInput->setObjectName("tabRegexInput");
        verticalLayout_2 = new QVBoxLayout(tabRegexInput);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        groupBox_Input = new QGroupBox(tabRegexInput);
        groupBox_Input->setObjectName("groupBox_Input");
        verticalLayout_3 = new QVBoxLayout(groupBox_Input);
        verticalLayout_3->setObjectName("verticalLayout_3");
        textEditRegexInput = new QTextEdit(groupBox_Input);
        textEditRegexInput->setObjectName("textEditRegexInput");
        QFont font;
        font.setFamilies({QString::fromUtf8("Courier New")});
        font.setPointSize(10);
        textEditRegexInput->setFont(font);

        verticalLayout_3->addWidget(textEditRegexInput);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        pushButtonParse = new QPushButton(groupBox_Input);
        pushButtonParse->setObjectName("pushButtonParse");

        horizontalLayout_2->addWidget(pushButtonParse);

        pushButtonLoadExample = new QPushButton(groupBox_Input);
        pushButtonLoadExample->setObjectName("pushButtonLoadExample");

        horizontalLayout_2->addWidget(pushButtonLoadExample);

        pushButtonClear = new QPushButton(groupBox_Input);
        pushButtonClear->setObjectName("pushButtonClear");

        horizontalLayout_2->addWidget(pushButtonClear);


        verticalLayout_3->addLayout(horizontalLayout_2);


        horizontalLayout->addWidget(groupBox_Input);

        groupBox_ParseResult = new QGroupBox(tabRegexInput);
        groupBox_ParseResult->setObjectName("groupBox_ParseResult");
        verticalLayout_4 = new QVBoxLayout(groupBox_ParseResult);
        verticalLayout_4->setObjectName("verticalLayout_4");
        tableWidgetParseResult = new QTableWidget(groupBox_ParseResult);
        if (tableWidgetParseResult->columnCount() < 4)
            tableWidgetParseResult->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidgetParseResult->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidgetParseResult->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidgetParseResult->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidgetParseResult->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        tableWidgetParseResult->setObjectName("tableWidgetParseResult");
        tableWidgetParseResult->setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
        tableWidgetParseResult->setColumnCount(4);
        tableWidgetParseResult->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_4->addWidget(tableWidgetParseResult);

        labelErrorMessage = new QLabel(groupBox_ParseResult);
        labelErrorMessage->setObjectName("labelErrorMessage");

        verticalLayout_4->addWidget(labelErrorMessage);


        horizontalLayout->addWidget(groupBox_ParseResult);


        verticalLayout_2->addLayout(horizontalLayout);

        tabWidget->addTab(tabRegexInput, QString());
        tabNFA = new QWidget();
        tabNFA->setObjectName("tabNFA");
        verticalLayout_5 = new QVBoxLayout(tabNFA);
        verticalLayout_5->setObjectName("verticalLayout_5");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        pushButtonBuildNFA = new QPushButton(tabNFA);
        pushButtonBuildNFA->setObjectName("pushButtonBuildNFA");

        horizontalLayout_3->addWidget(pushButtonBuildNFA);

        pushButtonExportNFA = new QPushButton(tabNFA);
        pushButtonExportNFA->setObjectName("pushButtonExportNFA");

        horizontalLayout_3->addWidget(pushButtonExportNFA);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        labelNFAInfo = new QLabel(tabNFA);
        labelNFAInfo->setObjectName("labelNFAInfo");

        horizontalLayout_3->addWidget(labelNFAInfo);


        verticalLayout_5->addLayout(horizontalLayout_3);

        tableWidgetNFA = new QTableWidget(tabNFA);
        if (tableWidgetNFA->columnCount() < 2)
            tableWidgetNFA->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidgetNFA->setHorizontalHeaderItem(0, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidgetNFA->setHorizontalHeaderItem(1, __qtablewidgetitem5);
        tableWidgetNFA->setObjectName("tableWidgetNFA");
        tableWidgetNFA->setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
        tableWidgetNFA->setColumnCount(2);
        tableWidgetNFA->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_5->addWidget(tableWidgetNFA);

        tabWidget->addTab(tabNFA, QString());
        tabDFA = new QWidget();
        tabDFA->setObjectName("tabDFA");
        verticalLayout_6 = new QVBoxLayout(tabDFA);
        verticalLayout_6->setObjectName("verticalLayout_6");
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        pushButtonConvertDFA = new QPushButton(tabDFA);
        pushButtonConvertDFA->setObjectName("pushButtonConvertDFA");

        horizontalLayout_4->addWidget(pushButtonConvertDFA);

        pushButtonExportDFA = new QPushButton(tabDFA);
        pushButtonExportDFA->setObjectName("pushButtonExportDFA");

        horizontalLayout_4->addWidget(pushButtonExportDFA);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        labelDFAInfo = new QLabel(tabDFA);
        labelDFAInfo->setObjectName("labelDFAInfo");

        horizontalLayout_4->addWidget(labelDFAInfo);


        verticalLayout_6->addLayout(horizontalLayout_4);

        tableWidgetDFA = new QTableWidget(tabDFA);
        if (tableWidgetDFA->columnCount() < 2)
            tableWidgetDFA->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidgetDFA->setHorizontalHeaderItem(0, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableWidgetDFA->setHorizontalHeaderItem(1, __qtablewidgetitem7);
        tableWidgetDFA->setObjectName("tableWidgetDFA");
        tableWidgetDFA->setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
        tableWidgetDFA->setColumnCount(2);
        tableWidgetDFA->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_6->addWidget(tableWidgetDFA);

        tabWidget->addTab(tabDFA, QString());
        tabMinDFA = new QWidget();
        tabMinDFA->setObjectName("tabMinDFA");
        verticalLayout_7 = new QVBoxLayout(tabMinDFA);
        verticalLayout_7->setObjectName("verticalLayout_7");
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        pushButtonMinimizeDFA = new QPushButton(tabMinDFA);
        pushButtonMinimizeDFA->setObjectName("pushButtonMinimizeDFA");

        horizontalLayout_5->addWidget(pushButtonMinimizeDFA);

        pushButtonExportMinDFA = new QPushButton(tabMinDFA);
        pushButtonExportMinDFA->setObjectName("pushButtonExportMinDFA");

        horizontalLayout_5->addWidget(pushButtonExportMinDFA);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);

        labelMinDFAInfo = new QLabel(tabMinDFA);
        labelMinDFAInfo->setObjectName("labelMinDFAInfo");

        horizontalLayout_5->addWidget(labelMinDFAInfo);


        verticalLayout_7->addLayout(horizontalLayout_5);

        tableWidgetMinDFA = new QTableWidget(tabMinDFA);
        if (tableWidgetMinDFA->columnCount() < 2)
            tableWidgetMinDFA->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableWidgetMinDFA->setHorizontalHeaderItem(0, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableWidgetMinDFA->setHorizontalHeaderItem(1, __qtablewidgetitem9);
        tableWidgetMinDFA->setObjectName("tableWidgetMinDFA");
        tableWidgetMinDFA->setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
        tableWidgetMinDFA->setColumnCount(2);
        tableWidgetMinDFA->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_7->addWidget(tableWidgetMinDFA);

        tabWidget->addTab(tabMinDFA, QString());

        verticalLayout->addWidget(tabWidget);

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
        groupBox_Input->setTitle(QCoreApplication::translate("Task2Window", "\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217\350\276\223\345\205\245", nullptr));
        pushButtonParse->setText(QCoreApplication::translate("Task2Window", "\350\247\243\346\236\220", nullptr));
        pushButtonLoadExample->setText(QCoreApplication::translate("Task2Window", "\345\212\240\350\275\275\347\244\272\344\276\213", nullptr));
        pushButtonClear->setText(QCoreApplication::translate("Task2Window", "\346\270\205\347\251\272", nullptr));
        groupBox_ParseResult->setTitle(QCoreApplication::translate("Task2Window", "\350\247\243\346\236\220\347\273\223\346\236\234", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidgetParseResult->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("Task2Window", "\345\220\215\347\247\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidgetParseResult->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("Task2Window", "\346\250\241\345\274\217", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidgetParseResult->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("Task2Window", "\347\274\226\347\240\201", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidgetParseResult->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("Task2Window", "\346\230\257\345\220\246\344\270\272\345\244\232\345\215\225\350\257\215", nullptr));
        labelErrorMessage->setText(QCoreApplication::translate("Task2Window", "\351\224\231\350\257\257\344\277\241\346\201\257\357\274\232", nullptr));
        labelErrorMessage->setStyleSheet(QCoreApplication::translate("Task2Window", "color: red;", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabRegexInput), QCoreApplication::translate("Task2Window", "\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217\350\276\223\345\205\245\344\270\216\350\247\243\346\236\220", nullptr));
        pushButtonBuildNFA->setText(QCoreApplication::translate("Task2Window", "\346\236\204\345\273\272NFA", nullptr));
        pushButtonExportNFA->setText(QCoreApplication::translate("Task2Window", "\345\257\274\345\207\272NFA", nullptr));
        labelNFAInfo->setText(QCoreApplication::translate("Task2Window", "NFA\344\277\241\346\201\257\357\274\232\347\212\266\346\200\201\346\225\260: 0, \350\275\254\347\247\273\346\225\260: 0", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidgetNFA->horizontalHeaderItem(0);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("Task2Window", "\347\212\266\346\200\201\346\240\207\350\256\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidgetNFA->horizontalHeaderItem(1);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("Task2Window", "\347\212\266\346\200\201\347\274\226\345\217\267", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabNFA), QCoreApplication::translate("Task2Window", "NFA\345\217\257\350\247\206\345\214\226", nullptr));
        pushButtonConvertDFA->setText(QCoreApplication::translate("Task2Window", "\350\275\254\346\215\242DFA", nullptr));
        pushButtonExportDFA->setText(QCoreApplication::translate("Task2Window", "\345\257\274\345\207\272DFA", nullptr));
        labelDFAInfo->setText(QCoreApplication::translate("Task2Window", "DFA\344\277\241\346\201\257\357\274\232\347\212\266\346\200\201\346\225\260: 0, \350\275\254\347\247\273\346\225\260: 0", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tableWidgetDFA->horizontalHeaderItem(0);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("Task2Window", "\347\212\266\346\200\201\346\240\207\350\256\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tableWidgetDFA->horizontalHeaderItem(1);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("Task2Window", "\347\212\266\346\200\201\347\274\226\345\217\267", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabDFA), QCoreApplication::translate("Task2Window", "DFA\345\217\257\350\247\206\345\214\226", nullptr));
        pushButtonMinimizeDFA->setText(QCoreApplication::translate("Task2Window", "\346\234\200\345\260\217\345\214\226DFA", nullptr));
        pushButtonExportMinDFA->setText(QCoreApplication::translate("Task2Window", "\345\257\274\345\207\272\346\234\200\345\260\217\345\214\226DFA", nullptr));
        labelMinDFAInfo->setText(QCoreApplication::translate("Task2Window", "\346\234\200\345\260\217\345\214\226DFA\344\277\241\346\201\257\357\274\232\347\212\266\346\200\201\346\225\260: 0, \350\275\254\347\247\273\346\225\260: 0", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tableWidgetMinDFA->horizontalHeaderItem(0);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("Task2Window", "\347\212\266\346\200\201\346\240\207\350\256\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = tableWidgetMinDFA->horizontalHeaderItem(1);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("Task2Window", "\347\212\266\346\200\201\347\274\226\345\217\267", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabMinDFA), QCoreApplication::translate("Task2Window", "\346\234\200\345\260\217\345\214\226DFA\345\217\257\350\247\206\345\214\226", nullptr));
        menuFile->setTitle(QCoreApplication::translate("Task2Window", "\346\226\207\344\273\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Task2Window: public Ui_Task2Window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TASK2WINDOW_H
