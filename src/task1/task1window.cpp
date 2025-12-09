#include "task1/task1window.h"
#include "ui_task1window.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <utility>

Task1Window::Task1Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Task1Window)
    , m_dynamicTableNFA(nullptr)
    , m_dynamicTableDFA(nullptr)
    , m_dynamicTableMinDFA(nullptr)
{
    ui->setupUi(this);
    
    // 创建动态表格
    createDynamicTables();
    
    // 初始化测试输出文本框（替换表格）
    m_textEditTestOutput = new QTextEdit(this);
    m_textEditTestOutput->setObjectName("textEditTestOutput");
    m_textEditTestOutput->setReadOnly(true);
    m_textEditTestOutput->setMinimumHeight(300);
    
    // 移除原来的表格
    QHBoxLayout *layout = ui->horizontalLayout_3;
    if (layout) {
        layout->removeWidget(ui->tableTestOutput);
        ui->tableTestOutput->setParent(nullptr);
        delete ui->tableTestOutput;
        layout->addWidget(m_textEditTestOutput);
    }
    
    // 初始化成员变量
    m_currentRegexName = "";
    m_isTotalView = false;
    
    // 添加信号槽连接
    connect(ui->comboBoxNFA, SIGNAL(currentTextChanged(const QString &)), this, SLOT(on_comboBoxNFA_currentIndexChanged(const QString &)));
    connect(ui->comboBoxDFA, SIGNAL(currentTextChanged(const QString &)), this, SLOT(on_comboBoxDFA_currentIndexChanged(const QString &)));
    connect(ui->comboBoxMinDFA, SIGNAL(currentTextChanged(const QString &)), this, SLOT(on_comboBoxMinDFA_currentIndexChanged(const QString &)));
    
    // 创建刷新按钮
    QPushButton *btnRefreshNFA = new QPushButton(tr("刷新NFA"), this);
    btnRefreshNFA->setObjectName("btnRefreshNFA");
    connect(btnRefreshNFA, SIGNAL(clicked()), this, SLOT(on_btnRefreshNFA_clicked()));
    
    QPushButton *btnRefreshDFA = new QPushButton(tr("刷新DFA"), this);
    btnRefreshDFA->setObjectName("btnRefreshDFA");
    connect(btnRefreshDFA, SIGNAL(clicked()), this, SLOT(on_btnRefreshDFA_clicked()));
    
    QPushButton *btnRefreshMinDFA = new QPushButton(tr("刷新最小DFA"), this);
    btnRefreshMinDFA->setObjectName("btnRefreshMinDFA");
    connect(btnRefreshMinDFA, SIGNAL(clicked()), this, SLOT(on_btnRefreshMinDFA_clicked()));
    
    // 找到NFA标签页的水平布局并添加刷新按钮
    QHBoxLayout *nfaLayout = qobject_cast<QHBoxLayout*>(ui->tabNFA->layout()->itemAt(0)->layout());
    if (nfaLayout) {
        nfaLayout->addWidget(btnRefreshNFA);
    }
    
    // 找到DFA标签页的水平布局并添加刷新按钮
    QHBoxLayout *dfaLayout = qobject_cast<QHBoxLayout*>(ui->tabDFA->layout()->itemAt(0)->layout());
    if (dfaLayout) {
        dfaLayout->addWidget(btnRefreshDFA);
    }
    
    // 找到最小化DFA标签页的水平布局并添加刷新按钮
    QHBoxLayout *minDfaLayout = qobject_cast<QHBoxLayout*>(ui->tabMinDFA->layout()->itemAt(0)->layout());
    if (minDfaLayout) {
        minDfaLayout->addWidget(btnRefreshMinDFA);
    }
}

Task1Window::~Task1Window()
{
    cleanupDynamicTables();
    delete ui;
}

// 正则表达式模块
void Task1Window::on_btnOpenRegex_clicked()
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

void Task1Window::on_btnSaveRegex_clicked()
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

void Task1Window::on_btnParseRegex_clicked()
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

void Task1Window::on_actionOpenRegex_triggered()
{
    on_btnOpenRegex_clicked();
}

void Task1Window::on_actionSaveRegex_triggered()
{
    on_btnSaveRegex_clicked();
}

void Task1Window::on_actionExit_triggered()
{
    close();
}

// NFA模块
void Task1Window::on_btnGenerateNFA_clicked()
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
    m_currentRegexItems = regexItems;
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
    
    // 如果当前没有选中的正则表达式，选择第一个
    if (m_currentRegexName.isEmpty() && !m_nfaMap.isEmpty()) {
        m_currentRegexName = m_nfaMap.keys().first();
    }
    
    // 更新正则表达式下拉列表
    updateRegexComboBox();
    
    // 生成总NFA
    if (generatedCount > 0) {
        m_totalNFA = mergeNFAs(m_nfaMap.values());
    }
    
    // 更新NFA显示
    updateNFADisplay();
    
    QMessageBox::information(this, tr("成功"), tr("成功生成 %1 个NFA！").arg(generatedCount));
}

// DFA模块
NFA Task1Window::mergeNFAs(const QList<NFA> &nfAs)
{
    NFA totalNFA;
    if (nfAs.isEmpty()) {
        return totalNFA;
    }
    
    // 状态偏移量，用于确保每个NFA的状态编号不冲突
    int stateOffset = 0;
    
    // 收集所有NFA的起始状态（偏移后）
    QList<NFAState> allStartStates;
    
    // 合并所有NFA的状态、转移、接受状态和字母表
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
        
        // 处理字母表 - 合并所有NFA的字母表
        totalNFA.alphabet.unite(nfa.alphabet);
        
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
    
    // 构建总NFA的邻接表
    totalNFA.transitionTable.clear();
    for (const NFATransition &transition : totalNFA.transitions) {
        totalNFA.transitionTable[transition.fromState][transition.input].insert(transition.toState);
    }
    
    return totalNFA;
}

void Task1Window::on_btnGenerateDFA_clicked()
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
void Task1Window::on_btnMinimizeDFA_clicked()
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
void Task1Window::on_btnGenerateLexer_clicked()
{
    ui->statusbar->showMessage("正在生成词法分析器...", 0);
    
    // 检查是否已生成最小化DFA
    if ((m_isTotalView && m_totalMinimizedDFA.states.isEmpty()) || 
        (!m_isTotalView && (m_minimizedDfaMap.isEmpty() || m_currentRegexName.isEmpty() || !m_minimizedDfaMap.contains(m_currentRegexName)))) {
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
    QString lexerCode;
    if (m_isTotalView) {
        lexerCode = m_lexerGenerator.generateLexer(m_currentRegexItems, m_totalMinimizedDFA, method);
    } else {
        lexerCode = m_lexerGenerator.generateLexer(m_currentRegexItems, m_minimizedDfaMap[m_currentRegexName], method);
    }
    
    if (lexerCode.isEmpty()) {
        ui->statusbar->showMessage("词法分析器生成失败", 3000);
        QMessageBox::warning(this, tr("生成失败"), tr("词法分析器生成失败！\n错误：\n") + m_lexerGenerator.getErrorMessage());
    } else {
        ui->statusbar->showMessage("词法分析器生成完成", 3000);
        ui->textEditLexerCode->setPlainText(lexerCode);
    }
}

void Task1Window::on_btnSaveLexer_clicked()
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
void Task1Window::on_btnOpenTestFile_clicked()
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

void Task1Window::on_btnRunLexer_clicked()
{
    // 编译词法分析器
    QString lexerCode = ui->textEditLexerCode->toPlainText();
    if (lexerCode.isEmpty()) {
        ui->statusbar->showMessage("请先生成词法分析器代码", 3000);
        return;
    }
    
    // 获取测试输入
    QString testInput = ui->textEditTestInput->toPlainText();
    if (testInput.isEmpty()) {
        ui->statusbar->showMessage("请输入测试内容", 3000);
        return;
    }
    
    ui->statusbar->showMessage("正在编译词法分析器...", 0);
    
    // 编译词法分析器代码
    bool success = m_lexerTester.compileLexer(lexerCode, "lexer");
    
    if (success) {
        ui->statusbar->showMessage("词法分析器编译成功，正在进行词法分析...", 0);
        
        // 保存测试输入到临时文件
        QString tempFile = "temp_test.txt";
        QFile file(tempFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << testInput;
            file.close();
            
            // 运行词法分析器
            m_currentLexicalResults = m_lexerTester.testLexer(tempFile);
            
            if (m_currentLexicalResults.isEmpty() && !m_lexerTester.getError().isEmpty()) {
                ui->statusbar->showMessage("词法分析执行失败", 3000);
                QMessageBox::warning(this, tr("执行失败"), tr("词法分析执行失败！\n错误：\n") + m_lexerTester.getError());
            } else {
                ui->statusbar->showMessage("词法分析完成", 3000);
                displayLexicalResults(m_currentLexicalResults);
            }
            
            // 删除临时文件
            QFile::remove(tempFile);
        }
    } else {
        ui->statusbar->showMessage("词法分析器编译失败", 3000);
        QMessageBox::warning(this, tr("编译失败"), tr("词法分析器编译失败！\n错误：\n") + m_lexerTester.getError());
    }
}

void Task1Window::on_btnSaveTestOutput_clicked()
{
    // 直接保存到sample.lex文件
    QString fileName = "sample.lex";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        // 从文本框中读取数据并保存
        out << m_textEditTestOutput->toPlainText();
        file.close();
        QMessageBox::information(this, tr("成功"), tr("结果已保存到sample.lex文件"));
    } else {
        QMessageBox::warning(this, tr("错误"), tr("无法保存文件：") + file.errorString());
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
void Task1Window::initTableWidget(QTableWidget *table, const QStringList &headers)
{
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void Task1Window::addTableRow(QTableWidget *table, const QStringList &data)
{
    int row = table->rowCount();
    table->insertRow(row);
    for (int col = 0; col < data.size() && col < table->columnCount(); ++col) {
        table->setItem(row, col, new QTableWidgetItem(data[col]));
    }
}

void Task1Window::clearTable(QTableWidget *table)
{
    table->setRowCount(0);
}

// 处理正则表达式引用，返回字符到引用名称的映射
QMap<QString, QString> Task1Window::processRegexReferences(const QList<RegexItem> &/*regexItems*/)
{
    QMap<QString, QString> charToRefMap;
    
    // 从正则表达式文本中提取引用定义
    QString regexText = ui->textEditRegex->toPlainText();
    QStringList lines = regexText.split("\n");
    
    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        // 移除可能存在的回车符
        trimmedLine = trimmedLine.replace("\r", "");
        if (trimmedLine.isEmpty() || trimmedLine.startsWith("//")) {
            continue; // 跳过空行和注释行
        }
        
        // 寻找等号位置
        int equalsPos = trimmedLine.indexOf('=');
        if (equalsPos == -1) {
            continue; // 跳过无效行
        }
        
        // 提取名称和模式
        QString name = trimmedLine.left(equalsPos).trimmed();
        QString pattern = trimmedLine.mid(equalsPos + 1).trimmed();
        
        // 只处理不以下划线开头的名称
            if (!name.startsWith('_')) {
                // 处理字符集格式，如 [a-z] 或 [0-9] 或 [!@#$%^&*()]
                if (pattern.startsWith('[') && pattern.endsWith(']')) {
                    // 提取字符集内容
                    QString charset = pattern.mid(1, pattern.length() - 2);
                    
                    // 使用循环处理字符集中的所有字符和范围
                    int i = 0;
                    while (i < charset.length()) {
                        QChar c = charset.at(i);
                        
                        // 处理转义字符
                        if (c == '\\' && i < charset.length() - 1) {
                            // 下一个字符是转义字符
                            QChar nextC = charset.at(i + 1);
                            if (nextC == 't') {
                                charToRefMap[QString('\t')] = name;
                            } else if (nextC == 'n') {
                                charToRefMap[QString('\n')] = name;
                            } else if (nextC == 'r') {
                                charToRefMap[QString('\r')] = name;
                            } else if (nextC == '\\') {
                                charToRefMap[QString('\\')] = name;
                            } else {
                                // 其他转义字符，如 \a, \b, \f, \v
                                charToRefMap[nextC] = name;
                            }
                            i += 2; // 跳过转义字符和被转义的字符
                        }
                        // 处理范围格式，如 a-z, A-Z, 0-9
                        else if (i + 2 < charset.length() && charset.at(i + 1) == '-') {
                            char start = charset.at(i).toLatin1();
                            char end = charset.at(i + 2).toLatin1();
                            
                            // 添加范围内的所有字符
                            for (char c = start; c <= end; ++c) {
                                charToRefMap[QString(c)] = name;
                            }
                            
                            i += 3; // 跳过范围：start + '-' + end
                        } else {
                            // 处理单个字符
                            charToRefMap[QString(c)] = name;
                            i += 1;
                        }
                    }
                } else if (pattern.length() == 1) {
                    // 处理单个字符
                    charToRefMap[pattern] = name;
                }
        }
    }
    
    return charToRefMap;
}

// 合并相同引用名称的转移
QMap<QString, QList<QString>> Task1Window::mergeTransitionsByRef(const QSet<QString> &transitions, const QMap<QString, QString> &charToRefMap)
{
    QMap<QString, QList<QString>> refToCharsMap;
    
    // 遍历所有转移字符
    for (const QString &transition : transitions) {
        if (transition == "#") {
            // 保持空转移不变
            refToCharsMap["#"] = {"#"};
        } else if (charToRefMap.contains(transition)) {
            // 将字符映射到引用名称
            QString refName = charToRefMap[transition];
            refToCharsMap[refName].append(transition);
        } else {
            // 没有引用的字符保持不变
            refToCharsMap[transition] = {transition};
        }
    }
    
    return refToCharsMap;
}

void Task1Window::displayNFA(const NFA &nfa, QTableWidget *table)
{
    clearTable(table);
    
    // 收集所有转移名称（输入字符）
    QSet<QString> transitions;
    for (const NFATransition &transition : nfa.transitions) {
        transitions.insert(transition.input);
    }
    
    // 检查当前处理的正则表达式是否为keyword（不区分大小写）
    bool isKeywordRegex = false;
    if (!m_isTotalView && m_currentRegexName.size() >= 8) {
        QString regexName = m_currentRegexName.toLower();
        // 提取正则表达式名称，忽略编号和S
        int underscoreIndex = regexName.indexOf('_');
        if (underscoreIndex != -1) {
            QString baseName = regexName.mid(underscoreIndex + 1);
            // 移除编号部分（从数字开始）
            int digitIndex = 0;
            while (digitIndex < baseName.size() && !baseName[digitIndex].isDigit()) {
                digitIndex++;
            }
            baseName = baseName.left(digitIndex);
            // 检查是否为keyword
            isKeywordRegex = (baseName == "keyword");
        }
    }
    
    // 处理正则表达式引用
    QMap<QString, QString> charToRefMap;
    QMap<QString, QList<QString>> refToCharsMap;
    QStringList refList;
    
    if (isKeywordRegex) {
        // 直接使用原始转移字符，不合并
        refList = QList<QString>(transitions.begin(), transitions.end());
        std::sort(refList.begin(), refList.end());
    } else {
        // 处理正则表达式引用
        charToRefMap = processRegexReferences(m_currentRegexItems);
        
        // 合并相同引用名称的转移
        refToCharsMap = mergeTransitionsByRef(transitions, charToRefMap);
        
        // 收集所有引用名称
        QSet<QString> referenceNames;
        QString regexText = ui->textEditRegex->toPlainText();
        QStringList lines = regexText.split("\n");
        for (const QString &line : lines) {
            QString trimmedLine = line.trimmed();
            // 移除可能存在的回车符
            trimmedLine = trimmedLine.replace("\r", "");
            if (trimmedLine.isEmpty() || trimmedLine.startsWith("//")) {
                continue;
            }
            int equalsPos = trimmedLine.indexOf('=');
            if (equalsPos == -1) {
                continue;
            }
            QString name = trimmedLine.left(equalsPos).trimmed();
            if (!name.startsWith('_')) {
                referenceNames.insert(name);
            }
        }
        
        // 将引用名称转换为列表并排序，保留引用名称、"#"和没有映射到引用名称的字符
        for (const QString &key : refToCharsMap.keys()) {
            if (key == "#" || referenceNames.contains(key) || (refToCharsMap[key].size() == 1 && refToCharsMap[key].first() == key)) {
                refList << key;
            }
        }
        std::sort(refList.begin(), refList.end());
    }
    
    // 设置表头
    QStringList headers;
    headers << "状态标记" << "状态编号";
    for (const QString &ref : refList) {
        headers << ref;
    }
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    
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
        for (const QString &ref : std::as_const(refList)) {
            QList<QString> chars;
            if (isKeywordRegex) {
                // 直接使用当前ref作为字符
                chars << ref;
            } else {
                // 获取引用名称对应的字符列表
                chars = refToCharsMap[ref];
            }
            
            QSet<int> uniqueTargetStates;
            
            // 收集所有匹配字符的转移
            for (const QString &charStr : chars) {
                for (const NFATransition &t : nfa.transitions) {
                    if (t.fromState == state && t.input == charStr) {
                        uniqueTargetStates.insert(t.toState);
                    }
                }
            }
            
            // 将唯一的目标状态转换为字符串
            QString targetStates;
            for (int ts : uniqueTargetStates) {
                if (!targetStates.isEmpty()) {
                    targetStates += ",";
                }
                targetStates += QString::number(ts);
            }
            
            rowData << targetStates;
        }
        
        addTableRow(table, rowData);
    }
    
    // 调整列宽
    table->resizeColumnsToContents();
}

void Task1Window::displayDFA(const DFA &dfa, QTableWidget *table)
{
    clearTable(table);
    
    // 收集所有转移名称（输入字符）
    QSet<QString> transitions;
    for (const DFATransition &transition : dfa.transitions) {
        transitions.insert(transition.input);
    }
    
    // 检查当前处理的正则表达式是否为keyword（不区分大小写）
    bool isKeywordRegex = false;
    if (!m_isTotalView && m_currentRegexName.size() >= 8) {
        QString regexName = m_currentRegexName.toLower();
        // 提取正则表达式名称，忽略编号和S
        int underscoreIndex = regexName.indexOf('_');
        if (underscoreIndex != -1) {
            QString baseName = regexName.mid(underscoreIndex + 1);
            // 移除编号部分（从数字开始）
            int digitIndex = 0;
            while (digitIndex < baseName.size() && !baseName[digitIndex].isDigit()) {
                digitIndex++;
            }
            baseName = baseName.left(digitIndex);
            // 检查是否为keyword
            isKeywordRegex = (baseName == "keyword");
        }
    }
    
    // 处理正则表达式引用
    QMap<QString, QString> charToRefMap;
    QMap<QString, QList<QString>> refToCharsMap;
    QStringList refList;
    
    if (isKeywordRegex) {
        // 直接使用原始转移字符，不合并
        refList = QList<QString>(transitions.begin(), transitions.end());
        std::sort(refList.begin(), refList.end());
    } else {
        // 处理正则表达式引用
        charToRefMap = processRegexReferences(m_currentRegexItems);
        
        // 合并相同引用名称的转移
        refToCharsMap = mergeTransitionsByRef(transitions, charToRefMap);
        
        // 收集所有引用名称
        QSet<QString> referenceNames;
        QString regexText = ui->textEditRegex->toPlainText();
        QStringList lines = regexText.split("\n");
        for (const QString &line : lines) {
            QString trimmedLine = line.trimmed();
            // 移除可能存在的回车符
            trimmedLine = trimmedLine.replace("\r", "");
            if (trimmedLine.isEmpty() || trimmedLine.startsWith("//")) {
                continue;
            }
            int equalsPos = trimmedLine.indexOf('=');
            if (equalsPos == -1) {
                continue;
            }
            QString name = trimmedLine.left(equalsPos).trimmed();
            if (!name.startsWith('_')) {
                referenceNames.insert(name);
            }
        }
        
        // 将引用名称转换为列表并排序，保留引用名称、"#"和没有映射到引用名称的字符
        for (const QString &key : refToCharsMap.keys()) {
            if (key == "#" || referenceNames.contains(key) || (refToCharsMap[key].size() == 1 && refToCharsMap[key].first() == key)) {
                refList << key;
            }
        }
        std::sort(refList.begin(), refList.end());
    }
    
    // 设置表头
    QStringList headers;
    headers << "状态标记" << "状态编号";
    for (const QString &ref : refList) {
        headers << ref;
    }
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    
    // 创建DFA状态到NFA状态集合的反向映射
    QMap<DFAState, QList<NFAState>> dfaToNfaStatesMap;
    for (auto it = dfa.stateMap.constBegin(); it != dfa.stateMap.constEnd(); ++it) {
        const QList<NFAState> &nfaStates = it.key();
        DFAState dfaState = it.value();
        dfaToNfaStatesMap[dfaState] = nfaStates;
    }
    
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
        for (const QString &ref : refList) {
            QList<QString> chars;
            if (isKeywordRegex) {
                // 直接使用当前ref作为字符
                chars << ref;
            } else {
                // 获取引用名称对应的字符列表
                chars = refToCharsMap[ref];
            }
            
            QString targetState = "";
            
            // 查找匹配字符的转移，DFA每个状态和转移只有一个目标状态
            for (const QString &charStr : chars) {
                for (const DFATransition &t : dfa.transitions) {
                    if (t.fromState == state && t.input == charStr) {
                        targetState = QString::number(t.toState);
                        break;
                    }
                }
                if (!targetState.isEmpty()) {
                    break;
                }
            }
            
            rowData << targetState;
        }
        
        addTableRow(table, rowData);
    }
    
    // 调整列宽
    table->resizeColumnsToContents();
}

void Task1Window::displayMinimizedDFA(const DFA &dfa, QTableWidget *table)
{
    clearTable(table);
    
    // 收集所有转移名称（输入字符）
    QSet<QString> transitions;
    for (const DFATransition &transition : dfa.transitions) {
        transitions.insert(transition.input);
    }
    
    // 检查当前处理的正则表达式是否为keyword（不区分大小写）
    bool isKeywordRegex = false;
    if (!m_isTotalView && m_currentRegexName.size() >= 8) {
        QString regexName = m_currentRegexName.toLower();
        // 提取正则表达式名称，忽略编号和S
        int underscoreIndex = regexName.indexOf('_');
        if (underscoreIndex != -1) {
            QString baseName = regexName.mid(underscoreIndex + 1);
            // 移除编号部分（从数字开始）
            int digitIndex = 0;
            while (digitIndex < baseName.size() && !baseName[digitIndex].isDigit()) {
                digitIndex++;
            }
            baseName = baseName.left(digitIndex);
            // 检查是否为keyword
            isKeywordRegex = (baseName == "keyword");
        }
    }
    
    // 处理正则表达式引用
    QMap<QString, QString> charToRefMap;
    QMap<QString, QList<QString>> refToCharsMap;
    QStringList refList;
    
    if (isKeywordRegex) {
        // 直接使用原始转移字符，不合并
        refList = QList<QString>(transitions.begin(), transitions.end());
        std::sort(refList.begin(), refList.end());
    } else {
        // 处理正则表达式引用
        charToRefMap = processRegexReferences(m_currentRegexItems);
        
        // 合并相同引用名称的转移
        refToCharsMap = mergeTransitionsByRef(transitions, charToRefMap);
        
        // 收集所有引用名称
        QSet<QString> referenceNames;
        QString regexText = ui->textEditRegex->toPlainText();
        QStringList lines = regexText.split("\n");
        for (const QString &line : lines) {
            QString trimmedLine = line.trimmed();
            // 移除可能存在的回车符
            trimmedLine = trimmedLine.replace("\r", "");
            if (trimmedLine.isEmpty() || trimmedLine.startsWith("//")) {
                continue;
            }
            int equalsPos = trimmedLine.indexOf('=');
            if (equalsPos == -1) {
                continue;
            }
            QString name = trimmedLine.left(equalsPos).trimmed();
            if (!name.startsWith('_')) {
                referenceNames.insert(name);
            }
        }
        
        // 将引用名称转换为列表并排序，保留引用名称、"#"和没有映射到引用名称的字符
        for (const QString &key : refToCharsMap.keys()) {
            if (key == "#" || referenceNames.contains(key) || (refToCharsMap[key].size() == 1 && refToCharsMap[key].first() == key)) {
                refList << key;
            }
        }
        std::sort(refList.begin(), refList.end());
    }
    
    // 设置表头
    QStringList headers;
    headers << "状态标记" << "状态编号";
    for (const QString &ref : refList) {
        headers << ref;
    }
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    
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
        for (const QString &ref : refList) {
            QList<QString> chars;
            if (isKeywordRegex) {
                // 直接使用当前ref作为字符
                chars << ref;
            } else {
                // 获取引用名称对应的字符列表
                chars = refToCharsMap[ref];
            }
            
            QString targetState = "";
            
            // 查找匹配字符的转移
            for (const QString &charStr : chars) {
                for (const DFATransition &t : dfa.transitions) {
                    if (t.fromState == state && t.input == charStr) {
                        targetState = QString::number(t.toState);
                        break;
                    }
                }
                if (!targetState.isEmpty()) {
                    break;
                }
            }
            
            rowData << targetState;
        }
        
        addTableRow(table, rowData);
    }
    
    // 调整列宽
    table->resizeColumnsToContents();
}

void Task1Window::displayLexicalResults(const QList<LexicalResult> &results)
{
    // 清空文本框
    m_textEditTestOutput->clear();
    
    // 构建结果字符串
    QString resultText;
    
    // 遍历所有结果
    for (const auto &result : results) {
        // 添加单词编码
        resultText += QString::number(result.tokenCode) + " ";
        
        // 检查是否为标识符或数字：根据tokenCode判断
        // 标识符（tokenCode 100）和数字（tokenCode 101）：显示编码 单词
        // 其他符号：只显示编码
        bool isIdentifierOrNumber = (result.tokenCode == 100 || result.tokenCode == 101);
        
        // 对于标识符或数字，在编码后接着输出单词，中间使用空格分隔
        if (isIdentifierOrNumber) {
            resultText += result.lexeme + " ";
        }
    }
    
    // 设置到文本框
    m_textEditTestOutput->setPlainText(resultText);
}

// 更新正则表达式下拉列表
void Task1Window::updateRegexComboBox()
{
    QStringList allItems;
    allItems << "总表";
    
    // 添加所有以下划线开头的正则表达式名称
    for (const RegexItem &item : m_currentRegexItems) {
        if (item.name.startsWith('_')) {
            allItems << item.name;
        }
    }
    
    // 更新所有下拉列表
    ui->comboBoxNFA->clear();
    ui->comboBoxNFA->addItems(allItems);
    
    ui->comboBoxDFA->clear();
    ui->comboBoxDFA->addItems(allItems);
    
    ui->comboBoxMinDFA->clear();
    ui->comboBoxMinDFA->addItems(allItems);
    
    // 设置当前选中项
    if (m_isTotalView) {
        ui->comboBoxNFA->setCurrentText("总表");
        ui->comboBoxDFA->setCurrentText("总表");
        ui->comboBoxMinDFA->setCurrentText("总表");
    } else if (!m_currentRegexName.isEmpty() && ui->comboBoxNFA->findText(m_currentRegexName) != -1) {
        ui->comboBoxNFA->setCurrentText(m_currentRegexName);
        ui->comboBoxDFA->setCurrentText(m_currentRegexName);
        ui->comboBoxMinDFA->setCurrentText(m_currentRegexName);
    }
}

void Task1Window::updateRegexComboBoxWithoutChangingSelection(const QString &currentRegexName, bool isTotalView)
{
    QStringList allItems;
    allItems << "总表";
    
    // 添加所有以下划线开头的正则表达式名称
    for (const RegexItem &item : m_currentRegexItems) {
        if (item.name.startsWith('_')) {
            allItems << item.name;
        }
    }
    
    // 更新所有下拉列表
    ui->comboBoxNFA->clear();
    ui->comboBoxNFA->addItems(allItems);
    
    ui->comboBoxDFA->clear();
    ui->comboBoxDFA->addItems(allItems);
    
    ui->comboBoxMinDFA->clear();
    ui->comboBoxMinDFA->addItems(allItems);
    
    // 尝试恢复之前的选中项
    if (isTotalView && ui->comboBoxNFA->count() > 0) {
        ui->comboBoxNFA->setCurrentText("总表");
        ui->comboBoxDFA->setCurrentText("总表");
        ui->comboBoxMinDFA->setCurrentText("总表");
    } else if (!currentRegexName.isEmpty() && ui->comboBoxNFA->findText(currentRegexName) != -1) {
        ui->comboBoxNFA->setCurrentText(currentRegexName);
        ui->comboBoxDFA->setCurrentText(currentRegexName);
        ui->comboBoxMinDFA->setCurrentText(currentRegexName);
    } else if (ui->comboBoxNFA->count() > 0) {
        // 如果之前的选项不存在，选择第一个有效选项
        ui->comboBoxNFA->setCurrentIndex(0);
        ui->comboBoxDFA->setCurrentIndex(0);
        ui->comboBoxMinDFA->setCurrentIndex(0);
    }
}

// 重置图表状态到初始加载状态
void Task1Window::resetChartDisplayState()
{
    // 重置动态表格
    if (m_dynamicTableNFA) {
        m_dynamicTableNFA->clear();
        m_dynamicTableNFA->setRowCount(0);
        m_dynamicTableNFA->setColumnCount(0);
    }
    
    if (m_dynamicTableDFA) {
        m_dynamicTableDFA->clear();
        m_dynamicTableDFA->setRowCount(0);
        m_dynamicTableDFA->setColumnCount(0);
    }
    
    if (m_dynamicTableMinDFA) {
        m_dynamicTableMinDFA->clear();
        m_dynamicTableMinDFA->setRowCount(0);
        m_dynamicTableMinDFA->setColumnCount(0);
    }
    
    // 重置分组框标题
    ui->groupBoxNFA->setTitle("NFA图表");
    ui->groupBoxDFA->setTitle("DFA图表");
    ui->groupBoxMinDFA->setTitle("最小化DFA图表");
    
    // 强制UI更新
    ui->groupBoxNFA->update();
    ui->groupBoxDFA->update();
    ui->groupBoxMinDFA->update();
}

// 更新NFA表格显示
void Task1Window::updateNFADisplay()
{
    if (!m_dynamicTableNFA) {
        return;
    }
    
    // 清空表格
    m_dynamicTableNFA->setRowCount(0);
    m_dynamicTableNFA->setColumnCount(0);
    
    if (m_isTotalView) {
        // 显示总NFA
        if (m_totalNFA.states.size() > 0) {
            try {
                displayNFA(m_totalNFA, m_dynamicTableNFA);
                ui->groupBoxNFA->setTitle("总NFA图表");
            } catch (const std::exception &e) {
                ui->groupBoxNFA->setTitle("NFA图表（生成失败）");
                ui->statusbar->showMessage(tr("NFA图表生成失败: %1").arg(e.what()), 3000);
            }
        } else {
            ui->groupBoxNFA->setTitle("NFA图表（未生成）");
        }
    } else {
        // 显示单个NFA
        if (!m_currentRegexName.isEmpty() && m_nfaMap.contains(m_currentRegexName)) {
            const NFA &nfa = m_nfaMap[m_currentRegexName];
            try {
                displayNFA(nfa, m_dynamicTableNFA);
                ui->groupBoxNFA->setTitle(QString("单个正则表达式NFA图表 - %1").arg(m_currentRegexName));
            } catch (const std::exception &e) {
                ui->groupBoxNFA->setTitle(QString("NFA图表（生成失败 - %1）").arg(m_currentRegexName));
                ui->statusbar->showMessage(tr("NFA图表生成失败: %1").arg(e.what()), 3000);
            }
        } else {
            ui->groupBoxNFA->setTitle("NFA图表（未选择）");
        }
    }
    
    // 调整表格大小以适应内容
    m_dynamicTableNFA->resizeColumnsToContents();
    m_dynamicTableNFA->resizeRowsToContents();
}

// 更新DFA表格显示
void Task1Window::updateDFADisplay()
{
    if (!m_dynamicTableDFA) {
        return;
    }
    
    // 清空表格
    m_dynamicTableDFA->setRowCount(0);
    m_dynamicTableDFA->setColumnCount(0);
    
    if (m_isTotalView) {
        // 显示总DFA
        if (m_totalDFA.states.size() > 0) {
            try {
                displayDFA(m_totalDFA, m_dynamicTableDFA);
                ui->groupBoxDFA->setTitle("总DFA图表");
            } catch (const std::exception &e) {
                ui->groupBoxDFA->setTitle("DFA图表（生成失败）");
                ui->statusbar->showMessage(tr("DFA图表生成失败: %1").arg(e.what()), 3000);
            }
        } else {
            ui->groupBoxDFA->setTitle("DFA图表（未生成）");
        }
    } else {
        // 显示单个DFA
        if (!m_currentRegexName.isEmpty() && m_dfaMap.contains(m_currentRegexName)) {
            const DFA &dfa = m_dfaMap[m_currentRegexName];
            try {
                displayDFA(dfa, m_dynamicTableDFA);
                ui->groupBoxDFA->setTitle(QString("单个正则表达式DFA图表 - %1").arg(m_currentRegexName));
            } catch (const std::exception &e) {
                ui->groupBoxDFA->setTitle(QString("DFA图表（生成失败 - %1）").arg(m_currentRegexName));
                ui->statusbar->showMessage(tr("DFA图表生成失败: %1").arg(e.what()), 3000);
            }
        } else {
            ui->groupBoxDFA->setTitle("DFA图表（未选择）");
        }
    }
    
    // 调整表格大小以适应内容
    m_dynamicTableDFA->resizeColumnsToContents();
    m_dynamicTableDFA->resizeRowsToContents();
}

// 更新最小化DFA表格显示
void Task1Window::updateMinimizedDFADisplay()
{
    if (!m_dynamicTableMinDFA) {
        return;
    }
    
    // 清空表格
    m_dynamicTableMinDFA->setRowCount(0);
    m_dynamicTableMinDFA->setColumnCount(0);
    
    if (m_isTotalView) {
        // 显示总最小化DFA
        if (m_totalMinimizedDFA.states.size() > 0) {
            try {
                displayMinimizedDFA(m_totalMinimizedDFA, m_dynamicTableMinDFA);
                ui->groupBoxMinDFA->setTitle("总最小化DFA图表");
            } catch (const std::exception &e) {
                ui->groupBoxMinDFA->setTitle("最小化DFA图表（生成失败）");
                ui->statusbar->showMessage(tr("最小化DFA图表生成失败: %1").arg(e.what()), 3000);
            }
        } else {
            ui->groupBoxMinDFA->setTitle("最小化DFA图表（未生成）");
        }
    } else {
        // 显示单个最小化DFA
        if (!m_currentRegexName.isEmpty() && m_minimizedDfaMap.contains(m_currentRegexName)) {
            const DFA &minimizedDFA = m_minimizedDfaMap[m_currentRegexName];
            try {
                displayMinimizedDFA(minimizedDFA, m_dynamicTableMinDFA);
                ui->groupBoxMinDFA->setTitle(QString("单个正则表达式最小化DFA图表 - %1").arg(m_currentRegexName));
            } catch (const std::exception &e) {
                ui->groupBoxMinDFA->setTitle(QString("最小化DFA图表（生成失败 - %1）").arg(m_currentRegexName));
                ui->statusbar->showMessage(tr("最小化DFA图表生成失败: %1").arg(e.what()), 3000);
            }
        } else {
            ui->groupBoxMinDFA->setTitle("最小化DFA图表（未选择）");
        }
    }
    
    // 调整表格大小以适应内容
    m_dynamicTableMinDFA->resizeColumnsToContents();
    m_dynamicTableMinDFA->resizeRowsToContents();
}

// 正则表达式选择切换
void Task1Window::on_comboBoxNFA_currentIndexChanged(const QString &arg1)
{
    // 显示加载状态
    ui->statusbar->showMessage(tr("正在加载图表..."), 0);
    
    m_currentRegexName = arg1;
    m_isTotalView = (arg1 == "总表");
    
    // 同步其他下拉列表，避免信号循环
    ui->comboBoxDFA->blockSignals(true);
    ui->comboBoxMinDFA->blockSignals(true);
    ui->comboBoxDFA->setCurrentText(arg1);
    ui->comboBoxMinDFA->setCurrentText(arg1);
    ui->comboBoxDFA->blockSignals(false);
    ui->comboBoxMinDFA->blockSignals(false);
    
    // 先清空所有表格内容，准备更新
    if (m_dynamicTableNFA) {
        m_dynamicTableNFA->clearContents();
        m_dynamicTableNFA->setRowCount(0);
    }
    if (m_dynamicTableDFA) {
        m_dynamicTableDFA->clearContents();
        m_dynamicTableDFA->setRowCount(0);
    }
    if (m_dynamicTableMinDFA) {
        m_dynamicTableMinDFA->clearContents();
        m_dynamicTableMinDFA->setRowCount(0);
    }
    
    // 处理UI更新，确保清空操作立即生效
    QApplication::processEvents();
    
    // 更新图表显示
    updateNFADisplay();
    updateDFADisplay();
    updateMinimizedDFADisplay();
    
    // 显示加载完成状态
    ui->statusbar->showMessage(tr("图表加载完成"), 3000);
}

// DFA标签页的正则表达式选择切换
void Task1Window::on_comboBoxDFA_currentIndexChanged(const QString &arg1)
{
    // 显示加载状态
    ui->statusbar->showMessage(tr("正在加载图表..."), 0);
    
    m_currentRegexName = arg1;
    m_isTotalView = (arg1 == "总表");

    // 同步其他下拉列表，避免信号循环
    ui->comboBoxNFA->blockSignals(true);
    ui->comboBoxMinDFA->blockSignals(true);
    ui->comboBoxNFA->setCurrentText(arg1);
    ui->comboBoxMinDFA->setCurrentText(arg1);
    ui->comboBoxNFA->blockSignals(false);
    ui->comboBoxMinDFA->blockSignals(false);
    
    // 先清空所有表格内容，准备更新
    if (m_dynamicTableNFA) {
        m_dynamicTableNFA->clearContents();
        m_dynamicTableNFA->setRowCount(0);
    }
    if (m_dynamicTableDFA) {
        m_dynamicTableDFA->clearContents();
        m_dynamicTableDFA->setRowCount(0);
    }
    if (m_dynamicTableMinDFA) {
        m_dynamicTableMinDFA->clearContents();
        m_dynamicTableMinDFA->setRowCount(0);
    }
    
    // 处理UI更新，确保清空操作立即生效
    QApplication::processEvents();
    
    // 更新图表显示
    updateNFADisplay();
    updateDFADisplay();
    updateMinimizedDFADisplay();
    
    // 显示加载完成状态
    ui->statusbar->showMessage(tr("图表加载完成"), 3000);
}

// 最小化DFA标签页的正则表达式选择切换
void Task1Window::on_comboBoxMinDFA_currentIndexChanged(const QString &arg1)
{
    // 显示加载状态
    ui->statusbar->showMessage(tr("正在加载图表..."), 0);
    
    m_currentRegexName = arg1;
    m_isTotalView = (arg1 == "总表");

    // 同步其他下拉列表，避免信号循环
    ui->comboBoxNFA->blockSignals(true);
    ui->comboBoxDFA->blockSignals(true);
    ui->comboBoxNFA->setCurrentText(arg1);
    ui->comboBoxDFA->setCurrentText(arg1);
    ui->comboBoxNFA->blockSignals(false);
    ui->comboBoxDFA->blockSignals(false);
    
    // 先清空所有表格内容，准备更新
    if (m_dynamicTableNFA) {
        m_dynamicTableNFA->clearContents();
        m_dynamicTableNFA->setRowCount(0);
    }
    if (m_dynamicTableDFA) {
        m_dynamicTableDFA->clearContents();
        m_dynamicTableDFA->setRowCount(0);
    }
    if (m_dynamicTableMinDFA) {
        m_dynamicTableMinDFA->clearContents();
        m_dynamicTableMinDFA->setRowCount(0);
    }
    
    // 处理UI更新，确保清空操作立即生效
    QApplication::processEvents();
    
    // 更新图表显示
    updateNFADisplay();
    updateDFADisplay();
    updateMinimizedDFADisplay();
    
    // 显示加载完成状态
    ui->statusbar->showMessage(tr("图表加载完成"), 3000);
}



// 手动刷新NFA图表
void Task1Window::on_btnRefreshNFA_clicked()
{
    // 显示加载状态
    ui->statusbar->showMessage(tr("正在刷新NFA图表..."), 0);
    
    // 保存当前选中的正则表达式名称和视图状态
    QString currentRegexName = m_currentRegexName;
    bool currentIsTotalView = m_isTotalView;
    
    // 重置图表状态到初始加载状态
    ui->statusbar->showMessage(tr("正在重置图表状态..."), 0);
    resetChartDisplayState();
    
    // 首先解析正则表达式
    ui->statusbar->showMessage(tr("正在解析正则表达式..."), 0);
    QString regexText = ui->textEditRegex->toPlainText();
    if (regexText.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请输入正则表达式"));
        ui->statusbar->showMessage(tr("刷新失败：正则表达式为空"), 3000);
        return;
    }
    
    if (!m_regexProcessor.parse(regexText)) {
        QMessageBox::warning(this, tr("错误"), tr("正则表达式解析失败: %1").arg(m_regexProcessor.getErrorMessage()));
        ui->statusbar->showMessage(tr("刷新失败：正则表达式解析错误"), 3000);
        return;
    }
    
    QList<RegexItem> regexItems = m_regexProcessor.getRegexItems();
    m_currentRegexItems = regexItems;
    if (regexItems.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("没有找到有效的正则表达式"));
        ui->statusbar->showMessage(tr("刷新失败：未找到有效正则表达式"), 3000);
        return;
    }
    
    // 清空之前的NFA映射
    ui->statusbar->showMessage(tr("正在清空NFA数据..."), 0);
    m_nfaMap.clear();
    m_totalNFA = NFA();
    
    // 为每个以下划线开头的正则表达式构建NFA
    ui->statusbar->showMessage(tr("正在构建NFA..."), 0);
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
        ui->statusbar->showMessage(tr("刷新失败：NFA构建错误"), 3000);
        return;
    }
    
    // 生成总NFA
    ui->statusbar->showMessage(tr("正在生成总NFA..."), 0);
    if (generatedCount > 0) {
        m_totalNFA = mergeNFAs(m_nfaMap.values());
    }
    
    // 更新正则表达式下拉列表（不改变当前选择）
    ui->statusbar->showMessage(tr("正在更新下拉列表..."), 0);
    updateRegexComboBoxWithoutChangingSelection(currentRegexName, currentIsTotalView);
    
    // 恢复当前选择状态
    m_currentRegexName = currentRegexName;
    m_isTotalView = currentIsTotalView;
    
    // 更新NFA图表显示
    ui->statusbar->showMessage(tr("正在更新图表显示..."), 0);
    updateNFADisplay();
    
    // 强制更新UI
    ui->groupBoxNFA->update();
    
    // 显示加载完成状态
    ui->statusbar->showMessage(tr("NFA图表刷新完成，共生成 %1 个NFA").arg(generatedCount), 5000);
}

// 手动刷新DFA图表
void Task1Window::on_btnRefreshDFA_clicked()
{
    // 显示加载状态
    ui->statusbar->showMessage(tr("正在刷新DFA图表..."), 0);
    
    // 保存当前选中的正则表达式名称和视图状态
    QString currentRegexName = m_currentRegexName;
    bool currentIsTotalView = m_isTotalView;
    
    // 重置图表状态到初始加载状态
    ui->statusbar->showMessage(tr("正在重置图表状态..."), 0);
    resetChartDisplayState();
    
    // 检查是否已生成NFA
    if (m_nfaMap.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先生成NFA"));
        ui->statusbar->showMessage(tr("刷新失败：请先生成NFA"), 3000);
        return;
    }
    
    // 清空之前的DFA映射
    ui->statusbar->showMessage(tr("正在清空DFA数据..."), 0);
    m_dfaMap.clear();
    m_totalDFA = DFA();
    
    // 为每个NFA生成DFA
    ui->statusbar->showMessage(tr("正在转换NFA到DFA..."), 0);
    int generatedCount = 0;
    for (auto it = m_nfaMap.constBegin(); it != m_nfaMap.constEnd(); ++it) {
        const QString &regexName = it.key();
        const NFA &nfa = it.value();
        
        DFA dfa = m_dfaBuilder.convertNFAToDFA(nfa);
        if (!dfa.states.isEmpty()) {
            m_dfaMap[regexName] = dfa;
            generatedCount++;
        }
    }
    
    // 生成总DFA
    ui->statusbar->showMessage(tr("正在生成总DFA..."), 0);
    if (!m_totalNFA.states.isEmpty()) {
        m_totalDFA = m_dfaBuilder.convertNFAToDFA(m_totalNFA);
    }
    
    // 更新正则表达式下拉列表（不改变当前选择）
    ui->statusbar->showMessage(tr("正在更新下拉列表..."), 0);
    updateRegexComboBoxWithoutChangingSelection(currentRegexName, currentIsTotalView);
    
    // 恢复当前选择状态
    m_currentRegexName = currentRegexName;
    m_isTotalView = currentIsTotalView;
    
    // 更新DFA图表显示
    ui->statusbar->showMessage(tr("正在更新图表显示..."), 0);
    updateDFADisplay();
    
    // 强制更新UI
    ui->groupBoxDFA->update();
    
    // 显示加载完成状态
    ui->statusbar->showMessage(tr("DFA图表刷新完成，共生成 %1 个DFA").arg(generatedCount), 5000);
}

// 手动刷新最小化DFA图表
void Task1Window::on_btnRefreshMinDFA_clicked()
{
    // 显示加载状态
    ui->statusbar->showMessage(tr("正在刷新最小化DFA图表..."), 0);
    
    // 保存当前选中的正则表达式名称和视图状态
    QString currentRegexName = m_currentRegexName;
    bool currentIsTotalView = m_isTotalView;
    
    // 重置图表状态到初始加载状态
    ui->statusbar->showMessage(tr("正在重置图表状态..."), 0);
    resetChartDisplayState();
    
    // 检查是否已生成DFA
    if (m_dfaMap.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先生成DFA"));
        ui->statusbar->showMessage(tr("刷新失败：请先生成DFA"), 3000);
        return;
    }
    
    // 清空之前的最小化DFA映射
    ui->statusbar->showMessage(tr("正在清空最小化DFA数据..."), 0);
    m_minimizedDfaMap.clear();
    m_totalMinimizedDFA = DFA();
    
    // 为每个DFA生成最小化DFA
    ui->statusbar->showMessage(tr("正在最小化DFA..."), 0);
    int generatedCount = 0;
    for (auto it = m_dfaMap.constBegin(); it != m_dfaMap.constEnd(); ++it) {
        const QString &regexName = it.key();
        const DFA &dfa = it.value();
        
        DFA minimizedDFA = m_dfaMinimizer.minimizeDFA(dfa);
        if (!minimizedDFA.states.isEmpty()) {
            m_minimizedDfaMap[regexName] = minimizedDFA;
            generatedCount++;
        }
    }
    
    // 生成总最小化DFA
    ui->statusbar->showMessage(tr("正在生成总最小化DFA..."), 0);
    if (!m_totalDFA.states.isEmpty()) {
        m_totalMinimizedDFA = m_dfaMinimizer.minimizeDFA(m_totalDFA);
    }
    
    // 更新正则表达式下拉列表（不改变当前选择）
    ui->statusbar->showMessage(tr("正在更新下拉列表..."), 0);
    updateRegexComboBoxWithoutChangingSelection(currentRegexName, currentIsTotalView);
    
    // 恢复当前选择状态
    m_currentRegexName = currentRegexName;
    m_isTotalView = currentIsTotalView;
    
    // 更新最小化DFA图表显示
    ui->statusbar->showMessage(tr("正在更新图表显示..."), 0);
    updateMinimizedDFADisplay();
    
    // 强制更新UI
    ui->groupBoxMinDFA->update();
    
    // 显示加载完成状态
    ui->statusbar->showMessage(tr("最小化DFA图表刷新完成，共生成 %1 个最小化DFA").arg(generatedCount), 5000);
}

// 创建动态表格
void Task1Window::createDynamicTables()
{
    // 清理之前的动态表格（如果存在）
    cleanupDynamicTables();
    
    // 创建NFA表格
    m_dynamicTableNFA = new QTableWidget(this);
    m_dynamicTableNFA->setAlternatingRowColors(true);
    
    // 创建DFA表格
    m_dynamicTableDFA = new QTableWidget(this);
    m_dynamicTableDFA->setAlternatingRowColors(true);
    
    // 创建最小化DFA表格
    m_dynamicTableMinDFA = new QTableWidget(this);
    m_dynamicTableMinDFA->setAlternatingRowColors(true);
    
    // 将表格添加到对应的GroupBox布局中
    QVBoxLayout *nfaLayout = qobject_cast<QVBoxLayout*>(ui->groupBoxNFA->layout());
    if (nfaLayout) {
        nfaLayout->addWidget(m_dynamicTableNFA);
    }
    
    QVBoxLayout *dfaLayout = qobject_cast<QVBoxLayout*>(ui->groupBoxDFA->layout());
    if (dfaLayout) {
        dfaLayout->addWidget(m_dynamicTableDFA);
    }
    
    QVBoxLayout *minDFALayout = qobject_cast<QVBoxLayout*>(ui->groupBoxMinDFA->layout());
    if (minDFALayout) {
        minDFALayout->addWidget(m_dynamicTableMinDFA);
    }
}

// 清理动态表格
void Task1Window::cleanupDynamicTables()
{
    // 删除所有动态表格对象
    if (m_dynamicTableNFA) {
        delete m_dynamicTableNFA;
        m_dynamicTableNFA = nullptr;
    }
    if (m_dynamicTableDFA) {
        delete m_dynamicTableDFA;
        m_dynamicTableDFA = nullptr;
    }
    if (m_dynamicTableMinDFA) {
        delete m_dynamicTableMinDFA;
        m_dynamicTableMinDFA = nullptr;
    }
}


