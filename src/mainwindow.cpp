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
    
    // 初始化成员变量
    m_currentRegexName = "";
    m_isTotalView = false;
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
        
        // 更新正则表达式下拉列表
        updateRegexComboBox();
        
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
    
    // 清空之前的NFA映射
    m_nfaMap.clear();
    
    // 为每个以下划线开头的正则表达式构建NFA
    int generatedCount = 0;
    for (const RegexItem &item : regexItems) {
        // 仅为下划线开头的正则表达式生成NFA
        if (item.name.startsWith('_')) {
            NFA nfa = m_nfaBuilder.buildNFA(item);
            if (!nfa.states.isEmpty()) {
                m_nfaMap[item.name] = nfa;
                generatedCount++;
            }
        }
    }
    
    if (generatedCount == 0) {
        QMessageBox::warning(this, tr("错误"), tr("NFA构建失败: %1").arg(m_nfaBuilder.getErrorMessage()));
        return;
    }
    
    // 更新正则表达式下拉列表
    updateRegexComboBox();
    
    // 如果当前没有选中的正则表达式，选择第一个
    if (m_currentRegexName.isEmpty() && !m_nfaMap.isEmpty()) {
        m_currentRegexName = m_nfaMap.keys().first();
    }
    
    // 生成总NFA
    if (generatedCount > 0) {
        m_totalNFA = mergeNFAs(m_nfaMap.values());
    }
    
    // 更新NFA显示
    updateNFADisplay();
    
    QMessageBox::information(this, tr("成功"), tr("成功生成 %1 个NFA！").arg(generatedCount));
}

// DFA模块
NFA MainWindow::mergeNFAs(const QList<NFA> &nfAs)
{
    NFA totalNFA;
    if (nfAs.isEmpty()) {
        return totalNFA;
    }
    
    // 状态偏移量，用于确保每个NFA的状态编号不冲突
    int stateOffset = 0;
    
    // 收集所有NFA的起始状态（偏移后）
    QList<NFAState> allStartStates;
    
    // 合并所有NFA的状态、转移和接受状态
    for (const NFA &nfa : nfAs) {
        // 处理状态
        for (const NFAState &state : nfa.states) {
            totalNFA.states.append(state + stateOffset);
        }
        
        // 处理转移
        for (const NFATransition &transition : nfa.transitions) {
            NFATransition newTransition;
            newTransition.fromState = transition.fromState + stateOffset;
            newTransition.input = transition.input;
            newTransition.toState = transition.toState + stateOffset;
            totalNFA.transitions.append(newTransition);
        }
        
        // 处理接受状态
        for (const NFAState &acceptState : nfa.acceptStates) {
            totalNFA.acceptStates.insert(acceptState + stateOffset);
        }
        
        // 记录偏移后的起始状态
        allStartStates.append(nfa.startState + stateOffset);
        
        // 更新状态偏移量
        stateOffset += nfa.states.size();
    }
    
    // 创建总起始状态
    NFAState totalStartState = stateOffset;
    totalNFA.startState = totalStartState;
    totalNFA.states.append(totalStartState);
    
    // 从总起始状态添加ε转移到每个NFA的起始状态
    for (const NFAState &startState : allStartStates) {
        NFATransition transition;
        transition.fromState = totalStartState;
        transition.input = "#"; // ε转移
        transition.toState = startState;
        totalNFA.transitions.append(transition);
    }
    
    return totalNFA;
}

void MainWindow::on_btnGenerateDFA_clicked()
{
    // 检查是否已生成NFA
    if (m_nfaMap.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先生成NFA"));
        return;
    }
    
    // 清空之前的DFA映射
    m_dfaMap.clear();
    
    // 为每个NFA生成DFA
    for (auto it = m_nfaMap.constBegin(); it != m_nfaMap.constEnd(); ++it) {
        const QString &regexName = it.key();
        const NFA &nfa = it.value();
        
        DFA dfa = m_dfaBuilder.convertNFAToDFA(nfa);
        if (!dfa.states.isEmpty()) {
            m_dfaMap[regexName] = dfa;
        }
    }
    
    // 生成总DFA
    if (!m_totalNFA.states.isEmpty()) {
        m_totalDFA = m_dfaBuilder.convertNFAToDFA(m_totalNFA);
    }
    
    // 更新DFA显示
    updateDFADisplay();
    
    QMessageBox::information(this, tr("成功"), tr("DFA生成成功！"));
}

// 最小化DFA模块
void MainWindow::on_btnMinimizeDFA_clicked()
{
    // 检查是否已生成DFA
    if (m_dfaMap.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先生成DFA"));
        return;
    }
    
    // 清空之前的最小化DFA映射
    m_minimizedDfaMap.clear();
    
    // 为每个DFA生成最小化DFA
    for (auto it = m_dfaMap.constBegin(); it != m_dfaMap.constEnd(); ++it) {
        const QString &regexName = it.key();
        const DFA &dfa = it.value();
        
        DFA minimizedDFA = m_dfaMinimizer.minimizeDFA(dfa);
        if (!minimizedDFA.states.isEmpty()) {
            m_minimizedDfaMap[regexName] = minimizedDFA;
        }
    }
    
    // 生成总最小化DFA
    if (!m_totalDFA.states.isEmpty()) {
        m_totalMinimizedDFA = m_dfaMinimizer.minimizeDFA(m_totalDFA);
    }
    
    // 更新最小化DFA显示
    updateMinimizedDFADisplay();
    
    QMessageBox::information(this, tr("成功"), tr("DFA最小化成功！"));
}

// 词法分析器生成模块
void MainWindow::on_btnGenerateLexer_clicked()
{
    ui->statusbar->showMessage("正在生成词法分析器...", 0);
    
    // 检查是否已生成最小化DFA
    if (m_minimizedDfaMap.isEmpty() || m_currentRegexName.isEmpty() || !m_minimizedDfaMap.contains(m_currentRegexName)) {
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
    QString lexerCode = m_lexerGenerator.generateLexer(m_currentRegexItems, m_minimizedDfaMap[m_currentRegexName], method);
    
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
    
    // 收集所有转移名称（输入字符）
    QSet<QString> transitions;
    for (const NFATransition &transition : nfa.transitions) {
        transitions.insert(transition.input);
    }
    
    // 将转移名称转换为列表并排序
    QStringList transitionList = transitions.values();
    std::sort(transitionList.begin(), transitionList.end());
    
    // 设置表头
    QStringList headers;
    headers << "状态标记" << "状态编号";
    for (const QString &transition : transitionList) {
        headers << (transition == "#" ? "#" : transition);
    }
    ui->tableNFA->setColumnCount(headers.size());
    ui->tableNFA->setHorizontalHeaderLabels(headers);
    
    // 收集所有状态并排序
    QList<NFAState> allStates = nfa.states;
    std::sort(allStates.begin(), allStates.end());
    
    // 为每个状态创建一行
    for (const NFAState &state : allStates) {
        QStringList rowData;
        
        // 第一列：状态标记
        QString stateMark = "";
        if (state == nfa.startState) {
            stateMark += "-";
        }
        if (nfa.acceptStates.contains(state)) {
            stateMark += "+";
        }
        rowData << stateMark;
        
        // 第二列：状态编号
        rowData << QString::number(state);
        
        // 后续列：转移对应的目标状态
        for (const QString &transition : transitionList) {
            QString targetStates = "";
            for (const NFATransition &t : nfa.transitions) {
                if (t.fromState == state && t.input == transition) {
                    if (!targetStates.isEmpty()) {
                        targetStates += ",";
                    }
                    targetStates += QString::number(t.toState);
                }
            }
            rowData << targetStates;
        }
        
        addTableRow(ui->tableNFA, rowData);
    }
    
    // 调整列宽
    ui->tableNFA->resizeColumnsToContents();
}

void MainWindow::displayDFA(const DFA &dfa)
{
    clearTable(ui->tableDFA);
    
    // 收集所有转移名称（输入字符）
    QSet<QString> transitions;
    for (const DFATransition &transition : dfa.transitions) {
        transitions.insert(transition.input);
    }
    
    // 将转移名称转换为列表并排序
    QStringList transitionList = transitions.values();
    std::sort(transitionList.begin(), transitionList.end());
    
    // 设置表头
    QStringList headers;
    headers << "状态标记" << "状态编号";
    for (const QString &transition : transitionList) {
        headers << (transition == "#" ? "#" : transition);
    }
    ui->tableDFA->setColumnCount(headers.size());
    ui->tableDFA->setHorizontalHeaderLabels(headers);
    
    // 收集所有状态并排序
    QList<DFAState> allStates = dfa.states;
    std::sort(allStates.begin(), allStates.end());
    
    // 为每个状态创建一行
    for (const DFAState &state : allStates) {
        QStringList rowData;
        
        // 第一列：状态标记
        QString stateMark = "";
        if (state == dfa.startState) {
            stateMark += "-";
        }
        if (dfa.acceptStates.contains(state)) {
            stateMark += "+";
        }
        rowData << stateMark;
        
        // 第二列：状态编号
        rowData << QString::number(state);
        
        // 后续列：转移对应的目标状态
        for (const QString &transition : transitionList) {
            QString targetState = "";
            for (const DFATransition &t : dfa.transitions) {
                if (t.fromState == state && t.input == transition) {
                    targetState = QString::number(t.toState);
                    break;
                }
            }
            rowData << targetState;
        }
        
        addTableRow(ui->tableDFA, rowData);
    }
    
    // 调整列宽
    ui->tableDFA->resizeColumnsToContents();
}

void MainWindow::displayMinimizedDFA(const DFA &dfa)
{
    clearTable(ui->tableMinDFA);
    
    // 收集所有转移名称（输入字符）
    QSet<QString> transitions;
    for (const DFATransition &transition : dfa.transitions) {
        transitions.insert(transition.input);
    }
    
    // 将转移名称转换为列表并排序
    QStringList transitionList = transitions.values();
    std::sort(transitionList.begin(), transitionList.end());
    
    // 设置表头
    QStringList headers;
    headers << "状态标记" << "状态编号";
    for (const QString &transition : transitionList) {
        headers << (transition == "#" ? "#" : transition);
    }
    ui->tableMinDFA->setColumnCount(headers.size());
    ui->tableMinDFA->setHorizontalHeaderLabels(headers);
    
    // 收集所有状态
    QSet<DFAState> allStates;
    for (const DFAState &state : dfa.states) {
        allStates.insert(state);
    }
    
    // 为每个状态创建一行
    for (const DFAState &state : allStates) {
        QStringList rowData;
        
        // 第一列：状态标记
        QString stateMark = "";
        if (state == dfa.startState) {
            stateMark += "-";
        }
        if (dfa.acceptStates.contains(state)) {
            stateMark += "+";
        }
        rowData << stateMark;
        
        // 第二列：状态编号
        rowData << QString::number(state);
        
        // 后续列：转移对应的目标状态
        for (const QString &transition : transitionList) {
            QString targetState = "";
            for (const DFATransition &t : dfa.transitions) {
                if (t.fromState == state && t.input == transition) {
                    targetState = QString::number(t.toState);
                    break;
                }
            }
            rowData << targetState;
        }
        
        addTableRow(ui->tableMinDFA, rowData);
    }
    
    // 调整列宽
    ui->tableMinDFA->resizeColumnsToContents();
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

// 更新正则表达式下拉列表
void MainWindow::updateRegexComboBox()
{
    // 清空现有选项
    ui->comboBoxRegex->clear();
    
    // 添加总表选项
    ui->comboBoxRegex->addItem("总表");
    
    // 添加所有以下划线开头的正则表达式名称
    for (const RegexItem &item : m_currentRegexItems) {
        if (item.name.startsWith('_')) {
            ui->comboBoxRegex->addItem(item.name);
        }
    }
    
    // 如果当前选中的正则表达式名称存在，设置为当前选项
    if (m_isTotalView) {
        ui->comboBoxRegex->setCurrentText("总表");
    } else if (!m_currentRegexName.isEmpty() && ui->comboBoxRegex->findText(m_currentRegexName) != -1) {
        ui->comboBoxRegex->setCurrentText(m_currentRegexName);
    }
}

// 更新NFA显示
void MainWindow::updateNFADisplay()
{
    if (m_isTotalView) {
        // 显示总NFA
        displayNFA(m_totalNFA);
    } else {
        // 检查当前选中的正则表达式名称是否有对应的NFA
        if (m_currentRegexName.isEmpty() || !m_nfaMap.contains(m_currentRegexName)) {
            clearTable(ui->tableNFA);
            return;
        }
        
        // 显示当前选中的NFA
        const NFA &nfa = m_nfaMap[m_currentRegexName];
        displayNFA(nfa);
    }
}

// 更新DFA显示
void MainWindow::updateDFADisplay()
{
    if (m_isTotalView) {
        // 显示总DFA
        displayDFA(m_totalDFA);
    } else {
        // 检查当前选中的正则表达式名称是否有对应的DFA
        if (m_currentRegexName.isEmpty() || !m_dfaMap.contains(m_currentRegexName)) {
            clearTable(ui->tableDFA);
            return;
        }
        
        // 显示当前选中的DFA
        const DFA &dfa = m_dfaMap[m_currentRegexName];
        displayDFA(dfa);
    }
}

// 更新最小化DFA显示
void MainWindow::updateMinimizedDFADisplay()
{
    if (m_isTotalView) {
        // 显示总最小化DFA
        displayMinimizedDFA(m_totalMinimizedDFA);
    } else {
        // 检查当前选中的正则表达式名称是否有对应的最小化DFA
        if (m_currentRegexName.isEmpty() || !m_minimizedDfaMap.contains(m_currentRegexName)) {
            clearTable(ui->tableMinDFA);
            return;
        }
        
        // 显示当前选中的最小化DFA
        const DFA &minimizedDFA = m_minimizedDfaMap[m_currentRegexName];
        displayMinimizedDFA(minimizedDFA);
    }
}

// 正则表达式选择切换
void MainWindow::on_comboBoxRegex_currentIndexChanged(const QString &arg1)
{
    m_currentRegexName = arg1;
    m_isTotalView = (arg1 == "总表");
    
    // 更新NFA、DFA和最小化DFA的显示
    updateNFADisplay();
    updateDFADisplay();
    updateMinimizedDFADisplay();
}
