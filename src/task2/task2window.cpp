/**
 * @file task2window.cpp
 * @brief 任务二主窗口实现
 * @version 1.0
 * @author 编译原理课程设计小组
 * @date 2025-12-06
 * @copyright Copyright (c) 2025 编译原理课程设计
 * @details 实现了任务二的主窗口界面和事件处理
 */

#include "task2/task2window.h"
#include "ui_task2window.h"
#include "task2/bnfparser.h"
#include "task2/firstfollow.h"
#include "task2/lr0dfa.h"
#include "task2/slr1checker.h"
#include "task2/lr1dfa.h"
#include "task2/lr1table.h"
#include "task2/syntaxanalyzer.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

Task2Window::Task2Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Task2Window)
    , m_bnfParser(new BNFParser())
    , m_firstFollow(new FirstFollow())
    , m_lr0dfa(new LR0DFA())
    , m_slr1Checker(new SLR1Checker())
    , m_lr1dfa(new LR1DFA())
    , m_lr1Table(new LR1Table())
    , m_syntaxAnalyzer(new SyntaxAnalyzer())
{
    ui->setupUi(this);
    setWindowTitle(tr("实验二 - 语法分析器生成器"));
    
    // 初始化处理器类之间的关联
    m_firstFollow->setParser(m_bnfParser);
    m_lr0dfa->setParser(m_bnfParser);
    m_slr1Checker->setLR0DFA(m_lr0dfa);
    m_slr1Checker->setFirstFollow(m_firstFollow);
    m_lr1dfa->setParser(m_bnfParser);
    m_lr1dfa->setFirstFollow(m_firstFollow);
    m_lr1Table->setLR1DFA(m_lr1dfa);
    m_lr1Table->setParser(m_bnfParser);
    m_syntaxAnalyzer->setLR1Table(m_lr1Table);
    m_syntaxAnalyzer->setParser(m_bnfParser);
    
    // 初始化UI组件
    initUI();
}

Task2Window::~Task2Window()
{
    // 释放处理器类内存
    delete m_bnfParser;
    delete m_firstFollow;
    delete m_lr0dfa;
    delete m_slr1Checker;
    delete m_lr1dfa;
    delete m_lr1Table;
    delete m_syntaxAnalyzer;
    
    delete ui;
}



void Task2Window::loadExampleGrammar()
{
    // Tiny语言BNF文法示例
    QString example = "// Tiny语言BNF文法示例\n";
    example += "program -> stmt-sequence\n";
    example += "stmt-sequence -> stmt-sequence ; statement | statement\n";
    example += "statement -> if-stmt | repeat-stmt | assign-stmt | read-stmt | write-stmt\n";
    example += "if-stmt -> if exp then stmt-sequence end | if exp then stmt-sequence else stmt-sequence end\n";
    example += "repeat-stmt -> repeat stmt-sequence until exp\n";
    example += "assign-stmt -> identifier := exp\n";
    example += "read-stmt -> read identifier\n";
    example += "write-stmt -> write exp\n";
    example += "exp -> simple-exp comparison-op simple-exp | simple-exp\n";
    example += "comparison-op -> < | > | = | >= | <= | <>\n";
    example += "simple-exp -> simple-exp addop term | term\n";
    example += "addop -> + | -\n";
    example += "term -> term mulop factor | factor\n";
    example += "mulop -> * | / | % | ^\n";
    example += "factor -> ( exp ) | number | identifier\n";
    
    ui->textEditBNF->setPlainText(example);
}

void Task2Window::on_actionExit_triggered()
{
    close();
}

void Task2Window::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开BNF文法文件"), ".", tr("文本文件 (*.txt);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        if (m_bnfParser->loadFromFile(fileName)) {
            ui->textEditBNF->setPlainText(m_bnfParser->printGrammar());
            QMessageBox::information(this, tr("成功"), tr("BNF文法文件加载成功"));
        } else {
            QMessageBox::warning(this, tr("错误"), tr("BNF文法文件加载失败: %1").arg(m_bnfParser->getErrorMessage()));
        }
    }
}

void Task2Window::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存BNF文法文件"), ".", tr("文本文件 (*.txt);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        if (m_bnfParser->saveToFile(fileName)) {
            QMessageBox::information(this, tr("成功"), tr("BNF文法文件保存成功"));
        } else {
            QMessageBox::warning(this, tr("错误"), tr("BNF文法文件保存失败: %1").arg(m_bnfParser->getErrorMessage()));
        }
    }
}

void Task2Window::on_pushButtonParseBNF_clicked()
{
    QString bnfText = ui->textEditBNF->toPlainText();
    
    if (bnfText.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请输入BNF文法"));
        return;
    }
    
    if (m_bnfParser->parse(bnfText)) {
        QMessageBox::information(this, tr("成功"), tr("BNF文法解析成功"));
        
        // 显示文法信息
        QString grammarInfo = m_bnfParser->printGrammar();
        ui->textBrowserGrammarInfo->setText(grammarInfo);
    } else {
        QMessageBox::warning(this, tr("错误"), tr("BNF文法解析失败: %1").arg(m_bnfParser->getErrorMessage()));
    }
}

void Task2Window::on_pushButtonClearBNF_clicked()
{
    ui->textEditBNF->clear();
    ui->textBrowserGrammarInfo->clear();
    ui->tableWidgetFirst->setRowCount(0);
    ui->tableWidgetFollow->setRowCount(0);
    ui->tableWidgetLR0->setRowCount(0);
    ui->tableWidgetLR1->setRowCount(0);
    ui->tableWidgetLR1Table->setRowCount(0);
    ui->tableWidgetAnalysisSteps->setRowCount(0);
    ui->textEditTokenFile->clear();
    ui->treeWidgetSyntaxTree->clear();
    
    // 清空处理器类的数据
    m_bnfParser->clear();
    m_firstFollow->clear();
    m_lr0dfa->clear();
    m_slr1Checker->clear();
    m_lr1dfa->clear();
    m_lr1Table->clear();
    m_syntaxAnalyzer->clear();
}

void Task2Window::on_pushButtonLoadExample_clicked()
{
    loadExampleGrammar();
}

void Task2Window::on_pushButtonCalculateFirst_clicked()
{
    if (m_bnfParser->getProductions().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    if (m_firstFollow->calculateFirst()) {
        displayFirstSet();
        QMessageBox::information(this, tr("成功"), tr("First集合计算成功"));
    } else {
        QMessageBox::warning(this, tr("错误"), tr("First集合计算失败: %1").arg(m_firstFollow->getErrorMessage()));
    }
}

void Task2Window::on_pushButtonCalculateFollow_clicked()
{
    if (m_bnfParser->getProductions().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    // 确保First集合已经计算
    if (m_firstFollow->getAllFirst().isEmpty()) {
        if (!m_firstFollow->calculateFirst()) {
            QMessageBox::warning(this, tr("错误"), tr("First集合计算失败: %1").arg(m_firstFollow->getErrorMessage()));
            return;
        }
    }
    
    if (m_firstFollow->calculateFollow()) {
        displayFollowSet();
        QMessageBox::information(this, tr("成功"), tr("Follow集合计算成功"));
    } else {
        QMessageBox::warning(this, tr("错误"), tr("Follow集合计算失败: %1").arg(m_firstFollow->getErrorMessage()));
    }
}

void Task2Window::on_pushButtonGenerateLR0DFA_clicked()
{
    if (m_bnfParser->getProductions().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    if (m_lr0dfa->generate()) {
        displayLR0DFA();
        QMessageBox::information(this, tr("成功"), tr("LR(0) DFA生成成功"));
    } else {
        QMessageBox::warning(this, tr("错误"), tr("LR(0) DFA生成失败: %1").arg(m_lr0dfa->getErrorMessage()));
    }
}

void Task2Window::on_pushButtonCheckSLR1_clicked()
{
    if (m_bnfParser->getProductions().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    // 确保LR(0) DFA已经生成
    if (m_lr0dfa->getItemSets().isEmpty()) {
        if (!m_lr0dfa->generate()) {
            QMessageBox::warning(this, tr("错误"), tr("LR(0) DFA生成失败: %1").arg(m_lr0dfa->getErrorMessage()));
            return;
        }
    }
    
    // 确保First和Follow集合已经计算
    if (m_firstFollow->getAllFirst().isEmpty()) {
        if (!m_firstFollow->calculateFirst()) {
            QMessageBox::warning(this, tr("错误"), tr("First集合计算失败: %1").arg(m_firstFollow->getErrorMessage()));
            return;
        }
    }
    
    if (m_firstFollow->getAllFollow().isEmpty()) {
        if (!m_firstFollow->calculateFollow()) {
            QMessageBox::warning(this, tr("错误"), tr("Follow集合计算失败: %1").arg(m_firstFollow->getErrorMessage()));
            return;
        }
    }
    
    if (m_slr1Checker->checkSLR1()) {
        displaySLR1Result();
        QMessageBox::information(this, tr("成功"), tr("该文法为SLR(1)文法"));
    } else {
        displaySLR1Result();
        QMessageBox::warning(this, tr("警告"), tr("该文法不是SLR(1)文法，存在冲突"));
    }
}

void Task2Window::on_pushButtonGenerateLR1DFA_clicked()
{
    if (m_bnfParser->getProductions().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    if (m_lr1dfa->generate()) {
        displayLR1DFA();
        QMessageBox::information(this, tr("成功"), tr("LR(1) DFA生成成功"));
    } else {
        QMessageBox::warning(this, tr("错误"), tr("LR(1) DFA生成失败: %1").arg(m_lr1dfa->getErrorMessage()));
    }
}

void Task2Window::on_pushButtonGenerateLR1Table_clicked()
{
    if (m_bnfParser->getProductions().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    // 确保LR(1) DFA已经生成
    if (m_lr1dfa->getItemSets().isEmpty()) {
        if (!m_lr1dfa->generate()) {
            QMessageBox::warning(this, tr("错误"), tr("LR(1) DFA生成失败: %1").arg(m_lr1dfa->getErrorMessage()));
            return;
        }
    }
    
    if (m_lr1Table->generateLR1Table()) {
        displayLR1Table();
        QMessageBox::information(this, tr("成功"), tr("LR(1)分析表生成成功"));
    } else {
        QMessageBox::warning(this, tr("错误"), tr("LR(1)分析表生成失败: %1").arg(m_lr1Table->getErrorMessage()));
    }
}

void Task2Window::on_pushButtonSaveLR1Table_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存LR(1)分析表"), ".", tr("CSV文件 (*.csv);;文本文件 (*.txt);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        if (m_lr1Table->saveToCSV(fileName)) {
            QMessageBox::information(this, tr("成功"), tr("LR(1)分析表保存成功"));
        } else {
            QMessageBox::warning(this, tr("错误"), tr("LR(1)分析表保存失败: %1").arg(m_lr1Table->getErrorMessage()));
        }
    }
}

void Task2Window::on_pushButtonOpenTokenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开词法单元文件"), ".", tr("文本文件 (*.txt);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        if (m_syntaxAnalyzer->loadTokensFromFile(fileName)) {
            // 显示词法单元
            QString tokensText;
            for (const Token &token : m_syntaxAnalyzer->getTokens()) {
                tokensText += token.toString() + "\n";
            }
            ui->textEditTokenFile->setText(tokensText);
            QMessageBox::information(this, tr("成功"), tr("词法单元文件加载成功"));
        } else {
            QMessageBox::warning(this, tr("错误"), tr("词法单元文件加载失败: %1").arg(m_syntaxAnalyzer->getErrorMessage()));
        }
    }
}

void Task2Window::on_pushButtonAnalyzeSyntax_clicked()
{
    if (m_syntaxAnalyzer->getTokens().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载词法单元文件"));
        return;
    }
    
    if (m_lr1Table->getActionTable().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先生成LR(1)分析表"));
        return;
    }
    
    AnalysisResult result = m_syntaxAnalyzer->analyze();
    if (result.success) {
        displaySyntaxAnalysisResult();
        QMessageBox::information(this, tr("成功"), tr("语法分析成功"));
    } else {
        displaySyntaxAnalysisResult();
        QMessageBox::warning(this, tr("错误"), tr("语法分析失败: %1").arg(result.errorMessage));
    }
}

void Task2Window::on_pushButtonSaveSyntaxTree_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存语法树"), ".", tr("PNG文件 (*.png);;JPEG文件 (*.jpg);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        if (m_syntaxAnalyzer->saveSyntaxTreeToImage(fileName)) {
            QMessageBox::information(this, tr("成功"), tr("语法树保存成功"));
        } else {
            QMessageBox::warning(this, tr("错误"), tr("语法树保存失败: %1").arg(m_syntaxAnalyzer->getErrorMessage()));
        }
    }
}

void Task2Window::on_comboBoxLR0View_currentIndexChanged(int index)
{
    // 切换LR(0) DFA的显示方式（图形化或表格化）
    displayLR0DFA();
}

void Task2Window::on_comboBoxLR1View_currentIndexChanged(int index)
{
    // 切换LR(1) DFA的显示方式（图形化或表格化）
    displayLR1DFA();
}

void Task2Window::displayFirstSet()
{
    // 清空表格
    ui->tableWidgetFirst->setRowCount(0);
    ui->tableWidgetFirst->setColumnCount(2);
    
    // 设置列名
    ui->tableWidgetFirst->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("非终结符")));
    ui->tableWidgetFirst->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("First集合")));
    
    // 获取First集合
    QMap<QString, QSet<QString>> firstMap = m_firstFollow->getAllFirst();
    
    // 填充表格
    int row = 0;
    for (const QString &nonTerminal : firstMap.keys()) {
        QSet<QString> firstSet = firstMap[nonTerminal];
        
        ui->tableWidgetFirst->insertRow(row);
        ui->tableWidgetFirst->setItem(row, 0, new QTableWidgetItem(nonTerminal));
        
        QString firstStr;
        if (firstSet.isEmpty()) {
            firstStr = "∅";
        } else {
            firstStr = firstSet.values().join(", ");
        }
        ui->tableWidgetFirst->setItem(row, 1, new QTableWidgetItem(firstStr));
        
        row++;
    }
    
    // 调整列宽
    ui->tableWidgetFirst->resizeColumnsToContents();
}

void Task2Window::displayFollowSet()
{
    // 清空表格
    ui->tableWidgetFollow->setRowCount(0);
    ui->tableWidgetFollow->setColumnCount(2);
    
    // 设置列名
    ui->tableWidgetFollow->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("非终结符")));
    ui->tableWidgetFollow->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Follow集合")));
    
    // 获取Follow集合
    QMap<QString, QSet<QString>> followMap = m_firstFollow->getAllFollow();
    
    // 填充表格
    int row = 0;
    for (const QString &nonTerminal : followMap.keys()) {
        QSet<QString> followSet = followMap[nonTerminal];
        
        ui->tableWidgetFollow->insertRow(row);
        ui->tableWidgetFollow->setItem(row, 0, new QTableWidgetItem(nonTerminal));
        
        QString followStr;
        if (followSet.isEmpty()) {
            followStr = "∅";
        } else {
            followStr = followSet.values().join(", ");
        }
        ui->tableWidgetFollow->setItem(row, 1, new QTableWidgetItem(followStr));
        
        row++;
    }
    
    // 调整列宽
    ui->tableWidgetFollow->resizeColumnsToContents();
}

void Task2Window::displayLR0DFA()
{
    // 清空表格
    ui->tableWidgetLR0->setRowCount(0);
    ui->tableWidgetLR0->setColumnCount(3);
    
    // 设置列名
    ui->tableWidgetLR0->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("状态ID")));
    ui->tableWidgetLR0->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("项目集")));
    ui->tableWidgetLR0->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("转移关系")));
    
    // 获取LR(0) DFA的项目集和转移表
    QList<LR0ItemSet> itemSets = m_lr0dfa->getItemSets();
    QList<LR0Transition> transitions = m_lr0dfa->getTransitions();
    
    // 填充表格
    for (int i = 0; i < itemSets.size(); ++i) {
        const LR0ItemSet &itemSet = itemSets[i];
        
        ui->tableWidgetLR0->insertRow(i);
        ui->tableWidgetLR0->setItem(i, 0, new QTableWidgetItem(QString::number(itemSet.stateId)));
        ui->tableWidgetLR0->setItem(i, 1, new QTableWidgetItem(itemSet.toString()));
        
        // 收集当前状态的所有转移
        QString transitionsStr;
        for (const LR0Transition &trans : transitions) {
            if (trans.fromState == itemSet.stateId) {
                if (!transitionsStr.isEmpty()) {
                    transitionsStr += "; ";
                }
                transitionsStr += QString("%1 -> %2").arg(trans.symbol).arg(trans.toState);
            }
        }
        ui->tableWidgetLR0->setItem(i, 2, new QTableWidgetItem(transitionsStr));
    }
    
    // 调整列宽
    ui->tableWidgetLR0->resizeColumnsToContents();
}

void Task2Window::displaySLR1Result()
{
    // 获取冲突信息
    QList<ConflictInfo> conflicts = m_slr1Checker->getConflicts();
    
    // 显示冲突信息
    QString conflictText;
    if (conflicts.isEmpty()) {
        conflictText = tr("该文法为SLR(1)文法，无冲突");
    } else {
        conflictText = tr("该文法不是SLR(1)文法，存在以下冲突：\n\n");
        for (const ConflictInfo &conflict : conflicts) {
            conflictText += conflict.toString() + "\n\n";
        }
    }
    
    ui->textBrowserSLR1Result->setText(conflictText);
}

void Task2Window::displayLR1DFA()
{
    // 清空表格
    ui->tableWidgetLR1->setRowCount(0);
    ui->tableWidgetLR1->setColumnCount(3);
    
    // 设置列名
    ui->tableWidgetLR1->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("状态ID")));
    ui->tableWidgetLR1->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("项目集")));
    ui->tableWidgetLR1->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("转移关系")));
    
    // 获取LR(1) DFA的项目集和转移表
    QList<LR1ItemSet> itemSets = m_lr1dfa->getItemSets();
    QList<LR1Transition> transitions = m_lr1dfa->getTransitions();
    
    // 填充表格
    for (int i = 0; i < itemSets.size(); ++i) {
        const LR1ItemSet &itemSet = itemSets[i];
        
        ui->tableWidgetLR1->insertRow(i);
        ui->tableWidgetLR1->setItem(i, 0, new QTableWidgetItem(QString::number(itemSet.stateId)));
        ui->tableWidgetLR1->setItem(i, 1, new QTableWidgetItem(itemSet.toString()));
        
        // 收集当前状态的所有转移
        QString transitionsStr;
        for (const LR1Transition &trans : transitions) {
            if (trans.fromState == itemSet.stateId) {
                if (!transitionsStr.isEmpty()) {
                    transitionsStr += "; ";
                }
                transitionsStr += QString("%1 -> %2").arg(trans.symbol).arg(trans.toState);
            }
        }
        ui->tableWidgetLR1->setItem(i, 2, new QTableWidgetItem(transitionsStr));
    }
    
    // 调整列宽
    ui->tableWidgetLR1->resizeColumnsToContents();
}

void Task2Window::displayLR1Table()
{
    // 清空表格
    ui->tableWidgetLR1Table->setRowCount(0);
    
    // 这里简化处理，只显示Action表的一部分
    ui->tableWidgetLR1Table->setColumnCount(2);
    ui->tableWidgetLR1Table->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("状态ID")));
    ui->tableWidgetLR1Table->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Action/Goto")));
    
    // 获取Action表和Goto表
    QMap<int, QMap<QString, Action>> actionTable = m_lr1Table->getActionTable();
    QMap<int, QMap<QString, int>> gotoTable = m_lr1Table->getGotoTable();
    
    // 填充表格
    int row = 0;
    for (int stateId : actionTable.keys()) {
        ui->tableWidgetLR1Table->insertRow(row);
        ui->tableWidgetLR1Table->setItem(row, 0, new QTableWidgetItem(QString::number(stateId)));
        
        // 显示Action表
        QString actionStr = tr("Action:\n");
        for (const QString &symbol : actionTable[stateId].keys()) {
            const Action &action = actionTable[stateId][symbol];
            actionStr += QString("%1: %2\n").arg(symbol).arg(action.toString());
        }
        
        // 显示Goto表
        actionStr += tr("\nGoto:\n");
        for (const QString &nonTerminal : gotoTable[stateId].keys()) {
            int gotoState = gotoTable[stateId][nonTerminal];
            actionStr += QString("%1: %2\n").arg(nonTerminal).arg(gotoState);
        }
        
        ui->tableWidgetLR1Table->setItem(row, 1, new QTableWidgetItem(actionStr));
        
        row++;
    }
    
    // 调整列宽
    ui->tableWidgetLR1Table->resizeColumnsToContents();
}

void Task2Window::displaySyntaxAnalysisResult()
{
    // 清空表格和语法树
    ui->tableWidgetAnalysisSteps->setRowCount(0);
    ui->treeWidgetSyntaxTree->clear();
    
    // 设置表格列名
    ui->tableWidgetAnalysisSteps->setColumnCount(5);
    ui->tableWidgetAnalysisSteps->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("步骤")));
    ui->tableWidgetAnalysisSteps->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("状态栈")));
    ui->tableWidgetAnalysisSteps->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("符号栈")));
    ui->tableWidgetAnalysisSteps->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("输入串")));
    ui->tableWidgetAnalysisSteps->setHorizontalHeaderItem(4, new QTableWidgetItem(tr("动作")));
    
    // 获取分析结果
    AnalysisResult result = m_syntaxAnalyzer->getResult();
    
    // 填充分析步骤表格
    for (int i = 0; i < result.steps.size(); ++i) {
        const AnalysisStep &step = result.steps[i];
        
        ui->tableWidgetAnalysisSteps->insertRow(i);
        ui->tableWidgetAnalysisSteps->setItem(i, 0, new QTableWidgetItem(QString::number(step.step)));
        ui->tableWidgetAnalysisSteps->setItem(i, 1, new QTableWidgetItem(step.stateStack));
        ui->tableWidgetAnalysisSteps->setItem(i, 2, new QTableWidgetItem(step.symbolStack));
        ui->tableWidgetAnalysisSteps->setItem(i, 3, new QTableWidgetItem(step.inputString));
        ui->tableWidgetAnalysisSteps->setItem(i, 4, new QTableWidgetItem(step.action));
    }
    
    // 填充语法树
    if (result.success) {
        QTreeWidgetItem *rootItem = result.syntaxTree.toTreeWidgetItem();
        ui->treeWidgetSyntaxTree->addTopLevelItem(rootItem);
        ui->treeWidgetSyntaxTree->expandAll();
    }
    
    // 调整列宽
    ui->tableWidgetAnalysisSteps->resizeColumnsToContents();
}

void Task2Window::initUI()
{
    // 初始化表格
    ui->tableWidgetFirst->setRowCount(0);
    ui->tableWidgetFollow->setRowCount(0);
    ui->tableWidgetLR0->setRowCount(0);
    ui->tableWidgetLR1->setRowCount(0);
    ui->tableWidgetLR1Table->setRowCount(0);
    ui->tableWidgetAnalysisSteps->setRowCount(0);
    
    // 初始化语法树
    ui->treeWidgetSyntaxTree->clear();
    
    // 初始化文本浏览器
    ui->textBrowserGrammarInfo->clear();
    ui->textBrowserSLR1Result->clear();
    
    // 初始化组合框
    ui->comboBoxLR0View->addItem(tr("表格化显示"));
    ui->comboBoxLR0View->addItem(tr("图形化显示"));
    ui->comboBoxLR1View->addItem(tr("表格化显示"));
    ui->comboBoxLR1View->addItem(tr("图形化显示"));
}

