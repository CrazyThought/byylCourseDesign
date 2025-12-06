/**
 * @file lr0dfa.cpp
 * @brief LR(0) DFA生成模块实现
 * @version 1.0
 * @author 编译原理课程设计小组
 * @date 2025-12-06
 * @copyright Copyright (c) 2025 编译原理课程设计
 * @details 实现了LR(0) DFA的生成功能
 */

#include "task2/lr0dfa.h"
#include "task2/bnfparser.h"
#include <QDebug>

// LR0Item结构体实现
bool LR0Item::operator==(const LR0Item &other) const
{
    return production.nonTerminal == other.production.nonTerminal && 
           production.candidates == other.production.candidates && 
           dotPosition == other.dotPosition && 
           productionIndex == other.productionIndex;
}

bool LR0Item::operator<(const LR0Item &other) const
{
    if (production.nonTerminal < other.production.nonTerminal) {
        return true;
    } else if (production.nonTerminal > other.production.nonTerminal) {
        return false;
    }
    
    if (productionIndex < other.productionIndex) {
        return true;
    } else if (productionIndex > other.productionIndex) {
        return false;
    }
    
    return dotPosition < other.dotPosition;
}

QString LR0Item::getSymbolAfterDot() const
{
    if (isDotAtEnd()) {
        return "";
    }
    
    if (production.candidates.isEmpty()) {
        return "";
    }
    
    // 假设每个产生式只有一个候选式
    if (production.candidates.isEmpty() || production.candidates.first().size() <= dotPosition) {
        return "";
    }
    
    return production.candidates.first()[dotPosition];
}

bool LR0Item::isDotAtEnd() const
{
    if (production.candidates.isEmpty()) {
        return true;
    }
    
    // 假设每个产生式只有一个候选式
    return production.candidates.first().size() <= dotPosition;
}

bool LR0Item::isDotAtBegin() const
{
    return dotPosition == 0;
}

QStringList LR0Item::getSymbolsAfterDot() const
{
    if (isDotAtEnd() || production.candidates.isEmpty()) {
        return QStringList();
    }
    
    // 假设每个产生式只有一个候选式
    return production.candidates.first().mid(dotPosition);
}

QStringList LR0Item::getSymbolsBeforeDot() const
{
    if (production.candidates.isEmpty()) {
        return QStringList();
    }
    
    // 假设每个产生式只有一个候选式
    return production.candidates.first().mid(0, dotPosition);
}

LR0Item LR0Item::moveDotForward() const
{
    LR0Item newItem = *this;
    newItem.dotPosition++;
    return newItem;
}

QString LR0Item::toString() const
{
    QString result = production.nonTerminal + " -> ";
    
    if (production.candidates.isEmpty()) {
        result += "#";
    } else {
        // 假设每个产生式只有一个候选式
        const QStringList &candidate = production.candidates.first();
        
        for (int i = 0; i < candidate.size(); ++i) {
            if (i == dotPosition) {
                result += ". ";
            }
            result += candidate[i] + " ";
        }
        
        if (dotPosition >= candidate.size()) {
            result += ".";
        }
    }
    
    return result;
}

// LR0ItemSet结构体实现
bool LR0ItemSet::operator==(const LR0ItemSet &other) const
{
    return items == other.items;
}

bool LR0ItemSet::operator<(const LR0ItemSet &other) const
{
    return items.size() < other.items.size();
}

bool LR0ItemSet::containsItem(const LR0Item &item) const
{
    return items.contains(item);
}

void LR0ItemSet::merge(const LR0ItemSet &other)
{
    items.unite(other.items);
}

QString LR0ItemSet::toString() const
{
    QString result = "State " + QString::number(stateId) + "\n";
    for (const LR0Item &item : items) {
        result += "  " + item.toString() + "\n";
    }
    return result;
}

QSet<QString> LR0ItemSet::getSymbolsAfterDot() const
{
    QSet<QString> symbols;
    
    for (const LR0Item &item : items) {
        QString symbol = item.getSymbolAfterDot();
        if (!symbol.isEmpty()) {
            symbols.insert(symbol);
        }
    }
    
    return symbols;
}

LR0ItemSet LR0ItemSet::getSubsetBySymbol(const QString &symbol) const
{
    LR0ItemSet subset;
    subset.stateId = stateId;
    
    for (const LR0Item &item : items) {
        if (item.getSymbolAfterDot() == symbol) {
            subset.items.insert(item);
        }
    }
    
    return subset;
}

// LR0Transition结构体实现
bool LR0Transition::operator==(const LR0Transition &other) const
{
    return fromState == other.fromState && 
           symbol == other.symbol && 
           toState == other.toState;
}

QString LR0Transition::toString() const
{
    return QString("%1 --%2--> %3").arg(fromState).arg(symbol).arg(toState);
}

// LR0DFA类实现
LR0DFA::LR0DFA()
    : m_parser(nullptr)
    , m_stateCount(0)
    , m_startStateId(-1)
    , m_acceptStateId(-1)
{
}

LR0DFA::~LR0DFA()
{
    clear();
}

void LR0DFA::setParser(BNFParser *parser)
{
    m_parser = parser;
}

bool LR0DFA::generate()
{
    if (!m_parser) {
        m_errorMessage = "BNF解析器未设置";
        return false;
    }
    
    clear();
    
    // 初始化开始项目集
    LR0ItemSet startItemSet = initStartItemSet();
    startItemSet.stateId = 0;
    
    // 计算闭包
    LR0ItemSet startClosure = closure(startItemSet);
    startClosure.stateId = 0;
    
    // 添加到项目集规范族
    m_itemSets.append(startClosure);
    m_stateToItemSetMap[0] = startClosure;
    m_itemSetToStateMap[startClosure] = 0;
    m_startStateId = 0;
    m_stateCount = 1;
    
    // 使用队列进行广度优先搜索
    QList<int> queue;
    queue.append(0);
    
    while (!queue.isEmpty()) {
        int currentState = queue.takeFirst();
        LR0ItemSet currentItemSet = m_stateToItemSetMap[currentState];
        
        // 获取当前项目集所有点后的符号
        QSet<QString> symbols = currentItemSet.getSymbolsAfterDot();
        
        for (const QString &symbol : symbols) {
            // 执行GOTO操作
            LR0ItemSet gotoItemSet = gotoOperation(currentItemSet, symbol);
            
            if (gotoItemSet.items.isEmpty()) {
                continue;
            }
            
            // 检查是否已经存在该项目集
            int existingIndex = findItemSetIndex(gotoItemSet);
            int nextState;
            
            if (existingIndex == -1) {
                // 新的项目集，分配新状态ID
                nextState = m_stateCount++;
                gotoItemSet.stateId = nextState;
                m_itemSets.append(gotoItemSet);
                m_stateToItemSetMap[nextState] = gotoItemSet;
                m_itemSetToStateMap[gotoItemSet] = nextState;
                queue.append(nextState);
            } else {
                // 已经存在，使用现有状态ID
                nextState = existingIndex;
            }
            
            // 添加状态转移
            LR0Transition transition;
            transition.fromState = currentState;
            transition.symbol = symbol;
            transition.toState = nextState;
            m_transitions.append(transition);
        }
    }
    
    // 查找接受状态
    for (const LR0ItemSet &itemSet : m_itemSets) {
        for (const LR0Item &item : itemSet.items) {
            if (isAcceptItem(item)) {
                m_acceptStateId = itemSet.stateId;
                break;
            }
        }
        
        if (m_acceptStateId != -1) {
            break;
        }
    }
    
    return true;
}

QList<LR0ItemSet> LR0DFA::getItemSets() const
{
    return m_itemSets;
}

QList<LR0Transition> LR0DFA::getTransitions() const
{
    return m_transitions;
}

int LR0DFA::getStateCount() const
{
    return m_stateCount;
}

int LR0DFA::getStartStateId() const
{
    return m_startStateId;
}

int LR0DFA::getAcceptStateId() const
{
    return m_acceptStateId;
}

QString LR0DFA::getErrorMessage() const
{
    return m_errorMessage;
}

QString LR0DFA::printDFA() const
{
    QString result = "LR(0) DFA\n";
    result += "状态数量: " + QString::number(m_stateCount) + "\n";
    result += "开始状态: " + QString::number(m_startStateId) + "\n";
    result += "接受状态: " + QString::number(m_acceptStateId) + "\n\n";
    
    // 打印项目集
    for (const LR0ItemSet &itemSet : m_itemSets) {
        result += itemSet.toString() + "\n";
    }
    
    // 打印状态转移
    result += "状态转移:\n";
    for (const LR0Transition &transition : m_transitions) {
        result += "  " + transition.toString() + "\n";
    }
    
    return result;
}

void LR0DFA::clear()
{
    m_itemSets.clear();
    m_transitions.clear();
    m_stateCount = 0;
    m_startStateId = -1;
    m_acceptStateId = -1;
    m_errorMessage.clear();
    m_stateToItemSetMap.clear();
    m_itemSetToStateMap.clear();
}

LR0ItemSet LR0DFA::getItemSetByStateId(int stateId) const
{
    return m_stateToItemSetMap.value(stateId);
}

int LR0DFA::getNextState(int stateId, const QString &symbol) const
{
    for (const LR0Transition &transition : m_transitions) {
        if (transition.fromState == stateId && transition.symbol == symbol) {
            return transition.toState;
        }
    }
    
    return -1;
}

bool LR0DFA::isAcceptState(int stateId) const
{
    return stateId == m_acceptStateId;
}

bool LR0DFA::isStartState(int stateId) const
{
    return stateId == m_startStateId;
}

LR0ItemSet LR0DFA::closure(const LR0ItemSet &itemSet) const
{
    LR0ItemSet result = itemSet;
    
    // 使用队列进行广度优先搜索
    QList<LR0Item> queue;
    for (const LR0Item &item : itemSet.items) {
        queue.append(item);
    }
    
    while (!queue.isEmpty()) {
        LR0Item currentItem = queue.takeFirst();
        
        // 获取点后的符号
        QString symbolAfterDot = currentItem.getSymbolAfterDot();
        
        // 如果点后的符号是非终结符，添加其产生式到闭包
        if (m_parser->isNonTerminal(symbolAfterDot)) {
            // 获取所有以该非终结符为左部的产生式
            QList<Production> productions = m_parser->getProductionsByNonTerminal(symbolAfterDot);
            
            for (const Production &production : productions) {
                for (int i = 0; i < production.candidates.size(); ++i) {
                    // 创建新的项目，点在产生式开头
                    LR0Item newItem;
                    newItem.production = production;
                    newItem.dotPosition = 0;
                    newItem.productionIndex = i;
                    
                    // 如果新项目不在结果中，添加到结果和队列
                    if (!result.containsItem(newItem)) {
                        result.items.insert(newItem);
                        queue.append(newItem);
                    }
                }
            }
        }
    }
    
    return result;
}

LR0ItemSet LR0DFA::gotoOperation(const LR0ItemSet &itemSet, const QString &symbol) const
{
    LR0ItemSet result;
    result.stateId = itemSet.stateId;
    
    // 获取所有点后是该符号的项目
    LR0ItemSet subset = itemSet.getSubsetBySymbol(symbol);
    
    // 对点后的符号执行moveDotForward操作
    for (const LR0Item &item : subset.items) {
        LR0Item newItem = item.moveDotForward();
        result.items.insert(newItem);
    }
    
    // 计算闭包
    return closure(result);
}

LR0ItemSet LR0DFA::initStartItemSet() const
{
    LR0ItemSet result;
    
    // 获取开始符号
    QString startSymbol = m_parser->getStartSymbol();
    
    // 获取开始符号的产生式
    QList<Production> startProductions = m_parser->getProductionsByNonTerminal(startSymbol);
    
    if (startProductions.isEmpty()) {
        return result;
    }
    
    // 创建开始项目，点在产生式开头
    LR0Item startItem;
    startItem.production = startProductions.first();
    startItem.dotPosition = 0;
    startItem.productionIndex = 0;
    
    result.items.insert(startItem);
    return result;
}

int LR0DFA::findItemSetIndex(const LR0ItemSet &itemSet) const
{
    for (int i = 0; i < m_itemSets.size(); ++i) {
        if (areItemSetsEqual(m_itemSets[i], itemSet)) {
            return i;
        }
    }
    
    return -1;
}

bool LR0DFA::areItemSetsEqual(const LR0ItemSet &set1, const LR0ItemSet &set2) const
{
    return set1.items == set2.items;
}

bool LR0DFA::generateTransitions()
{
    // 这个函数已经在generate()中实现了
    return true;
}

bool LR0DFA::isAcceptItem(const LR0Item &item) const
{
    // 检查是否是扩展文法的接受项目
    QString startSymbol = m_parser->getStartSymbol();
    return item.production.nonTerminal == startSymbol && item.isDotAtEnd();
}

QList<Production> LR0DFA::getProductions() const
{
    if (!m_parser) {
        return QList<Production>();
    }
    
    return m_parser->getProductions();
}

QSet<QString> LR0DFA::getNonTerminals() const
{
    if (!m_parser) {
        return QSet<QString>();
    }
    
    return m_parser->getNonTerminals();
}

QSet<QString> LR0DFA::getTerminals() const
{
    if (!m_parser) {
        return QSet<QString>();
    }
    
    return m_parser->getTerminals();
}

QString LR0DFA::getStartSymbol() const
{
    if (!m_parser) {
        return QString();
    }
    
    return m_parser->getStartSymbol();
}
