#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // 初始化表格
    initTableWidget(ui->tableNFA, QStringList() << "状态" << "输入字符" << "目标状态");
    initTableWidget(ui->tableDFA, QStringList() << "状态集合" << "输入字符" << "目标状态集合");
    initTableWidget(ui->tableMinDFA, QStringList() << "状态" << "输入字符" << "目标状态");
    initTableWidget(ui->tableTestOutput, QStringList() << "行号" << "单词" << "编码");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 正则表达式模块
void MainWindow::on_btnOpenRegex_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开正则表达式文件"), ".", tr("文本文件 (*.txt)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            ui->textEditRegex->setPlainText(in.readAll());
            file.close();
        } else {
            QMessageBox::warning(this, tr("错误"), tr("无法打开文件：") + file.errorString());
        }
    }
}

void MainWindow::on_btnSaveRegex_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存正则表达式文件"), ".", tr("文本文件 (*.txt)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->textEditRegex->toPlainText();
            file.close();
            QMessageBox::information(this, tr("成功"), tr("文件已保存"));
        } else {
            QMessageBox::warning(this, tr("错误"), tr("无法保存文件：") + file.errorString());
        }
    }
}

void MainWindow::on_btnParseRegex_clicked()
{
    QString regexText = ui->textEditRegex->toPlainText();
    
    if (regexText.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请输入正则表达式"));
        return;
    }
    
    if (m_regexProcessor.parse(regexText)) {
        m_currentRegexItems = m_regexProcessor.getRegexItems();
        QString message = tr("成功解析 %1 个正则表达式\n\n").arg(m_currentRegexItems.size());
        for (const RegexItem &item : m_currentRegexItems) {
            message += tr("名称: %1\n模式: %2\n编码: %3\n是否多单词: %4\n\n")
                      .arg(item.name)
                      .arg(item.pattern)
                      .arg(item.code)
                      .arg(item.isMultiWord ? tr("是") : tr("否"));
        }
        QMessageBox::information(this, tr("成功"), message);
    } else {
        QMessageBox::warning(this, tr("错误"), tr("解析失败: %1").arg(m_regexProcessor.getErrorMessage()));
    }
}

void MainWindow::on_actionOpenRegex_triggered()
{
    on_btnOpenRegex_clicked();
}

void MainWindow::on_actionSaveRegex_triggered()
{
    on_btnSaveRegex_clicked();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

// NFA模块
void MainWindow::on_btnGenerateNFA_clicked()
{
    // 首先解析正则表达式
    QString regexText = ui->textEditRegex->toPlainText();
    if (regexText.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请输入正则表达式"));
        return;
    }
    
    if (!m_regexProcessor.parse(regexText)) {
        QMessageBox::warning(this, tr("错误"), tr("正则表达式解析失败: %1").arg(m_regexProcessor.getErrorMessage()));
        return;
    }
    
    QList<RegexItem> regexItems = m_regexProcessor.getRegexItems();
    if (regexItems.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("没有找到有效的正则表达式"));
        return;
    }
    
    // 为第一个正则表达式构建NFA
    m_currentNFA = m_nfaBuilder.buildNFA(regexItems.first());
    
    if (m_currentNFA.states.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("NFA构建失败: %1").arg(m_nfaBuilder.getErrorMessage()));
        return;
    }
    
    // 显示NFA
    displayNFA(m_currentNFA);
    QMessageBox::information(this, tr("成功"), tr("NFA生成成功！"));
}

// DFA模块
void MainWindow::on_btnGenerateDFA_clicked()
{
    // 检查是否已生成NFA
    if (m_currentNFA.states.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先生成NFA"));
        return;
    }
    
    // 将NFA转换为DFA
    m_currentDFA = m_dfaBuilder.convertNFAToDFA(m_currentNFA);
    
    if (m_currentDFA.states.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("DFA转换失败: %1").arg(m_dfaBuilder.getErrorMessage()));
        return;
    }
    
    // 显示DFA
    displayDFA(m_currentDFA);
    QMessageBox::information(this, tr("成功"), tr("DFA生成成功！"));
}

// 最小化DFA模块
void MainWindow::on_btnMinimizeDFA_clicked()
{
    // 检查是否已生成DFA
    if (m_currentDFA.states.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先生成DFA"));
        return;
    }
    
    // 最小化DFA
    m_currentMinimizedDFA = m_dfaMinimizer.minimizeDFA(m_currentDFA);
    
    if (m_currentMinimizedDFA.states.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("DFA最小化失败: %1").arg(m_dfaMinimizer.getErrorMessage()));
        return;
    }
    
    // 显示最小化DFA
    displayMinimizedDFA(m_currentMinimizedDFA);
    QMessageBox::information(this, tr("成功"), tr("DFA最小化成功！"));
}

// 词法分析器生成模块
void MainWindow::on_btnGenerateLexer_clicked()
{
    ui->statusbar->showMessage("正在生成词法分析器...", 0);
    
    // 检查是否已生成最小化DFA
    if (m_currentMinimizedDFA.states.isEmpty()) {
        ui->statusbar->showMessage("词法分析器生成失败", 3000);
        QMessageBox::warning(this, tr("警告"), tr("请先完成DFA最小化"));
        return;
    }
    
    // 检查是否有解析后的正则表达式
    if (m_currentRegexItems.isEmpty()) {
        ui->statusbar->showMessage("词法分析器生成失败", 3000);
        QMessageBox::warning(this, tr("警告"), tr("请先解析正则表达式"));
        return;
    }
    
    // 默认使用表驱动方法
    GenerationMethod method = GenerationMethod::STATE_TRANSITION;
    
    // 生成词法分析器代码
    QString lexerCode = m_lexerGenerator.generateLexer(m_currentRegexItems, m_currentMinimizedDFA, method);
    
    if (lexerCode.isEmpty()) {
        ui->statusbar->showMessage("词法分析器生成失败", 3000);
        QMessageBox::warning(this, tr("生成失败"), tr("词法分析器生成失败！\n错误：\n") + m_lexerGenerator.getErrorMessage());
    } else {
        ui->statusbar->showMessage("词法分析器生成完成", 3000);
        ui->textEditLexerCode->setPlainText(lexerCode);
    }
}

void MainWindow::on_btnSaveLexer_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存词法分析器"), ".", tr("C++文件 (*.cpp)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->textEditLexerCode->toPlainText();
            file.close();
            QMessageBox::information(this, tr("成功"), tr("文件已保存"));
        } else {
            QMessageBox::warning(this, tr("错误"), tr("无法保存文件：") + file.errorString());
        }
    }
}

// 测试模块
void MainWindow::on_btnOpenTestFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开测试文件"), ".", tr("文本文件 (*.txt);;Tiny文件 (*.tny)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            ui->textEditTestInput->setPlainText(in.readAll());
            file.close();
        } else {
            QMessageBox::warning(this, tr("错误"), tr("无法打开文件：") + file.errorString());
        }
    }
}

void MainWindow::on_btnRunLexer_clicked()
{
    // 编译词法分析器
    QString lexerCode = ui->textEditLexerCode->toPlainText();
    if (lexerCode.isEmpty()) {
        ui->statusbar->showMessage("请先生成词法分析器代码", 3000);
        return;
    }
    
    ui->statusbar->showMessage("正在编译词法分析器...", 0);
    
    // 编译词法分析器代码
    bool success = m_lexerTester.compileLexer(lexerCode, "lexer");
    
    if (success) {
        ui->statusbar->showMessage("词法分析器编译成功", 3000);
        QMessageBox::information(this, tr("编译成功"), tr("词法分析器编译成功！\n输出：\n") + m_lexerTester.getCompileOutput());
    } else {
        ui->statusbar->showMessage("词法分析器编译失败", 3000);
        QMessageBox::warning(this, tr("编译失败"), tr("词法分析器编译失败！\n错误：\n") + m_lexerTester.getError());
    }
}

void MainWindow::on_btnSaveTestOutput_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存测试结果"), ".", tr("文本文件 (*.txt)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            // 从表格中读取数据并保存
            for (int row = 0; row < ui->tableTestOutput->rowCount(); ++row) {
                for (int col = 0; col < ui->tableTestOutput->columnCount(); ++col) {
                    QTableWidgetItem *item = ui->tableTestOutput->item(row, col);
                    if (item) {
                        out << item->text();
                        if (col < ui->tableTestOutput->columnCount() - 1) {
                            out << "\t";
                        }
                    }
                }
                out << "\n";
            }
            file.close();
            QMessageBox::information(this, tr("成功"), tr("文件已保存"));
        } else {
            QMessageBox::warning(this, tr("错误"), tr("无法保存文件：") + file.errorString());
        }
    }
}

// 该函数已被移除，因为UI中没有对应的按钮
// void MainWindow::on_btnTestLexer_clicked()
// {
//     // 测试词法分析器
//     QString sourceFile = QFileDialog::getOpenFileName(this, tr("打开源文件"), ".", tr("所有文件 (*);;Tiny文件 (*.tny);;C文件 (*.c)") );
//     if (sourceFile.isEmpty()) {
//         return;
//     }
//     
//     ui->statusbar->showMessage("正在进行词法分析...", 0);
//     
//     // 执行词法分析
//     m_currentLexicalResults = m_lexerTester.testLexer(sourceFile);
//     
//     if (m_currentLexicalResults.isEmpty() && !m_lexerTester.getError().isEmpty()) {
//         ui->statusbar->showMessage("词法分析执行失败", 3000);
//         QMessageBox::warning(this, tr("执行失败"), tr("词法分析执行失败！\n错误：\n") + m_lexerTester.getError());
//     } else {
//         ui->statusbar->showMessage("词法分析完成", 3000);
//         // ui->txtTestOutput->setPlainText(m_lexerTester.getTestOutput());
//         displayLexicalResults(m_currentLexicalResults);
//     }
// }

// 辅助函数
void MainWindow::initTableWidget(QTableWidget *table, const QStringList &headers)
{
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::addTableRow(QTableWidget *table, const QStringList &data)
{
    int row = table->rowCount();
    table->insertRow(row);
    for (int col = 0; col < data.size() && col < table->columnCount(); ++col) {
        table->setItem(row, col, new QTableWidgetItem(data[col]));
    }
}

void MainWindow::clearTable(QTableWidget *table)
{
    table->setRowCount(0);
}

void MainWindow::displayNFA(const NFA &nfa)
{
    clearTable(ui->tableNFA);
    
    // 显示所有转换
    for (const NFATransition &transition : nfa.transitions) {
        QStringList rowData;
        rowData << QString::number(transition.fromState)
                << (transition.input == "#" ? "ε" : transition.input)
                << QString::number(transition.toState);
        addTableRow(ui->tableNFA, rowData);
    }
}

void MainWindow::displayDFA(const DFA &dfa)
{
    clearTable(ui->tableDFA);
    
    // 显示所有转换
    for (const DFATransition &transition : dfa.transitions) {
        QStringList rowData;
        rowData << QString::number(transition.fromState)
                << transition.input
                << QString::number(transition.toState);
        addTableRow(ui->tableDFA, rowData);
    }
}

void MainWindow::displayMinimizedDFA(const DFA &dfa)
{
    clearTable(ui->tableMinDFA);
    
    // 显示所有转换
    for (const DFATransition &transition : dfa.transitions) {
        QStringList rowData;
        rowData << QString::number(transition.fromState)
                << transition.input
                << QString::number(transition.toState);
        addTableRow(ui->tableMinDFA, rowData);
    }
}

void MainWindow::displayLexicalResults(const QList<LexicalResult> &results)
{
    // 清空表格
    ui->tableTestOutput->clearContents();
    ui->tableTestOutput->setRowCount(0);
    
    // 设置表头
    ui->tableTestOutput->setColumnCount(3);
    ui->tableTestOutput->setHorizontalHeaderLabels(QStringList() << "行号" << "单词" << "编码");
    
    // 显示词法分析结果
    int row = 0;
    for (const auto &result : results) {
        ui->tableTestOutput->insertRow(row);
        ui->tableTestOutput->setItem(row, 0, new QTableWidgetItem(QString::number(result.line)));
        ui->tableTestOutput->setItem(row, 1, new QTableWidgetItem(result.lexeme));
        ui->tableTestOutput->setItem(row, 2, new QTableWidgetItem(QString::number(result.tokenCode)));
        row++;
    }
    
    // 调整列宽
    ui->tableTestOutput->resizeColumnsToContents();
}
