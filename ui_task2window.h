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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
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
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Task2Window
{
public:
    QAction *actionExit;
    QAction *actionOpen;
    QAction *actionSave;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tabBNF;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBoxBNFInput;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *textEditBNF;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButtonParseBNF;
    QPushButton *pushButtonLoadExample;
    QPushButton *pushButtonClearBNF;
    QGroupBox *groupBoxGrammarInfo;
    QVBoxLayout *verticalLayout_4;
    QTextBrowser *textBrowserGrammarInfo;
    QWidget *tabFirstFollow;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButtonCalculateFirst;
    QPushButton *pushButtonCalculateFollow;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_4;
    QGroupBox *groupBoxFirst;
    QVBoxLayout *verticalLayout_6;
    QTableWidget *tableWidgetFirst;
    QGroupBox *groupBoxFollow;
    QVBoxLayout *verticalLayout_7;
    QTableWidget *tableWidgetFollow;
    QWidget *tabLR0DFA;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButtonGenerateLR0DFA;
    QComboBox *comboBoxLR0View;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBoxLR0DFA;
    QVBoxLayout *verticalLayout_9;
    QTableWidget *tableWidgetLR0;
    QWidget *tabSLR1;
    QVBoxLayout *verticalLayout_10;
    QPushButton *pushButtonCheckSLR1;
    QGroupBox *groupBoxSLR1Result;
    QVBoxLayout *verticalLayout_11;
    QTextBrowser *textBrowserSLR1Result;
    QWidget *tabLR1DFA;
    QVBoxLayout *verticalLayout_12;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *pushButtonGenerateLR1DFA;
    QComboBox *comboBoxLR1View;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *groupBoxLR1DFA;
    QVBoxLayout *verticalLayout_13;
    QTableWidget *tableWidgetLR1;
    QWidget *tabLR1Table;
    QVBoxLayout *verticalLayout_14;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pushButtonGenerateLR1Table;
    QPushButton *pushButtonSaveLR1Table;
    QSpacerItem *horizontalSpacer_4;
    QGroupBox *groupBoxLR1Table;
    QVBoxLayout *verticalLayout_15;
    QTableWidget *tableWidgetLR1Table;
    QWidget *tabSyntaxAnalysis;
    QVBoxLayout *verticalLayout_16;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *pushButtonOpenTokenFile;
    QPushButton *pushButtonAnalyzeSyntax;
    QPushButton *pushButtonSaveSyntaxTree;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_9;
    QGroupBox *groupBoxTokenFile;
    QVBoxLayout *verticalLayout_17;
    QTextEdit *textEditTokenFile;
    QHBoxLayout *horizontalLayout_10;
    QGroupBox *groupBoxAnalysisSteps;
    QVBoxLayout *verticalLayout_18;
    QTableWidget *tableWidgetAnalysisSteps;
    QGroupBox *groupBoxSyntaxTree;
    QVBoxLayout *verticalLayout_19;
    QTreeWidget *treeWidgetSyntaxTree;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Task2Window)
    {
        if (Task2Window->objectName().isEmpty())
            Task2Window->setObjectName("Task2Window");
        Task2Window->resize(1200, 900);
        actionExit = new QAction(Task2Window);
        actionExit->setObjectName("actionExit");
        actionOpen = new QAction(Task2Window);
        actionOpen->setObjectName("actionOpen");
        actionSave = new QAction(Task2Window);
        actionSave->setObjectName("actionSave");
        centralwidget = new QWidget(Task2Window);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabBNF = new QWidget();
        tabBNF->setObjectName("tabBNF");
        verticalLayout_2 = new QVBoxLayout(tabBNF);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        groupBoxBNFInput = new QGroupBox(tabBNF);
        groupBoxBNFInput->setObjectName("groupBoxBNFInput");
        verticalLayout_3 = new QVBoxLayout(groupBoxBNFInput);
        verticalLayout_3->setObjectName("verticalLayout_3");
        textEditBNF = new QTextEdit(groupBoxBNFInput);
        textEditBNF->setObjectName("textEditBNF");
        QFont font;
        font.setFamilies({QString::fromUtf8("Courier New")});
        font.setPointSize(10);
        textEditBNF->setFont(font);

        verticalLayout_3->addWidget(textEditBNF);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        pushButtonParseBNF = new QPushButton(groupBoxBNFInput);
        pushButtonParseBNF->setObjectName("pushButtonParseBNF");

        horizontalLayout_2->addWidget(pushButtonParseBNF);

        pushButtonLoadExample = new QPushButton(groupBoxBNFInput);
        pushButtonLoadExample->setObjectName("pushButtonLoadExample");

        horizontalLayout_2->addWidget(pushButtonLoadExample);

        pushButtonClearBNF = new QPushButton(groupBoxBNFInput);
        pushButtonClearBNF->setObjectName("pushButtonClearBNF");

        horizontalLayout_2->addWidget(pushButtonClearBNF);


        verticalLayout_3->addLayout(horizontalLayout_2);


        horizontalLayout->addWidget(groupBoxBNFInput);

        groupBoxGrammarInfo = new QGroupBox(tabBNF);
        groupBoxGrammarInfo->setObjectName("groupBoxGrammarInfo");
        verticalLayout_4 = new QVBoxLayout(groupBoxGrammarInfo);
        verticalLayout_4->setObjectName("verticalLayout_4");
        textBrowserGrammarInfo = new QTextBrowser(groupBoxGrammarInfo);
        textBrowserGrammarInfo->setObjectName("textBrowserGrammarInfo");
        textBrowserGrammarInfo->setFont(font);

        verticalLayout_4->addWidget(textBrowserGrammarInfo);


        horizontalLayout->addWidget(groupBoxGrammarInfo);


        verticalLayout_2->addLayout(horizontalLayout);

        tabWidget->addTab(tabBNF, QString());
        tabFirstFollow = new QWidget();
        tabFirstFollow->setObjectName("tabFirstFollow");
        verticalLayout_5 = new QVBoxLayout(tabFirstFollow);
        verticalLayout_5->setObjectName("verticalLayout_5");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        pushButtonCalculateFirst = new QPushButton(tabFirstFollow);
        pushButtonCalculateFirst->setObjectName("pushButtonCalculateFirst");

        horizontalLayout_3->addWidget(pushButtonCalculateFirst);

        pushButtonCalculateFollow = new QPushButton(tabFirstFollow);
        pushButtonCalculateFollow->setObjectName("pushButtonCalculateFollow");

        horizontalLayout_3->addWidget(pushButtonCalculateFollow);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout_5->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        groupBoxFirst = new QGroupBox(tabFirstFollow);
        groupBoxFirst->setObjectName("groupBoxFirst");
        verticalLayout_6 = new QVBoxLayout(groupBoxFirst);
        verticalLayout_6->setObjectName("verticalLayout_6");
        tableWidgetFirst = new QTableWidget(groupBoxFirst);
        if (tableWidgetFirst->columnCount() < 2)
            tableWidgetFirst->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidgetFirst->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidgetFirst->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidgetFirst->setObjectName("tableWidgetFirst");
        tableWidgetFirst->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidgetFirst->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidgetFirst->setColumnCount(2);
        tableWidgetFirst->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_6->addWidget(tableWidgetFirst);


        horizontalLayout_4->addWidget(groupBoxFirst);

        groupBoxFollow = new QGroupBox(tabFirstFollow);
        groupBoxFollow->setObjectName("groupBoxFollow");
        verticalLayout_7 = new QVBoxLayout(groupBoxFollow);
        verticalLayout_7->setObjectName("verticalLayout_7");
        tableWidgetFollow = new QTableWidget(groupBoxFollow);
        if (tableWidgetFollow->columnCount() < 2)
            tableWidgetFollow->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidgetFollow->setHorizontalHeaderItem(0, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidgetFollow->setHorizontalHeaderItem(1, __qtablewidgetitem3);
        tableWidgetFollow->setObjectName("tableWidgetFollow");
        tableWidgetFollow->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidgetFollow->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidgetFollow->setColumnCount(2);
        tableWidgetFollow->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_7->addWidget(tableWidgetFollow);


        horizontalLayout_4->addWidget(groupBoxFollow);


        verticalLayout_5->addLayout(horizontalLayout_4);

        tabWidget->addTab(tabFirstFollow, QString());
        tabLR0DFA = new QWidget();
        tabLR0DFA->setObjectName("tabLR0DFA");
        verticalLayout_8 = new QVBoxLayout(tabLR0DFA);
        verticalLayout_8->setObjectName("verticalLayout_8");
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        pushButtonGenerateLR0DFA = new QPushButton(tabLR0DFA);
        pushButtonGenerateLR0DFA->setObjectName("pushButtonGenerateLR0DFA");

        horizontalLayout_5->addWidget(pushButtonGenerateLR0DFA);

        comboBoxLR0View = new QComboBox(tabLR0DFA);
        comboBoxLR0View->setObjectName("comboBoxLR0View");
        comboBoxLR0View->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        horizontalLayout_5->addWidget(comboBoxLR0View);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);


        verticalLayout_8->addLayout(horizontalLayout_5);

        groupBoxLR0DFA = new QGroupBox(tabLR0DFA);
        groupBoxLR0DFA->setObjectName("groupBoxLR0DFA");
        verticalLayout_9 = new QVBoxLayout(groupBoxLR0DFA);
        verticalLayout_9->setObjectName("verticalLayout_9");
        tableWidgetLR0 = new QTableWidget(groupBoxLR0DFA);
        if (tableWidgetLR0->columnCount() < 3)
            tableWidgetLR0->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidgetLR0->setHorizontalHeaderItem(0, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidgetLR0->setHorizontalHeaderItem(1, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidgetLR0->setHorizontalHeaderItem(2, __qtablewidgetitem6);
        tableWidgetLR0->setObjectName("tableWidgetLR0");
        tableWidgetLR0->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidgetLR0->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidgetLR0->setColumnCount(3);
        tableWidgetLR0->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_9->addWidget(tableWidgetLR0);


        verticalLayout_8->addWidget(groupBoxLR0DFA);

        tabWidget->addTab(tabLR0DFA, QString());
        tabSLR1 = new QWidget();
        tabSLR1->setObjectName("tabSLR1");
        verticalLayout_10 = new QVBoxLayout(tabSLR1);
        verticalLayout_10->setObjectName("verticalLayout_10");
        pushButtonCheckSLR1 = new QPushButton(tabSLR1);
        pushButtonCheckSLR1->setObjectName("pushButtonCheckSLR1");

        verticalLayout_10->addWidget(pushButtonCheckSLR1);

        groupBoxSLR1Result = new QGroupBox(tabSLR1);
        groupBoxSLR1Result->setObjectName("groupBoxSLR1Result");
        verticalLayout_11 = new QVBoxLayout(groupBoxSLR1Result);
        verticalLayout_11->setObjectName("verticalLayout_11");
        textBrowserSLR1Result = new QTextBrowser(groupBoxSLR1Result);
        textBrowserSLR1Result->setObjectName("textBrowserSLR1Result");
        textBrowserSLR1Result->setFont(font);

        verticalLayout_11->addWidget(textBrowserSLR1Result);


        verticalLayout_10->addWidget(groupBoxSLR1Result);

        tabWidget->addTab(tabSLR1, QString());
        tabLR1DFA = new QWidget();
        tabLR1DFA->setObjectName("tabLR1DFA");
        verticalLayout_12 = new QVBoxLayout(tabLR1DFA);
        verticalLayout_12->setObjectName("verticalLayout_12");
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        pushButtonGenerateLR1DFA = new QPushButton(tabLR1DFA);
        pushButtonGenerateLR1DFA->setObjectName("pushButtonGenerateLR1DFA");

        horizontalLayout_6->addWidget(pushButtonGenerateLR1DFA);

        comboBoxLR1View = new QComboBox(tabLR1DFA);
        comboBoxLR1View->setObjectName("comboBoxLR1View");
        comboBoxLR1View->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        horizontalLayout_6->addWidget(comboBoxLR1View);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_3);


        verticalLayout_12->addLayout(horizontalLayout_6);

        groupBoxLR1DFA = new QGroupBox(tabLR1DFA);
        groupBoxLR1DFA->setObjectName("groupBoxLR1DFA");
        verticalLayout_13 = new QVBoxLayout(groupBoxLR1DFA);
        verticalLayout_13->setObjectName("verticalLayout_13");
        tableWidgetLR1 = new QTableWidget(groupBoxLR1DFA);
        if (tableWidgetLR1->columnCount() < 3)
            tableWidgetLR1->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableWidgetLR1->setHorizontalHeaderItem(0, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableWidgetLR1->setHorizontalHeaderItem(1, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableWidgetLR1->setHorizontalHeaderItem(2, __qtablewidgetitem9);
        tableWidgetLR1->setObjectName("tableWidgetLR1");
        tableWidgetLR1->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidgetLR1->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidgetLR1->setColumnCount(3);
        tableWidgetLR1->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_13->addWidget(tableWidgetLR1);


        verticalLayout_12->addWidget(groupBoxLR1DFA);

        tabWidget->addTab(tabLR1DFA, QString());
        tabLR1Table = new QWidget();
        tabLR1Table->setObjectName("tabLR1Table");
        verticalLayout_14 = new QVBoxLayout(tabLR1Table);
        verticalLayout_14->setObjectName("verticalLayout_14");
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        pushButtonGenerateLR1Table = new QPushButton(tabLR1Table);
        pushButtonGenerateLR1Table->setObjectName("pushButtonGenerateLR1Table");

        horizontalLayout_7->addWidget(pushButtonGenerateLR1Table);

        pushButtonSaveLR1Table = new QPushButton(tabLR1Table);
        pushButtonSaveLR1Table->setObjectName("pushButtonSaveLR1Table");

        horizontalLayout_7->addWidget(pushButtonSaveLR1Table);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_4);


        verticalLayout_14->addLayout(horizontalLayout_7);

        groupBoxLR1Table = new QGroupBox(tabLR1Table);
        groupBoxLR1Table->setObjectName("groupBoxLR1Table");
        verticalLayout_15 = new QVBoxLayout(groupBoxLR1Table);
        verticalLayout_15->setObjectName("verticalLayout_15");
        tableWidgetLR1Table = new QTableWidget(groupBoxLR1Table);
        if (tableWidgetLR1Table->columnCount() < 2)
            tableWidgetLR1Table->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableWidgetLR1Table->setHorizontalHeaderItem(0, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableWidgetLR1Table->setHorizontalHeaderItem(1, __qtablewidgetitem11);
        tableWidgetLR1Table->setObjectName("tableWidgetLR1Table");
        tableWidgetLR1Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidgetLR1Table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidgetLR1Table->setColumnCount(2);
        tableWidgetLR1Table->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_15->addWidget(tableWidgetLR1Table);


        verticalLayout_14->addWidget(groupBoxLR1Table);

        tabWidget->addTab(tabLR1Table, QString());
        tabSyntaxAnalysis = new QWidget();
        tabSyntaxAnalysis->setObjectName("tabSyntaxAnalysis");
        verticalLayout_16 = new QVBoxLayout(tabSyntaxAnalysis);
        verticalLayout_16->setObjectName("verticalLayout_16");
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        pushButtonOpenTokenFile = new QPushButton(tabSyntaxAnalysis);
        pushButtonOpenTokenFile->setObjectName("pushButtonOpenTokenFile");

        horizontalLayout_8->addWidget(pushButtonOpenTokenFile);

        pushButtonAnalyzeSyntax = new QPushButton(tabSyntaxAnalysis);
        pushButtonAnalyzeSyntax->setObjectName("pushButtonAnalyzeSyntax");

        horizontalLayout_8->addWidget(pushButtonAnalyzeSyntax);

        pushButtonSaveSyntaxTree = new QPushButton(tabSyntaxAnalysis);
        pushButtonSaveSyntaxTree->setObjectName("pushButtonSaveSyntaxTree");

        horizontalLayout_8->addWidget(pushButtonSaveSyntaxTree);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_5);


        verticalLayout_16->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        groupBoxTokenFile = new QGroupBox(tabSyntaxAnalysis);
        groupBoxTokenFile->setObjectName("groupBoxTokenFile");
        verticalLayout_17 = new QVBoxLayout(groupBoxTokenFile);
        verticalLayout_17->setObjectName("verticalLayout_17");
        textEditTokenFile = new QTextEdit(groupBoxTokenFile);
        textEditTokenFile->setObjectName("textEditTokenFile");
        textEditTokenFile->setFont(font);

        verticalLayout_17->addWidget(textEditTokenFile);


        horizontalLayout_9->addWidget(groupBoxTokenFile);


        verticalLayout_16->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        groupBoxAnalysisSteps = new QGroupBox(tabSyntaxAnalysis);
        groupBoxAnalysisSteps->setObjectName("groupBoxAnalysisSteps");
        verticalLayout_18 = new QVBoxLayout(groupBoxAnalysisSteps);
        verticalLayout_18->setObjectName("verticalLayout_18");
        tableWidgetAnalysisSteps = new QTableWidget(groupBoxAnalysisSteps);
        if (tableWidgetAnalysisSteps->columnCount() < 5)
            tableWidgetAnalysisSteps->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tableWidgetAnalysisSteps->setHorizontalHeaderItem(0, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tableWidgetAnalysisSteps->setHorizontalHeaderItem(1, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tableWidgetAnalysisSteps->setHorizontalHeaderItem(2, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tableWidgetAnalysisSteps->setHorizontalHeaderItem(3, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tableWidgetAnalysisSteps->setHorizontalHeaderItem(4, __qtablewidgetitem16);
        tableWidgetAnalysisSteps->setObjectName("tableWidgetAnalysisSteps");
        tableWidgetAnalysisSteps->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidgetAnalysisSteps->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidgetAnalysisSteps->setColumnCount(5);
        tableWidgetAnalysisSteps->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_18->addWidget(tableWidgetAnalysisSteps);


        horizontalLayout_10->addWidget(groupBoxAnalysisSteps);

        groupBoxSyntaxTree = new QGroupBox(tabSyntaxAnalysis);
        groupBoxSyntaxTree->setObjectName("groupBoxSyntaxTree");
        verticalLayout_19 = new QVBoxLayout(groupBoxSyntaxTree);
        verticalLayout_19->setObjectName("verticalLayout_19");
        treeWidgetSyntaxTree = new QTreeWidget(groupBoxSyntaxTree);
        treeWidgetSyntaxTree->setObjectName("treeWidgetSyntaxTree");
        treeWidgetSyntaxTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeWidgetSyntaxTree->setHeaderHidden(false);

        verticalLayout_19->addWidget(treeWidgetSyntaxTree);


        horizontalLayout_10->addWidget(groupBoxSyntaxTree);


        verticalLayout_16->addLayout(horizontalLayout_10);

        tabWidget->addTab(tabSyntaxAnalysis, QString());

        verticalLayout->addWidget(tabWidget);

        Task2Window->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Task2Window);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1200, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        Task2Window->setMenuBar(menubar);
        statusbar = new QStatusBar(Task2Window);
        statusbar->setObjectName("statusbar");
        Task2Window->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionExit);

        retranslateUi(Task2Window);

        QMetaObject::connectSlotsByName(Task2Window);
    } // setupUi

    void retranslateUi(QMainWindow *Task2Window)
    {
        Task2Window->setWindowTitle(QCoreApplication::translate("Task2Window", "\345\256\236\351\252\214\344\272\214 - \350\257\255\346\263\225\345\210\206\346\236\220\345\231\250\347\224\237\346\210\220\345\231\250", nullptr));
        actionExit->setText(QCoreApplication::translate("Task2Window", "\351\200\200\345\207\272", nullptr));
        actionOpen->setText(QCoreApplication::translate("Task2Window", "\346\211\223\345\274\200", nullptr));
        actionSave->setText(QCoreApplication::translate("Task2Window", "\344\277\235\345\255\230", nullptr));
        groupBoxBNFInput->setTitle(QCoreApplication::translate("Task2Window", "BNF\346\226\207\346\263\225\350\276\223\345\205\245", nullptr));
        pushButtonParseBNF->setText(QCoreApplication::translate("Task2Window", "\350\247\243\346\236\220\346\226\207\346\263\225", nullptr));
        pushButtonLoadExample->setText(QCoreApplication::translate("Task2Window", "\345\212\240\350\275\275\347\244\272\344\276\213", nullptr));
        pushButtonClearBNF->setText(QCoreApplication::translate("Task2Window", "\346\270\205\347\251\272", nullptr));
        groupBoxGrammarInfo->setTitle(QCoreApplication::translate("Task2Window", "\346\226\207\346\263\225\344\277\241\346\201\257", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabBNF), QCoreApplication::translate("Task2Window", "BNF\346\226\207\346\263\225\345\244\204\347\220\206", nullptr));
        pushButtonCalculateFirst->setText(QCoreApplication::translate("Task2Window", "\350\256\241\347\256\227First\351\233\206\345\220\210", nullptr));
        pushButtonCalculateFollow->setText(QCoreApplication::translate("Task2Window", "\350\256\241\347\256\227Follow\351\233\206\345\220\210", nullptr));
        groupBoxFirst->setTitle(QCoreApplication::translate("Task2Window", "First\351\233\206\345\220\210", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidgetFirst->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("Task2Window", "\351\235\236\347\273\210\347\273\223\347\254\246", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidgetFirst->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("Task2Window", "First\351\233\206\345\220\210", nullptr));
        groupBoxFollow->setTitle(QCoreApplication::translate("Task2Window", "Follow\351\233\206\345\220\210", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidgetFollow->horizontalHeaderItem(0);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("Task2Window", "\351\235\236\347\273\210\347\273\223\347\254\246", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidgetFollow->horizontalHeaderItem(1);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("Task2Window", "Follow\351\233\206\345\220\210", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabFirstFollow), QCoreApplication::translate("Task2Window", "First\344\270\216Follow\351\233\206\345\220\210", nullptr));
        pushButtonGenerateLR0DFA->setText(QCoreApplication::translate("Task2Window", "\347\224\237\346\210\220LR(0) DFA", nullptr));
        groupBoxLR0DFA->setTitle(QCoreApplication::translate("Task2Window", "LR(0) DFA", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidgetLR0->horizontalHeaderItem(0);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("Task2Window", "\347\212\266\346\200\201ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidgetLR0->horizontalHeaderItem(1);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("Task2Window", "\351\241\271\347\233\256\351\233\206", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tableWidgetLR0->horizontalHeaderItem(2);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("Task2Window", "\350\275\254\347\247\273\345\205\263\347\263\273", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabLR0DFA), QCoreApplication::translate("Task2Window", "LR(0) DFA", nullptr));
        pushButtonCheckSLR1->setText(QCoreApplication::translate("Task2Window", "\346\243\200\346\237\245SLR(1)\346\226\207\346\263\225", nullptr));
        groupBoxSLR1Result->setTitle(QCoreApplication::translate("Task2Window", "SLR(1)\345\210\244\346\226\255\347\273\223\346\236\234", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabSLR1), QCoreApplication::translate("Task2Window", "SLR(1)\346\226\207\346\263\225\345\210\244\346\226\255", nullptr));
        pushButtonGenerateLR1DFA->setText(QCoreApplication::translate("Task2Window", "\347\224\237\346\210\220LR(1) DFA", nullptr));
        groupBoxLR1DFA->setTitle(QCoreApplication::translate("Task2Window", "LR(1) DFA", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tableWidgetLR1->horizontalHeaderItem(0);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("Task2Window", "\347\212\266\346\200\201ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tableWidgetLR1->horizontalHeaderItem(1);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("Task2Window", "\351\241\271\347\233\256\351\233\206", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = tableWidgetLR1->horizontalHeaderItem(2);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("Task2Window", "\350\275\254\347\247\273\345\205\263\347\263\273", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabLR1DFA), QCoreApplication::translate("Task2Window", "LR(1) DFA", nullptr));
        pushButtonGenerateLR1Table->setText(QCoreApplication::translate("Task2Window", "\347\224\237\346\210\220LR(1)\345\210\206\346\236\220\350\241\250", nullptr));
        pushButtonSaveLR1Table->setText(QCoreApplication::translate("Task2Window", "\344\277\235\345\255\230\345\210\206\346\236\220\350\241\250", nullptr));
        groupBoxLR1Table->setTitle(QCoreApplication::translate("Task2Window", "LR(1)\345\210\206\346\236\220\350\241\250", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tableWidgetLR1Table->horizontalHeaderItem(0);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("Task2Window", "\347\212\266\346\200\201ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = tableWidgetLR1Table->horizontalHeaderItem(1);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("Task2Window", "Action/Goto", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabLR1Table), QCoreApplication::translate("Task2Window", "LR(1)\345\210\206\346\236\220\350\241\250", nullptr));
        pushButtonOpenTokenFile->setText(QCoreApplication::translate("Task2Window", "\346\211\223\345\274\200\350\257\215\346\263\225\345\215\225\345\205\203\346\226\207\344\273\266", nullptr));
        pushButtonAnalyzeSyntax->setText(QCoreApplication::translate("Task2Window", "\350\277\233\350\241\214\350\257\255\346\263\225\345\210\206\346\236\220", nullptr));
        pushButtonSaveSyntaxTree->setText(QCoreApplication::translate("Task2Window", "\344\277\235\345\255\230\350\257\255\346\263\225\346\240\221", nullptr));
        groupBoxTokenFile->setTitle(QCoreApplication::translate("Task2Window", "\350\257\215\346\263\225\345\215\225\345\205\203\346\226\207\344\273\266", nullptr));
        groupBoxAnalysisSteps->setTitle(QCoreApplication::translate("Task2Window", "\345\210\206\346\236\220\346\255\245\351\252\244", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = tableWidgetAnalysisSteps->horizontalHeaderItem(0);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("Task2Window", "\346\255\245\351\252\244", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = tableWidgetAnalysisSteps->horizontalHeaderItem(1);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("Task2Window", "\347\212\266\346\200\201\346\240\210", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = tableWidgetAnalysisSteps->horizontalHeaderItem(2);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("Task2Window", "\347\254\246\345\217\267\346\240\210", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = tableWidgetAnalysisSteps->horizontalHeaderItem(3);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("Task2Window", "\350\276\223\345\205\245\344\270\262", nullptr));
        QTableWidgetItem *___qtablewidgetitem16 = tableWidgetAnalysisSteps->horizontalHeaderItem(4);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("Task2Window", "\345\212\250\344\275\234", nullptr));
        groupBoxSyntaxTree->setTitle(QCoreApplication::translate("Task2Window", "\350\257\255\346\263\225\346\240\221", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = treeWidgetSyntaxTree->headerItem();
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("Task2Window", "\350\257\255\346\263\225\346\240\221", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabSyntaxAnalysis), QCoreApplication::translate("Task2Window", "\350\257\255\346\263\225\345\210\206\346\236\220", nullptr));
        menuFile->setTitle(QCoreApplication::translate("Task2Window", "\346\226\207\344\273\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Task2Window: public Ui_Task2Window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TASK2WINDOW_H
