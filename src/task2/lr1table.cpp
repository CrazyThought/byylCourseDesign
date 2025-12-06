/**
 * @file lr1table.cpp
 * @brief LR(1)分析表生成模块实现
 * @version 1.0
 * @author 编译原理课程设计小组
 * @date 2025-12-06
 * @copyright Copyright (c) 2025 编译原理课程设计
 * @details 实现了LR(1)分析表的生成功能
 */

#include "task2/lr1table.h"
#include "task2/lr1dfa.h"
#include "task2/bnfparser.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>

// Action结构体实现
bool Action::operator==(const Action &other) const
{
    return type == other.type && value == other.value;
}

QString Action::toString() const
{
    switch (type) {
    case ACTION_ERROR:
        return "error";
    case ACTION_SHIFT:
        return "s" + QString::number(value);
    case ACTION_REDUCE:
        return "r" + QString::number(value);
    case ACTION_ACCEPT:
        return "acc";
    case ACTION_GOTO:
        return "goto " + QString::number(value);
    default:
        return "unknown";
    }
}

// LR1Table类实现
LR1Table::LR1Table()
    : m_lr1dfa(nullptr)
    , m_parser(nullptr)
    , m_conflictCount(0)
{
}

LR1Table::~LR1Table()
{
    clear();
}

void LR1Table::setLR1DFA(LR1DFA *lr1dfa)
{
    m_lr1dfa = lr1dfa;
}

void LR1Table::setParser(BNFParser *parser)
{
    m_parser = parser;
}

bool LR1Table::generateLR1Table()
{
    if (!m_lr1dfa || !m_parser) {
        m_errorMessage = "LR1DFA或BNF解析器未设置";
        return false;
    }
    
    clear();
    
    // 获取所有产生式
    m_productions = m_parser->getProductions();
    
    // 获取所有终结符和非终结符
    m_terminals = m_parser->getTerminals();
    m_nonTerminals = m_parser->getNonTerminals();
    
    // 添加结束符
    m_terminals.insert("$");
    
    // 获取所有项目集
    QList<LR1ItemSet> itemSets = m_lr1dfa->getItemSets();
    
    // 处理每个状态
    for (int stateId = 0; stateId < itemSets.size(); ++stateId) {
        LR1ItemSet itemSet = itemSets[stateId];
        
        // 处理移进行动
        if (!handleShiftActions(stateId, itemSet)) {
            return false;
        }
        
        // 处理归约动作
        if (!handleReduceActions(stateId, itemSet)) {
            return false;
        }
        
        // 处理接受动作
        if (!handleAcceptActions(stateId, itemSet)) {
            return false;
        }
        
        // 处理Goto动作
        if (!handleGotoActions(stateId)) {
            return false;
        }
    }
    
    return true;
}

QMap<int, QMap<QString, Action>> LR1Table::getActionTable() const
{
    return m_actionTable;
}

QMap<int, QMap<QString, int>> LR1Table::getGotoTable() const
{
    return m_gotoTable;
}

QString LR1Table::getErrorMessage() const
{
    return m_errorMessage;
}

void LR1Table::clear()
{
    m_actionTable.clear();
    m_gotoTable.clear();
    m_errorMessage.clear();
    m_conflicts.clear();
    m_conflictCount = 0;
    m_terminals.clear();
    m_nonTerminals.clear();
    m_productions.clear();
}

QString LR1Table::printLR1Table() const
{
    QString result = "LR(1)分析表\n\n";
    
    // 打印Action表
    result += "Action表:\n";
    for (int stateId : m_actionTable.keys()) {
        result += QString("状态 %1:\n").arg(stateId);
        QMap<QString, Action> actions = m_actionTable[stateId];
        for (const QString &symbol : actions.keys()) {
            result += QString("  %1: %2\n").arg(symbol).arg(actions[symbol].toString());
        }
    }
    
    // 打印Goto表
    result += "\nGoto表:\n";
    for (int stateId : m_gotoTable.keys()) {
        result += QString("状态 %1:\n").arg(stateId);
        QMap<QString, int> gotos = m_gotoTable[stateId];
        for (const QString &nonTerminal : gotos.keys()) {
            result += QString("  %1: %2\n").arg(nonTerminal).arg(gotos[nonTerminal]);
        }
    }
    
    // 打印冲突信息
    if (hasConflicts()) {
        result += "\n冲突信息:\n";
        for (const QString &conflict : m_conflicts) {
            result += "  " + conflict + "\n";
        }
    }
    
    return result;
}

Action LR1Table::getAction(int stateId, const QString &symbol) const
{
    if (m_actionTable.contains(stateId) && m_actionTable[stateId].contains(symbol)) {
        return m_actionTable[stateId][symbol];
    }
    
    Action errorAction;
    errorAction.type = ACTION_ERROR;
    errorAction.value = -1;
    return errorAction;
}

int LR1Table::getGoto(int stateId, const QString &nonTerminal) const
{
    if (m_gotoTable.contains(stateId) && m_gotoTable[stateId].contains(nonTerminal)) {
        return m_gotoTable[stateId][nonTerminal];
    }
    
    return -1;
}

QList<int> LR1Table::getAllStates() const
{
    return m_actionTable.keys();
}

QSet<QString> LR1Table::getAllTerminals() const
{
    return m_terminals;
}

QSet<QString> LR1Table::getAllNonTerminals() const
{
    return m_nonTerminals;
}

bool LR1Table::saveToCSV(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_errorMessage = "无法打开文件: " + file.errorString();
        return false;
    }
    
    QTextStream out(&file);
    
    // 写入Action表
    out << "Action表\n";
    out << "状态,符号,动作\n";
    for (int stateId : m_actionTable.keys()) {
        QMap<QString, Action> actions = m_actionTable[stateId];
        for (const QString &symbol : actions.keys()) {
            Action action = actions[symbol];
            out << stateId << "," << symbol << "," << action.toString() << "\n";
        }
    }
    
    // 写入Goto表
    out << "\nGoto表\n";
    out << "状态,非终结符,目标状态\n";
    for (int stateId : m_gotoTable.keys()) {
        QMap<QString, int> gotos = m_gotoTable[stateId];
        for (const QString &nonTerminal : gotos.keys()) {
            int gotoState = gotos[nonTerminal];
            out << stateId << "," << nonTerminal << "," << gotoState << "\n";
        }
    }
    
    file.close();
    return true;
}

bool LR1Table::hasConflicts() const
{
    return !m_conflicts.isEmpty();
}

int LR1Table::getConflictCount() const
{
    return m_conflictCount;
}

QList<QString> LR1Table::getConflictInfo() const
{
    return m_conflicts;
}

bool LR1Table::handleShiftActions(int stateId, const LR1ItemSet &itemSet)
{
    // 获取所有符号
    QSet<QString> symbols = itemSet.getSymbolsAfterDot();
    
    for (const QString &symbol : symbols) {
        // 执行GOTO操作，获取下一个状态
        int nextState = m_lr1dfa->getNextState(stateId, symbol);
        if (nextState == -1) {
            continue;
        }
        
        // 创建移进行动
        Action action;
        action.type = ACTION_SHIFT;
        action.value = nextState;
        
        // 检查是否有冲突
        if (checkActionConflict(stateId, symbol, action)) {
            QString conflictMsg = QString("状态 %1，符号 %2: 移进-移进冲突或移进-归约冲突").arg(stateId).arg(symbol);
            m_conflicts.append(conflictMsg);
            m_conflictCount++;
        }
        
        // 添加到Action表
        m_actionTable[stateId][symbol] = action;
    }
    
    return true;
}

bool LR1Table::handleReduceActions(int stateId, const LR1ItemSet &itemSet)
{
    // 获取所有归约项目
    QList<LR1Item> reduceItems = getReduceItems(itemSet);
    
    for (const LR1Item &item : reduceItems) {
        // 对于每个向前看符号，添加归约动作
        for (const QString &lookahead : item.lookaheadSymbols) {
            // 创建归约动作
            Action action;
            action.type = ACTION_REDUCE;
            action.value = item.productionIndex;
            
            // 检查是否有冲突
            if (checkActionConflict(stateId, lookahead, action)) {
                QString conflictMsg = QString("状态 %1，符号 %2: 归约-归约冲突或移进-归约冲突").arg(stateId).arg(lookahead);
                m_conflicts.append(conflictMsg);
                m_conflictCount++;
            }
            
            // 添加到Action表
            m_actionTable[stateId][lookahead] = action;
        }
    }
    
    return true;
}

bool LR1Table::handleAcceptActions(int stateId, const LR1ItemSet &itemSet)
{
    // 获取所有接受项目
    QList<LR1Item> acceptItems = getAcceptItems(itemSet);
    
    for (const LR1Item &item : acceptItems) {
        // 对于每个向前看符号，添加接受动作
        for (const QString &lookahead : item.lookaheadSymbols) {
            // 创建接受动作
            Action action;
            action.type = ACTION_ACCEPT;
            action.value = 0;
            
            // 检查是否有冲突
            if (checkActionConflict(stateId, lookahead, action)) {
                QString conflictMsg = QString("状态 %1，符号 %2: 接受动作冲突").arg(stateId).arg(lookahead);
                m_conflicts.append(conflictMsg);
                m_conflictCount++;
            }
            
            // 添加到Action表
            m_actionTable[stateId][lookahead] = action;
        }
    }
    
    return true;
}

bool LR1Table::handleGotoActions(int stateId)
{
    // 对于每个非终结符，执行GOTO操作
    for (const QString &nonTerminal : m_nonTerminals) {
        int nextState = m_lr1dfa->getNextState(stateId, nonTerminal);
        if (nextState != -1) {
            m_gotoTable[stateId][nonTerminal] = nextState;
        }
    }
    
    return true;
}

bool LR1Table::checkActionConflict(int stateId, const QString &symbol, const Action &newAction)
{
    // 检查是否已经有该状态和符号的动作
    if (m_actionTable.contains(stateId) && m_actionTable[stateId].contains(symbol)) {
        Action existingAction = m_actionTable[stateId][symbol];
        
        // 如果动作类型不同，或者动作值不同，则存在冲突
        if (existingAction.type != newAction.type || existingAction.value != newAction.value) {
            return true;
        }
    }
    
    return false;
}

QList<LR1Item> LR1Table::getReduceItems(const LR1ItemSet &itemSet) const
{
    QList<LR1Item> reduceItems;
    
    for (const LR1Item &item : itemSet.items) {
        if (item.isDotAtEnd() && !item.production.nonTerminal.isEmpty()) {
            reduceItems.append(item);
        }
    }
    
    return reduceItems;
}

QList<LR1Item> LR1Table::getAcceptItems(const LR1ItemSet &itemSet) const
{
    QList<LR1Item> acceptItems;
    
    // 获取开始符号
    QString startSymbol = m_parser->getStartSymbol();
    
    for (const LR1Item &item : itemSet.items) {
        if (item.isDotAtEnd() && item.production.nonTerminal == startSymbol) {
            acceptItems.append(item);
        }
    }
    
    return acceptItems;
}

QSet<QString> LR1Table::getTerminals() const
{
    if (!m_parser) {
        return QSet<QString>();
    }
    
    return m_parser->getTerminals();
}

QSet<QString> LR1Table::getNonTerminals() const
{
    if (!m_parser) {
        return QSet<QString>();
    }
    
    return m_parser->getNonTerminals();
}

QList<Production> LR1Table::getProductions() const
{
    if (!m_parser) {
        return QList<Production>();
    }
    
    return m_parser->getProductions();
}

Production LR1Table::getProductionByIndex(int index) const
{
    if (index < 0 || index >= m_productions.size()) {
        return Production();
    }
    
    return m_productions[index];
}
