/**
 * @file lr1dfa.cpp
 * @brief LR(1) DFA生成模块实现
 * @version 1.0
 * @author 编译原理课程设计小组
 * @date 2025-12-06
 * @copyright Copyright (c) 2025 编译原理课程设计
 * @details 实现了LR(1) DFA的生成功能
 */

#include "task2/lr1dfa.h"
#include "task2/bnfparser.h"
#include "task2/firstfollow.h"
#include <QDebug>

// LR1Item结构体实现
bool LR1Item::operator==(const LR1Item &other) const
{
    return production.nonTerminal == other.production.nonTerminal && 
           production.candidates == other.production.candidates && 
           dotPosition == other.dotPosition && 
           lookaheadSymbols == other.lookaheadSymbols && 
           productionIndex == other.productionIndex;
}

bool LR1Item::operator<(const LR1Item &other) const
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
    
    if (dotPosition < other.dotPosition) {
        return true;
    } else if (dotPosition > other.dotPosition) {
        return false;
    }
    
    // 比较向前看符号集合
    QList<QString> thisLookahead = lookaheadSymbols.values();
    QList<QString> otherLookahead = other.lookaheadSymbols.values();
    return thisLookahead.join(",") < otherLookahead.join(",");
}

QString LR1Item::getSymbolAfterDot() const
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

bool LR1Item::isDotAtEnd() const
{
    if (production.candidates.isEmpty()) {
        return true;
    }
    
    // 假设每个产生式只有一个候选式
    return production.candidates.first().size() <= dotPosition;
}

LR1Item LR1Item::moveDotForward() const
{
    LR1Item newItem = *this;
    newItem.dotPosition++;
    return newItem;
}

LR1Item LR1Item::mergeLookahead(const LR1Item &other) const
{
    if (!hasSameCore(other)) {
        return *this;
    }
    
    LR1Item newItem = *this;
    newItem.lookaheadSymbols.unite(other.lookaheadSymbols);
    return newItem;
}

QString LR1Item::toString() const
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
    
    // 添加向前看符号
    result += ", ";
    QList<QString> lookaheadList = lookaheadSymbols.values();
    result += lookaheadList.join(", ");
    
    return result;
}

bool LR1Item::hasSameCore(const LR1Item &other) const
{
    return production.nonTerminal == other.production.nonTerminal && 
           production.candidates == other.production.candidates && 
           dotPosition == other.dotPosition && 
           productionIndex == other.productionIndex;
}

// LR1ItemSet结构体实现
bool LR1ItemSet::operator==(const LR1ItemSet &other) const
{
    return items == other.items;
}

bool LR1ItemSet::operator<(const LR1ItemSet &other) const
{
    return items.size() < other.items.size();
}

void LR1ItemSet::merge(const LR1ItemSet &other)
{
    for (const LR1Item &otherItem : other.items) {
        bool found = false;
        
        // 检查是否已经存在相同核心的项目
        for (const LR1Item &item : items) {
            if (item.hasSameCore(otherItem)) {
                // 合并向前看符号
                LR1Item mergedItem = item.mergeLookahead(otherItem);
                items.remove(item);
                items.insert(mergedItem);
                found = true;
                break;
            }
        }
        
        // 如果不存在，添加新项目
        if (!found) {
            items.insert(otherItem);
        }
    }
}

bool LR1ItemSet::containsCoreItem(const LR1Item &item) const
{
    for (const LR1Item &setItem : items) {
        if (setItem.hasSameCore(item)) {
            return true;
        }
    }
    return false;
}

QString LR1ItemSet::toString() const
{
    QString result = "State " + QString::number(stateId) + "\n";
    for (const LR1Item &item : items) {
        result += "  " + item.toString() + "\n";
    }
    return result;
}

QSet<QString> LR1ItemSet::getSymbolsAfterDot() const
{
    QSet<QString> symbols;
    
    for (const LR1Item &item : items) {
        QString symbol = item.getSymbolAfterDot();
        if (!symbol.isEmpty()) {
            symbols.insert(symbol);
        }
    }
    
    return symbols;
}

LR1ItemSet LR1ItemSet::getSubsetBySymbol(const QString &symbol) const
{
    LR1ItemSet subset;
    subset.stateId = stateId;
    
    for (const LR1Item &item : items) {
        if (item.getSymbolAfterDot() == symbol) {
            subset.items.insert(item);
        }
    }
    
    return subset;
}

LR1Item LR1ItemSet::getItemByCore(const LR1Item &item) const
{
    for (const LR1Item &setItem : items) {
        if (setItem.hasSameCore(item)) {
            return setItem;
        }
    }
    return LR1Item();
}

// LR1Transition结构体实现
bool LR1Transition::operator==(const LR1Transition &other) const
{
    return fromState == other.fromState && 
           symbol == other.symbol && 
           toState == other.toState;
}

QString LR1Transition::toString() const
{
    return QString("%1 --%2--> %3").arg(fromState).arg(symbol).arg(toState);
}

// LR1DFA类实现
LR1DFA::LR1DFA()
    : m_parser(nullptr)
    , m_firstFollow(nullptr)
    , m_stateCount(0)
    , m_startStateId(-1)
    , m_acceptStateId(-1)
{
}

LR1DFA::~LR1DFA()
{
    clear();
}

void LR1DFA::setParser(BNFParser *parser)
{
    m_parser = parser;
}

void LR1DFA::setFirstFollow(FirstFollow *firstFollow)
{
    m_firstFollow = firstFollow;
}

bool LR1DFA::generate()
{
    if (!m_parser || !m_firstFollow) {
        m_errorMessage = "BNF解析器或FirstFollow未设置";
        return false;
    }
    
    clear();
    
    // 初始化开始项目集
    LR1ItemSet startItemSet = initStartItemSet();
    startItemSet.stateId = 0;
    
    // 计算闭包
    LR1ItemSet startClosure = closure(startItemSet);
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
        LR1ItemSet currentItemSet = m_stateToItemSetMap[currentState];
        
        // 获取当前项目集所有点后的符号
        QSet<QString> symbols = currentItemSet.getSymbolsAfterDot();
        
        for (const QString &symbol : symbols) {
            // 执行GOTO操作
            LR1ItemSet gotoItemSet = gotoOperation(currentItemSet, symbol);
            
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
            LR1Transition transition;
            transition.fromState = currentState;
            transition.symbol = symbol;
            transition.toState = nextState;
            m_transitions.append(transition);
        }
    }
    
    // 查找接受状态
    for (const LR1ItemSet &itemSet : m_itemSets) {
        for (const LR1Item &item : itemSet.items) {
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

QList<LR1ItemSet> LR1DFA::getItemSets() const
{
    return m_itemSets;
}

QList<LR1Transition> LR1DFA::getTransitions() const
{
    return m_transitions;
}

int LR1DFA::getStateCount() const
{
    return m_stateCount;
}

int LR1DFA::getStartStateId() const
{
    return m_startStateId;
}

int LR1DFA::getAcceptStateId() const
{
    return m_acceptStateId;
}

QString LR1DFA::getErrorMessage() const
{
    return m_errorMessage;
}

void LR1DFA::clear()
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

QString LR1DFA::printDFA() const
{
    QString result = "LR(1) DFA\n";
    result += "状态数量: " + QString::number(m_stateCount) + "\n";
    result += "开始状态: " + QString::number(m_startStateId) + "\n";
    result += "接受状态: " + QString::number(m_acceptStateId) + "\n\n";
    
    // 打印项目集
    for (const LR1ItemSet &itemSet : m_itemSets) {
        result += itemSet.toString() + "\n";
    }
    
    // 打印状态转移
    result += "状态转移:\n";
    for (const LR1Transition &transition : m_transitions) {
        result += "  " + transition.toString() + "\n";
    }
    
    return result;
}

LR1ItemSet LR1DFA::getItemSetByStateId(int stateId) const
{
    return m_stateToItemSetMap.value(stateId);
}

int LR1DFA::getNextState(int stateId, const QString &symbol) const
{
    for (const LR1Transition &transition : m_transitions) {
        if (transition.fromState == stateId && transition.symbol == symbol) {
            return transition.toState;
        }
    }
    
    return -1;
}

LR1ItemSet LR1DFA::closure(const LR1ItemSet &itemSet) const
{
    LR1ItemSet result = itemSet;
    
    // 使用队列进行广度优先搜索
    QList<LR1Item> queue;
    for (const LR1Item &item : itemSet.items) {
        queue.append(item);
    }
    
    while (!queue.isEmpty()) {
        LR1Item currentItem = queue.takeFirst();
        
        // 获取点后的符号
        QString symbolAfterDot = currentItem.getSymbolAfterDot();
        
        // 如果点后的符号是非终结符，添加其产生式到闭包
        if (m_parser->isNonTerminal(symbolAfterDot)) {
            // 获取所有以该非终结符为左部的产生式
            QList<Production> productions = m_parser->getProductionsByNonTerminal(symbolAfterDot);
            
            // 计算向前看符号
            QStringList beta;
            if (currentItem.production.candidates.isEmpty()) {
                beta = QStringList();
            } else {
                // 假设每个产生式只有一个候选式
                beta = currentItem.production.candidates.first().mid(currentItem.dotPosition + 1);
            }
            
            // 计算FIRST(beta a)，其中a是当前项目的向前看符号
            QSet<QString> firstSet;
            for (const QString &lookahead : currentItem.lookaheadSymbols) {
                QStringList betaA = beta;
                betaA.append(lookahead);
                QSet<QString> tempFirst = m_firstFollow->getFirstOfString(betaA);
                firstSet.unite(tempFirst);
            }
            
            for (const Production &production : productions) {
                for (int i = 0; i < production.candidates.size(); ++i) {
                    // 创建新的项目，点在产生式开头
                    LR1Item newItem;
                    newItem.production = production;
                    newItem.dotPosition = 0;
                    newItem.lookaheadSymbols = firstSet;
                    newItem.productionIndex = i;
                    
                    // 如果新项目不在结果中，添加到结果和队列
                    if (!result.containsCoreItem(newItem)) {
                        result.items.insert(newItem);
                        queue.append(newItem);
                    }
                }
            }
        }
    }
    
    return result;
}

LR1ItemSet LR1DFA::gotoOperation(const LR1ItemSet &itemSet, const QString &symbol) const
{
    LR1ItemSet result;
    result.stateId = itemSet.stateId;
    
    // 获取所有点后是该符号的项目
    LR1ItemSet subset = itemSet.getSubsetBySymbol(symbol);
    
    // 对点后的符号执行moveDotForward操作
    for (const LR1Item &item : subset.items) {
        LR1Item newItem = item.moveDotForward();
        result.items.insert(newItem);
    }
    
    // 计算闭包
    return closure(result);
}

LR1ItemSet LR1DFA::initStartItemSet() const
{
    LR1ItemSet result;
    
    // 获取开始符号
    QString startSymbol = m_parser->getStartSymbol();
    
    // 获取开始符号的产生式
    QList<Production> startProductions = m_parser->getProductionsByNonTerminal(startSymbol);
    
    if (startProductions.isEmpty()) {
        return result;
    }
    
    // 创建开始项目，点在产生式开头，向前看符号为$ 
    LR1Item startItem;
    startItem.production = startProductions.first();
    startItem.dotPosition = 0;
    startItem.lookaheadSymbols.insert("$");
    startItem.productionIndex = 0;
    
    result.items.insert(startItem);
    return result;
}

int LR1DFA::findItemSetIndex(const LR1ItemSet &itemSet) const
{
    for (int i = 0; i < m_itemSets.size(); ++i) {
        if (m_itemSets[i] == itemSet) {
            return i;
        }
    }
    
    return -1;
}

bool LR1DFA::isAcceptItem(const LR1Item &item) const
{
    // 检查是否是扩展文法的接受项目
    QString startSymbol = m_parser->getStartSymbol();
    return item.production.nonTerminal == startSymbol && 
           item.isDotAtEnd() && 
           item.lookaheadSymbols.contains("$");
}

bool LR1DFA::generateTransitions()
{
    // 这个函数已经在generate()中实现了
    return true;
}

QList<Production> LR1DFA::getProductions() const
{
    if (!m_parser) {
        return QList<Production>();
    }
    
    return m_parser->getProductions();
}

QSet<QString> LR1DFA::getNonTerminals() const
{
    if (!m_parser) {
        return QSet<QString>();
    }
    
    return m_parser->getNonTerminals();
}

QSet<QString> LR1DFA::getTerminals() const
{
    if (!m_parser) {
        return QSet<QString>();
    }
    
    return m_parser->getTerminals();
}

QString LR1DFA::getStartSymbol() const
{
    if (!m_parser) {
        return QString();
    }
    
    return m_parser->getStartSymbol();
}

QSet<QString> LR1DFA::computeFirst(const QStringList &symbols, const QSet<QString> &lookahead) const
{
    if (symbols.isEmpty()) {
        return lookahead;
    }
    
    return m_firstFollow->getFirstOfString(symbols);
}
