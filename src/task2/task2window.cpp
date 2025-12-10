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
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

Task2Window::Task2Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Task2Window)
{
    ui->setupUi(this);
    setWindowTitle(tr("实验二 - 语法分析器生成器"));
    
    // 初始化UI组件
    initUI();
}

Task2Window::~Task2Window()
{
    delete ui;
}

void Task2Window::initUI()
{
    // 设置表格列宽
    ui->tableWidgetFirst->setColumnCount(2);
    ui->tableWidgetFirst->setHorizontalHeaderLabels(QStringList() << tr("非终结符") << tr("First集"));
    ui->tableWidgetFirst->horizontalHeader()->setStretchLastSection(true);
    
    ui->tableWidgetFollow->setColumnCount(2);
    ui->tableWidgetFollow->setHorizontalHeaderLabels(QStringList() << tr("非终结符") << tr("Follow集"));
    ui->tableWidgetFollow->horizontalHeader()->setStretchLastSection(true);
    
    ui->tableWidgetLR0->setColumnCount(3);
    ui->tableWidgetLR0->setHorizontalHeaderLabels(QStringList() << tr("状态") << tr("项目集") << tr("转换"));
    ui->tableWidgetLR0->horizontalHeader()->setStretchLastSection(true);
    
    // 移除不存在的tableWidgetConflicts组件初始化
    
    ui->tableWidgetLR1->setColumnCount(3);
    ui->tableWidgetLR1->setHorizontalHeaderLabels(QStringList() << tr("状态") << tr("项目集") << tr("转换"));
    ui->tableWidgetLR1->horizontalHeader()->setStretchLastSection(true);
    
    ui->tableWidgetLR1Table->setColumnCount(4);
    ui->tableWidgetLR1Table->setHorizontalHeaderLabels(QStringList() << tr("状态") << tr("Action") << tr("Goto") << tr("说明"));
    ui->tableWidgetLR1Table->horizontalHeader()->setStretchLastSection(true);
    
    ui->tableWidgetAnalysisSteps->setColumnCount(4);
    ui->tableWidgetAnalysisSteps->setHorizontalHeaderLabels(QStringList() << tr("步骤") << tr("栈") << tr("输入") << tr("动作"));
    ui->tableWidgetAnalysisSteps->horizontalHeader()->setStretchLastSection(true);
    
    // 清除硬编码的token映射，改为从文件加载
    m_tokenMap.clear();
    
    // 尝试从当前目录加载固定名称的token映射文件
    QString tokenMapFile = "sample.tokenmap";
    if (QFile::exists(tokenMapFile)) {
        if (loadTokenMap(tokenMapFile)) {
            qDebug() << "成功加载token映射文件：" << tokenMapFile;
        } else {
            qDebug() << "警告：无法加载token映射文件";
        }
    } else {
        qDebug() << "未找到token映射文件：" << tokenMapFile;
    }
}

void Task2Window::loadExampleGrammar()
{
    // 简单的示例BNF文法
    QString exampleGrammar = "S → aS | b\n";
    ui->textEditBNF->setPlainText(exampleGrammar);
    QMessageBox::information(this, tr("成功"), tr("示例文法加载成功"));
}

void Task2Window::on_actionExit_triggered()
{
    close();
}

void Task2Window::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开BNF文法文件"), ".", tr("文本文件 (*.txt);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            ui->textEditBNF->setPlainText(content);
            QMessageBox::information(this, tr("成功"), tr("BNF文法文件加载成功"));
            file.close();
        } else {
            QMessageBox::warning(this, tr("错误"), tr("BNF文法文件加载失败"));
        }
    }
}

void Task2Window::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存BNF文法文件"), ".", tr("文本文件 (*.txt);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->textEditBNF->toPlainText();
            QMessageBox::information(this, tr("成功"), tr("BNF文法文件保存成功"));
            file.close();
        } else {
            QMessageBox::warning(this, tr("错误"), tr("BNF文法文件保存失败"));
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
    
    // 解析BNF文法
    QString error;
    m_grammar = GrammarParser::parseString(bnfText, error);
    
    if (!error.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("BNF文法解析失败: %1").arg(error));
        return;
    }
    
    // 显示文法信息
    QString grammarInfo;
    grammarInfo += tr("开始符号: %1\n").arg(m_grammar.startSymbol);
    
    // 转换非终结符集合为列表并排序
    QList<QString> nonterminalsList = m_grammar.nonterminals.values();
    std::sort(nonterminalsList.begin(), nonterminalsList.end());
    grammarInfo += tr("非终结符: %1\n").arg(nonterminalsList.join(", "));
    
    // 转换终结符集合为列表并排序
    QList<QString> terminalsList = m_grammar.terminals.values();
    std::sort(terminalsList.begin(), terminalsList.end());
    grammarInfo += tr("终结符: %1\n").arg(terminalsList.join(", "));
    
    grammarInfo += tr("产生式:\n");
    for (auto it = m_grammar.productions.constBegin(); it != m_grammar.productions.constEnd(); ++it) {
        const QString& nonterminal = it.key();
        const auto& prods = it.value();
        for (const auto& prod : prods) {
            grammarInfo += nonterminal + " -> " + prod.right.join(" ") + "\n";
        }
    }
    ui->textBrowserGrammarInfo->setText(grammarInfo);
    
    QMessageBox::information(this, tr("成功"), tr("BNF文法解析成功"));
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
    
    // 清空功能已完成
}

void Task2Window::on_pushButtonLoadExample_clicked()
{
    loadExampleGrammar();
}

void Task2Window::on_pushButtonCalculateFirst_clicked()
{
    // 检查是否已经解析了文法
    if (m_grammar.productions.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    // 清空表格
    ui->tableWidgetFirst->setRowCount(0);
    
    // 计算First集
    m_ll1Info = LL1::compute(m_grammar);
    displayFirstSet();
    QMessageBox::information(this, tr("成功"), tr("First集计算成功"));
}

void Task2Window::on_pushButtonCalculateFollow_clicked()
{
    // 检查是否已经解析了文法
    if (m_grammar.productions.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    // 确保First集合已经计算
    if (m_ll1Info.first.isEmpty()) {
        m_ll1Info = LL1::compute(m_grammar);
    }
    
    // 清空表格
    ui->tableWidgetFollow->setRowCount(0);
    
    // 显示Follow集
    displayFollowSet();
    QMessageBox::information(this, tr("成功"), tr("Follow集计算成功"));
}

void Task2Window::on_pushButtonGenerateLR0DFA_clicked()
{
    // 检查是否已经解析了文法
    if (m_grammar.productions.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    // 清空表格
    ui->tableWidgetLR0->setRowCount(0);
    
    // 生成LR(0) DFA
    m_lr0Graph = LR0Builder::build(m_grammar);
    if (!m_lr0Graph.states.isEmpty()) {
        displayLR0DFA();
        QMessageBox::information(this, tr("成功"), tr("LR(0) DFA生成成功"));
    } else {
        QMessageBox::warning(this, tr("错误"), tr("LR(0) DFA生成失败"));
    }
}

void Task2Window::on_pushButtonCheckSLR1_clicked()
{
    // 检查是否已经解析了文法
    if (m_grammar.productions.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    // 确保First和Follow集合已经计算
    if (m_ll1Info.first.isEmpty()) {
        m_ll1Info = LL1::compute(m_grammar);
    }
    
    // 确保LR(0) DFA已经生成
    if (m_lr0Graph.states.isEmpty()) {
        m_lr0Graph = LR0Builder::build(m_grammar);
        if (m_lr0Graph.states.isEmpty()) {
            QMessageBox::warning(this, tr("错误"), tr("LR(0) DFA生成失败"));
            return;
        }
    }
    
    // 检查SLR(1)文法
    m_slrResult = SLR::check(m_grammar, m_ll1Info);
    displaySLR1Result();
    
    if (m_slrResult.isSLR1) {
        QMessageBox::information(this, tr("成功"), tr("该文法为SLR(1)文法"));
    } else {
        QMessageBox::warning(this, tr("警告"), tr("该文法不是SLR(1)文法，存在冲突"));
    }
}

void Task2Window::on_pushButtonGenerateLR1DFA_clicked()
{
    // 检查是否已经解析了文法
    if (m_grammar.productions.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    // 清空表格
    ui->tableWidgetLR1->setRowCount(0);
    
    // 生成LR(1) DFA
    m_lr1Graph = LR1Builder::build(m_grammar);
    if (!m_lr1Graph.states.isEmpty()) {
        displayLR1DFA();
        QMessageBox::information(this, tr("成功"), tr("LR(1) DFA生成成功"));
    } else {
        QMessageBox::warning(this, tr("错误"), tr("LR(1) DFA生成失败"));
    }
}

void Task2Window::on_pushButtonGenerateLR1Table_clicked()
{
    // 检查是否已经解析了文法
    if (m_grammar.productions.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    // 确保LR(1) DFA已经生成
    if (m_lr1Graph.states.isEmpty()) {
        m_lr1Graph = LR1Builder::build(m_grammar);
        if (m_lr1Graph.states.isEmpty()) {
            QMessageBox::warning(this, tr("错误"), tr("LR(1) DFA生成失败"));
            return;
        }
    }
    
    // 清空表格
    ui->tableWidgetLR1Table->setRowCount(0);
    
    // 生成LR(1)分析表
    m_lr1Table = LR1Builder::computeActionTable(m_grammar, m_lr1Graph);
    displayLR1Table();
    QMessageBox::information(this, tr("成功"), tr("LR(1)分析表生成成功"));
}

void Task2Window::on_pushButtonSaveLR1Table_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存LR(1)分析表"), ".", tr("CSV文件 (*.csv);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        // 保存LR(1)分析表到CSV文件
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            
            // 写入表头
            out << "状态,符号,动作/目标\n";
            
            // 写入Action表
            for (auto actionIt = m_lr1Table.action.constBegin(); actionIt != m_lr1Table.action.constEnd(); ++actionIt) {
                int state = actionIt.key();
                const auto& symbolActions = actionIt.value();
                for (auto symbolIt = symbolActions.constBegin(); symbolIt != symbolActions.constEnd(); ++symbolIt) {
                    const QString& symbol = symbolIt.key();
                    const QString& action = symbolIt.value();
                    out << state << "," << symbol << "," << action << "\n";
                }
            }
            
            // 写入Goto表
            for (auto gotoIt = m_lr1Table.gotoTable.constBegin(); gotoIt != m_lr1Table.gotoTable.constEnd(); ++gotoIt) {
                int state = gotoIt.key();
                const auto& symbolStates = gotoIt.value();
                for (auto symbolIt = symbolStates.constBegin(); symbolIt != symbolStates.constEnd(); ++symbolIt) {
                    const QString& symbol = symbolIt.key();
                    int target = symbolIt.value();
                    out << state << "," << symbol << "," << target << "\n";
                }
            }
            
            file.close();
            QMessageBox::information(this, tr("成功"), tr("LR(1)分析表保存成功"));
        } else {
            QMessageBox::warning(this, tr("错误"), tr("LR(1)分析表保存失败"));
        }
    }
}

void Task2Window::on_pushButtonOpenTokenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开词法单元文件"), ".", tr("文本文件 (*.txt);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            m_tokenFileContent = in.readAll();
            ui->textEditTokenFile->setPlainText(m_tokenFileContent);
            
            // 正确解析交替的token编码和词素格式
            QStringList tokensAndLexemes = m_tokenFileContent.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            m_tokens.clear();
            
            int i = 0;
            while (i < tokensAndLexemes.size()) {
                QString code = tokensAndLexemes[i];
                
                // 使用token映射转换编码
                QString tokenName;
                if (m_tokenMap.contains(code)) {
                    tokenName = m_tokenMap.value(code);
                } else {
                    // 无法识别的编码，保留原始值
                    tokenName = code;
                    i++;
                    m_tokens.append(tokenName);
                    continue;
                }
                
                // 使用m_singleCodeTokens动态判断是否为单编码token
                bool isSingleCodeToken = m_singleCodeTokens.contains(tokenName);
                
                if (isSingleCodeToken) {
                    // 单编码token，需要词素，往后再读一个token作为词素
                    if (i + 1 < tokensAndLexemes.size()) {
                        // 单编码token只需要token名称，词素已经包含在token中
                        // 跳过词素，因为语法分析只需要token类型
                        i += 2;
                    } else {
                        // 没有词素，只使用token名称
                        i++;
                    }
                    // 单编码token只需要token名称，词素已经包含在token中
                    m_tokens.append(tokenName);
                } else {
                    // 多编码token，不需要词素，只使用token名称
                    m_tokens.append(tokenName);
                    i++;
                }
            }
            
            QMessageBox::information(this, tr("成功"), tr("词法单元文件加载成功，共加载 %1 个token").arg(m_tokens.size()));
            file.close();
        } else {
            QMessageBox::warning(this, tr("错误"), tr("词法单元文件加载失败"));
        }
    }
}

void Task2Window::on_pushButtonAnalyzeSyntax_clicked()
{
    // 检查是否已经解析了文法
    if (m_grammar.productions.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先解析BNF文法"));
        return;
    }
    
    // 检查是否已经加载了词法单元
    if (m_tokens.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载词法单元文件"));
        return;
    }
    
    // 检查token映射是否为空，如果为空，尝试加载
    if (m_tokenMap.isEmpty()) {
        QString tokenMapFileName = "sample.tokenmap";
        if (QFile::exists(tokenMapFileName)) {
            if (loadTokenMap(tokenMapFileName)) {
                qDebug() << "成功加载token映射文件：" << tokenMapFileName;
            } else {
                QMessageBox::warning(this, tr("警告"), tr("无法加载token映射文件：") + tokenMapFileName);
                return;
            }
        } else {
            QMessageBox::warning(this, tr("警告"), tr("未找到token映射文件：") + tokenMapFileName);
            return;
        }
    }
    
    // 确保LR(1)分析表已经生成
    if (m_lr1Table.action.isEmpty()) {
        if (m_lr1Graph.states.isEmpty()) {
            m_lr1Graph = LR1Builder::build(m_grammar);
            if (m_lr1Graph.states.isEmpty()) {
                QMessageBox::warning(this, tr("错误"), tr("LR(1) DFA生成失败"));
                return;
            }
        }
        m_lr1Table = LR1Builder::computeActionTable(m_grammar, m_lr1Graph);
    }
    
    // 清空表格
    ui->tableWidgetAnalysisSteps->setRowCount(0);
    
    // 进行语法分析
    m_parseResult = LR1Parser::parse(m_tokens, m_grammar, m_lr1Table);
    
    if (m_parseResult.errorPos == -1) {
        // 显示分析结果
        displaySyntaxAnalysisResult();
        QMessageBox::information(this, tr("成功"), tr("语法分析成功"));
    } else {
        // 显示详细的错误信息
        QString errorMsg = tr("语法分析失败！");
        errorMsg += tr("\n错误位置: %1").arg(m_parseResult.errorPos);
        
        if (!m_parseResult.errorMsg.isEmpty()) {
            errorMsg += tr("\n错误详情: %1").arg(m_parseResult.errorMsg);
        }
        
        // 显示当前token和期望的token
        if (m_parseResult.errorPos > 0 && m_parseResult.errorPos <= m_tokens.size()) {
            int tokenIndex = m_parseResult.errorPos - 1;
            QString currentToken = m_tokens[tokenIndex];
            errorMsg += tr("\n当前token: %1").arg(currentToken);
        }
        
        // 显示分析步骤，帮助调试
        if (!m_parseResult.steps.isEmpty()) {
            const ParseStep& lastStep = m_parseResult.steps.last();
            errorMsg += tr("\n最后分析步骤: %1").arg(lastStep.step);
            errorMsg += tr("\n最后动作: %1").arg(lastStep.action);
            if (!lastStep.production.isEmpty()) {
                errorMsg += tr("\n产生式: %1").arg(lastStep.production);
            }
        }
        
        QMessageBox::warning(this, tr("语法分析失败"), errorMsg);
    }
}

void Task2Window::on_pushButtonSaveSyntaxTree_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存语法树"), ".", tr("图片文件 (*.png *.jpg);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        // 简化实现，实际语法树生成需要额外逻辑
        QMessageBox::information(this, tr("提示"), tr("语法树保存功能暂未实现"));
    }
}

void Task2Window::on_comboBoxLR0View_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // 简单的LR0视图切换，实际功能暂未实现
}

void Task2Window::on_comboBoxLR1View_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // 简单的LR1视图切换，实际功能暂未实现
}

void Task2Window::displayFirstSet()
{
    // 清空表格
    ui->tableWidgetFirst->setRowCount(0);
    
    // 获取First集合
    const auto& firstMap = m_ll1Info.first;
    
    // 显示First集合
    int row = 0;
    for (auto it = firstMap.constBegin(); it != firstMap.constEnd(); ++it) {
        const QString& symbol = it.key();
        // 只显示非终结符的First集合
        if (m_grammar.nonterminals.contains(symbol)) {
            const QSet<QString>& firstSet = it.value();
            
            // 添加行
            ui->tableWidgetFirst->insertRow(row);
            
            // 显示非终结符
            ui->tableWidgetFirst->setItem(row, 0, new QTableWidgetItem(symbol));
            
            // 显示First集合
            QString firstStr;
            if (firstSet.isEmpty()) {
                firstStr = "∅";
            } else {
                for (const QString& s : firstSet) {
                    if (!firstStr.isEmpty()) {
                        firstStr += ", ";
                    }
                    firstStr += s;
                }
            }
            ui->tableWidgetFirst->setItem(row, 1, new QTableWidgetItem(firstStr));
            
            row++;
        }
    }
    
    // 调整列宽
    ui->tableWidgetFirst->resizeColumnsToContents();
}

void Task2Window::displayFollowSet()
{
    // 清空表格
    ui->tableWidgetFollow->setRowCount(0);
    
    // 获取Follow集合
    const auto& followMap = m_ll1Info.follow;
    
    // 显示Follow集合
    int row = 0;
    for (auto it = followMap.constBegin(); it != followMap.constEnd(); ++it) {
        const QString& nonterminal = it.key();
        const QSet<QString>& followSet = it.value();
        
        // 添加行
        ui->tableWidgetFollow->insertRow(row);
        
        // 显示非终结符
        ui->tableWidgetFollow->setItem(row, 0, new QTableWidgetItem(nonterminal));
        
        // 显示Follow集合
        QString followStr;
        if (followSet.isEmpty()) {
            followStr = "∅";
        } else {
            for (const QString& s : followSet) {
                if (!followStr.isEmpty()) {
                    followStr += ", ";
                }
                followStr += s;
            }
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
    
    // 获取LR(0)项目集和转换
    const auto& states = m_lr0Graph.states;
    const auto& edges = m_lr0Graph.edges;
    
    // 显示LR(0) DFA
    for (int i = 0; i < states.size(); ++i) {
        const auto& state = states[i];
        
        // 添加行
        ui->tableWidgetLR0->insertRow(i);
        
        // 显示状态编号
        ui->tableWidgetLR0->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
        
        // 显示项目集
        QString itemsStr;
        for (const auto& item : state) {
            if (!itemsStr.isEmpty()) {
                itemsStr += "\n";
            }
            itemsStr += item.left + " -> ";
            for (int j = 0; j < item.right.size(); ++j) {
                if (j == item.dot) {
                    itemsStr += "• ";
                }
                itemsStr += item.right[j] + " ";
            }
            if (item.dot == item.right.size()) {
                itemsStr += "•";
            }
        }
        ui->tableWidgetLR0->setItem(i, 1, new QTableWidgetItem(itemsStr));
        
        // 显示转换
        QString transitionsStr;
        if (edges.contains(i)) {
            const auto& stateEdges = edges[i];
            for (auto it = stateEdges.constBegin(); it != stateEdges.constEnd(); ++it) {
                if (!transitionsStr.isEmpty()) {
                    transitionsStr += ", ";
                }
                transitionsStr += it.key() + " → " + QString::number(it.value());
            }
        }
        ui->tableWidgetLR0->setItem(i, 2, new QTableWidgetItem(transitionsStr));
    }
    
    // 调整列宽
    ui->tableWidgetLR0->resizeColumnsToContents();
}

void Task2Window::displaySLR1Result()
{
    // 获取SLR(1)检查结果
    const auto& conflicts = m_slrResult.conflicts;
    
    if (conflicts.isEmpty()) {
        QMessageBox::information(this, tr("SLR(1)文法检查结果"), tr("该文法为SLR(1)文法，无冲突"));
    } else {
        QString conflictMsg = tr("该文法不是SLR(1)文法，存在以下冲突：\n");
        for (const auto& conflict : conflicts) {
            conflictMsg += tr("状态%1: %2冲突，终端符%3\n").arg(conflict.state).arg(conflict.type).arg(conflict.terminal);
            if (!conflict.detail.isEmpty()) {
                conflictMsg += tr("详细信息: %1\n").arg(conflict.detail);
            }
        }
        QMessageBox::warning(this, tr("SLR(1)文法检查结果"), conflictMsg);
    }
}

void Task2Window::displayLR1DFA()
{
    // 清空表格
    ui->tableWidgetLR1->setRowCount(0);
    
    // 获取LR(1)项目集和转换
    const auto& states = m_lr1Graph.states;
    const auto& edges = m_lr1Graph.edges;
    
    // 显示LR(1) DFA
    for (int i = 0; i < states.size(); ++i) {
        const auto& state = states[i];
        
        // 添加行
        ui->tableWidgetLR1->insertRow(i);
        
        // 显示状态编号
        ui->tableWidgetLR1->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
        
        // 显示项目集
        QString itemsStr;
        for (const auto& item : state) {
            if (!itemsStr.isEmpty()) {
                itemsStr += "\n";
            }
            itemsStr += item.left + " -> ";
            for (int j = 0; j < item.right.size(); ++j) {
                if (j == item.dot) {
                    itemsStr += "• ";
                }
                itemsStr += item.right[j] + " ";
            }
            if (item.dot == item.right.size()) {
                itemsStr += "•";
            }
            itemsStr += " / " + item.lookahead;
        }
        ui->tableWidgetLR1->setItem(i, 1, new QTableWidgetItem(itemsStr));
        
        // 显示转换
        QString transitionsStr;
        if (edges.contains(i)) {
            const auto& stateEdges = edges[i];
            for (auto it = stateEdges.constBegin(); it != stateEdges.constEnd(); ++it) {
                if (!transitionsStr.isEmpty()) {
                    transitionsStr += ", ";
                }
                transitionsStr += it.key() + " → " + QString::number(it.value());
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
    
    // 获取LR(1)分析表
    const auto& actionTable = m_lr1Table.action;
    const auto& gotoTable = m_lr1Table.gotoTable;
    
    // 收集所有状态和符号
    QSet<int> states;
    QSet<QString> terminals;
    QSet<QString> nonterminals;
    
    // 收集Action表中的状态和终结符
    for (auto actionIt = actionTable.constBegin(); actionIt != actionTable.constEnd(); ++actionIt) {
        int state = actionIt.key();
        states.insert(state);
        const auto& symbolActions = actionIt.value();
        for (auto symbolIt = symbolActions.constBegin(); symbolIt != symbolActions.constEnd(); ++symbolIt) {
            terminals.insert(symbolIt.key());
        }
    }
    
    // 收集Goto表中的状态和非终结符
    for (auto gotoIt = gotoTable.constBegin(); gotoIt != gotoTable.constEnd(); ++gotoIt) {
        int state = gotoIt.key();
        states.insert(state);
        const auto& symbolStates = gotoIt.value();
        for (auto symbolIt = symbolStates.constBegin(); symbolIt != symbolStates.constEnd(); ++symbolIt) {
            nonterminals.insert(symbolIt.key());
        }
    }
    
    // 显示LR(1)分析表
    for (int state : states) {
        // 添加行
        int row = ui->tableWidgetLR1Table->rowCount();
        ui->tableWidgetLR1Table->insertRow(row);
        
        // 显示状态编号
        ui->tableWidgetLR1Table->setItem(row, 0, new QTableWidgetItem(QString::number(state)));
        
        // 显示Action表
        QString actionStr;
        if (actionTable.contains(state)) {
            const auto& symbolActions = actionTable[state];
            for (auto symbolIt = symbolActions.constBegin(); symbolIt != symbolActions.constEnd(); ++symbolIt) {
                if (!actionStr.isEmpty()) {
                    actionStr += ", ";
                }
                actionStr += symbolIt.key() + ": " + symbolIt.value();
            }
        }
        ui->tableWidgetLR1Table->setItem(row, 1, new QTableWidgetItem(actionStr));
        
        // 显示Goto表
        QString gotoStr;
        if (gotoTable.contains(state)) {
            const auto& symbolStates = gotoTable[state];
            for (auto symbolIt = symbolStates.constBegin(); symbolIt != symbolStates.constEnd(); ++symbolIt) {
                if (!gotoStr.isEmpty()) {
                    gotoStr += ", ";
                }
                gotoStr += symbolIt.key() + ": " + QString::number(symbolIt.value());
            }
        }
        ui->tableWidgetLR1Table->setItem(row, 2, new QTableWidgetItem(gotoStr));
    }
    
    // 调整列宽
    ui->tableWidgetLR1Table->resizeColumnsToContents();
}

void Task2Window::displaySyntaxAnalysisResult()
{
    // 清空表格
    ui->tableWidgetAnalysisSteps->setRowCount(0);
    
    // 获取语法分析步骤
    const auto& analysisSteps = m_parseResult.steps;
    
    // 显示语法分析结果
    for (int i = 0; i < analysisSteps.size(); ++i) {
        const auto& step = analysisSteps[i];
        
        // 添加行
        ui->tableWidgetAnalysisSteps->insertRow(i);
        
        // 显示步骤编号
        ui->tableWidgetAnalysisSteps->setItem(i, 0, new QTableWidgetItem(QString::number(step.step)));
        
        // 显示状态栈
        QString stateStackStr;
        for (const auto& item : step.stack) {
            stateStackStr += QString::number(item.first) + " ";
        }
        ui->tableWidgetAnalysisSteps->setItem(i, 1, new QTableWidgetItem(stateStackStr.trimmed()));
        
        // 显示符号栈
        QString symbolStackStr;
        for (const auto& item : step.stack) {
            symbolStackStr += item.second + " ";
        }
        ui->tableWidgetAnalysisSteps->setItem(i, 2, new QTableWidgetItem(symbolStackStr.trimmed()));
        
        // 显示输入串
        QString inputStr;
        for (const auto& token : step.rest) {
            inputStr += token + " ";
        }
        ui->tableWidgetAnalysisSteps->setItem(i, 3, new QTableWidgetItem(inputStr.trimmed()));
        
        // 显示动作
        QString actionStr = step.action;
        if (!step.production.isEmpty()) {
            actionStr += " (" + step.production + ")";
        }
        ui->tableWidgetAnalysisSteps->setItem(i, 4, new QTableWidgetItem(actionStr));
    }
    
    // 调整列宽
    ui->tableWidgetAnalysisSteps->resizeColumnsToContents();
    
    // 显示语法树
    ui->treeWidgetSyntaxTree->clear();
    // 简化实现，实际语法树显示需要额外逻辑
    QTreeWidgetItem* root = new QTreeWidgetItem(ui->treeWidgetSyntaxTree);
    root->setText(0, tr("语法树"));
    ui->treeWidgetSyntaxTree->expandAll();
}

bool Task2Window::loadTokenMap(const QString &mapPath)
{
    QFile mapFile(mapPath);
    if (mapFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&mapFile);
        m_tokenMap.clear();
        m_singleCodeTokens.clear();
        
        int lineCount = 0;
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            lineCount++;
            
            if (line.isEmpty() || line.startsWith("//")) {
                continue; // 跳过空行和注释行
            }
            
            // 查找第一个等号位置
            int equalsPos = line.indexOf('=');
            if (equalsPos == -1) {
                qWarning() << "Invalid token map line" << lineCount << ":" << line;
                continue;
            }
            
            // 提取编码和token部分，只使用第一个等号作为分隔符
            QString code = line.left(equalsPos).trimmed();
            QString tokenPart = line.mid(equalsPos + 1).trimmed();
            
            // 检查是否为单编码token
            bool isSingleCode = false;
            QString tokenName;
            if (tokenPart.contains("|single")) {
                isSingleCode = true;
                tokenName = tokenPart.left(tokenPart.indexOf("|single")).trimmed();
            } else {
                tokenName = tokenPart;
            }
            
            // 存储映射关系
            m_tokenMap[code] = tokenName;
            
            // 存储单编码token标记
            if (isSingleCode) {
                m_singleCodeTokens.insert(tokenName);
            }
        }
        
        mapFile.close();
        return true;
    }
    return false;
}

void Task2Window::on_pushButtonLoadTokenMap_clicked()
{
    QString mapPath = QFileDialog::getOpenFileName(this, tr("打开Token映射文件"), ".", tr("Token映射文件 (*.tokenmap);;所有文件 (*.*)"));
    if (!mapPath.isEmpty()) {
        bool success = loadTokenMap(mapPath);
        if (success) {
            QMessageBox::information(this, tr("成功"), tr("Token映射文件加载成功，共加载 %1 个映射").arg(m_tokenMap.size()));
        } else {
            QMessageBox::warning(this, tr("错误"), tr("无法加载Token映射文件"));
        }
    }
}
