/**
 * @file slr1checker.cpp
 * @brief SLR(1)文法判断模块实现
 * @version 1.0
 * @author 编译原理课程设计小组
 * @date 2025-12-06
 * @copyright Copyright (c) 2025 编译原理课程设计
 * @details 实现了SLR(1)文法的判断功能
 */

#include "task2/slr1checker.h"
#include "task2/lr0dfa.h"
#include "task2/firstfollow.h"
#include <QDebug>

// ConflictInfo结构体实现
QString ConflictInfo::toString() const
{
    QString result;
    
    switch (type) {
    case NO_CONFLICT:
        result += "无冲突";
        break;
    case SHIFT_REDUCE_CONFLICT:
        result += "移进-归约冲突";
        break;
    case REDUCE_REDUCE_CONFLICT:
        result += "归约-归约冲突";
        break;
    }
    
    result += "，状态: " + QString::number(stateId);
    result += "，符号: " + symbol;
    
    if (type == SHIFT_REDUCE_CONFLICT) {
        result += "，移进状态: " + QString::number(shiftState);
        result += "，归约产生式: ";
        for (int i = 0; i < reduceProductions.size(); ++i) {
            if (i > 0) {
                result += ", ";
            }
            result += QString::number(reduceProductions[i]);
        }
    } else if (type == REDUCE_REDUCE_CONFLICT) {
        result += "，归约产生式: ";
        for (int i = 0; i < reduceProductions.size(); ++i) {
            if (i > 0) {
                result += ", ";
            }
            result += QString::number(reduceProductions[i]);
        }
    }
    
    return result;
}

// SLR1Checker类实现
SLR1Checker::SLR1Checker()
    : m_lr0dfa(nullptr)
    , m_firstFollow(nullptr)
    , m_isSLR1(true)
{
}

SLR1Checker::~SLR1Checker()
{
    clear();
}

void SLR1Checker::setLR0DFA(LR0DFA *lr0dfa)
{
    m_lr0dfa = lr0dfa;
}

void SLR1Checker::setFirstFollow(FirstFollow *firstFollow)
{
    m_firstFollow = firstFollow;
}

bool SLR1Checker::checkSLR1()
{
    if (!m_lr0dfa || !m_firstFollow) {
        m_errorMessage = "LR0DFA或FirstFollow未设置";
        return false;
    }
    
    clear();
    
    // 获取所有项目集
    QList<LR0ItemSet> itemSets = m_lr0dfa->getItemSets();
    
    // 检查每个项目集
    for (int i = 0; i < itemSets.size(); ++i) {
        LR0ItemSet itemSet = itemSets[i];
        QList<ConflictInfo> conflicts = checkItemSet(itemSet, i);
        
        if (!conflicts.isEmpty()) {
            m_conflicts.append(conflicts);
            m_isSLR1 = false;
        }
    }
    
    // 生成SLR(1)分析表
    if (m_isSLR1) {
        generateSLR1Table();
    }
    
    return m_isSLR1;
}

QList<ConflictInfo> SLR1Checker::getConflicts() const
{
    return m_conflicts;
}

QString SLR1Checker::getErrorMessage() const
{
    return m_errorMessage;
}

void SLR1Checker::clear()
{
    m_conflicts.clear();
    m_errorMessage.clear();
    m_isSLR1 = true;
    m_slr1Table.clear();
}

QString SLR1Checker::printConflicts() const
{
    if (m_conflicts.isEmpty()) {
        return "无冲突，该文法为SLR(1)文法";
    }
    
    QString result = "存在" + QString::number(m_conflicts.size()) + "个冲突，该文法不是SLR(1)文法:\n";
    
    for (const ConflictInfo &conflict : m_conflicts) {
        result += "  " + conflict.toString() + "\n";
    }
    
    return result;
}

int SLR1Checker::getConflictCount() const
{
    return m_conflicts.size();
}

bool SLR1Checker::hasShiftReduceConflict() const
{
    for (const ConflictInfo &conflict : m_conflicts) {
        if (conflict.type == SHIFT_REDUCE_CONFLICT) {
            return true;
        }
    }
    
    return false;
}

bool SLR1Checker::hasReduceReduceConflict() const
{
    for (const ConflictInfo &conflict : m_conflicts) {
        if (conflict.type == REDUCE_REDUCE_CONFLICT) {
            return true;
        }
    }
    
    return false;
}

QMap<int, QMap<QString, QString>> SLR1Checker::getSLR1Table() const
{
    return m_slr1Table;
}

QList<ConflictInfo> SLR1Checker::checkItemSet(const LR0ItemSet &itemSet, int stateId)
{
    QList<ConflictInfo> conflicts;
    
    // 获取所有归约项目
    QList<LR0Item> reduceItems = getReduceItems(itemSet);
    
    if (reduceItems.isEmpty()) {
        return conflicts;
    }
    
    // 获取所有移进项目
    QList<LR0Item> shiftItems = getShiftItems(itemSet);
    
    // 获取所有移进符号
    QSet<QString> shiftSymbols;
    for (const LR0Item &item : shiftItems) {
        shiftSymbols.insert(item.getSymbolAfterDot());
    }
    
    // 获取所有归约产生式
    QList<int> reduceProductions;
    for (const LR0Item &item : reduceItems) {
        reduceProductions.append(item.productionIndex);
    }
    
    // 检查每个移进符号是否与归约项目冲突
    for (const QString &symbol : shiftSymbols) {
        ConflictInfo conflict = checkShiftReduceConflict(itemSet, stateId, symbol, reduceProductions);
        if (conflict.type != NO_CONFLICT) {
            conflicts.append(conflict);
        }
    }
    
    // 检查归约-归约冲突
    if (reduceItems.size() > 1) {
        // 对于每个可能的符号，检查是否有多个归约项目
        for (const LR0Item &item : reduceItems) {
            // 获取该归约项目的Follow集合
            QSet<QString> followSet = getFollowSetForReduceItem(item);
            
            // 检查其他归约项目是否与该项目在同一符号上有冲突
            for (const LR0Item &otherItem : reduceItems) {
                if (&item == &otherItem) {
                    continue;
                }
                
                QSet<QString> otherFollowSet = getFollowSetForReduceItem(otherItem);
                
                // 检查Follow集合是否有交集
                QSet<QString> intersection = followSet.intersect(otherFollowSet);
                if (!intersection.isEmpty()) {
                    // 对于交集中的每个符号，添加归约-归约冲突
                    for (const QString &symbol : intersection) {
                        ConflictInfo conflict;
                        conflict.type = REDUCE_REDUCE_CONFLICT;
                        conflict.stateId = stateId;
                        conflict.symbol = symbol;
                        conflict.reduceProductions.append(item.productionIndex);
                        conflict.reduceProductions.append(otherItem.productionIndex);
                        conflicts.append(conflict);
                    }
                }
            }
        }
    }
    
    return conflicts;
}

ConflictInfo SLR1Checker::checkShiftReduceConflict(const LR0ItemSet &itemSet, int stateId, const QString &symbol, const QList<int> &reduceProductions)
{
    ConflictInfo conflict;
    conflict.type = NO_CONFLICT;
    conflict.stateId = stateId;
    conflict.symbol = symbol;
    conflict.reduceProductions = reduceProductions;
    
    // 检查该符号是否在任何归约项目的Follow集合中
    for (const LR0Item &item : getReduceItems(itemSet)) {
        QSet<QString> followSet = getFollowSetForReduceItem(item);
        if (isSymbolInFollowSet(symbol, followSet)) {
            // 存在移进-归约冲突
            conflict.type = SHIFT_REDUCE_CONFLICT;
            conflict.shiftState = m_lr0dfa->getNextState(stateId, symbol);
            break;
        }
    }
    
    return conflict;
}

ConflictInfo SLR1Checker::checkReduceReduceConflict(const LR0ItemSet &itemSet, int stateId, const QString &symbol, const QList<int> &reduceProductions)
{
    ConflictInfo conflict;
    conflict.type = NO_CONFLICT;
    conflict.stateId = stateId;
    conflict.symbol = symbol;
    conflict.reduceProductions = reduceProductions;
    
    // 如果归约产生式数量大于1，则存在归约-归约冲突
    if (reduceProductions.size() > 1) {
        conflict.type = REDUCE_REDUCE_CONFLICT;
    }
    
    return conflict;
}

QList<LR0Item> SLR1Checker::getReduceItems(const LR0ItemSet &itemSet) const
{
    QList<LR0Item> reduceItems;
    
    for (const LR0Item &item : itemSet.items) {
        if (item.isDotAtEnd()) {
            reduceItems.append(item);
        }
    }
    
    return reduceItems;
}

QList<LR0Item> SLR1Checker::getShiftItems(const LR0ItemSet &itemSet) const
{
    QList<LR0Item> shiftItems;
    
    for (const LR0Item &item : itemSet.items) {
        if (!item.isDotAtEnd() && !item.getSymbolAfterDot().isEmpty()) {
            shiftItems.append(item);
        }
    }
    
    return shiftItems;
}

QSet<QString> SLR1Checker::getFollowSetForReduceItem(const LR0Item &item) const
{
    return m_firstFollow->getFollow(item.production.nonTerminal);
}

bool SLR1Checker::isSymbolInFollowSet(const QString &symbol, const QSet<QString> &followSet) const
{
    return followSet.contains(symbol);
}

bool SLR1Checker::generateSLR1Table()
{
    if (!m_lr0dfa || !m_firstFollow) {
        return false;
    }
    
    // 获取所有项目集
    QList<LR0ItemSet> itemSets = m_lr0dfa->getItemSets();
    
    // 处理每个状态
    for (int stateId = 0; stateId < itemSets.size(); ++stateId) {
        LR0ItemSet itemSet = itemSets[stateId];
        
        // 获取所有移进项目
        QList<LR0Item> shiftItems = getShiftItems(itemSet);
        
        // 添加移进行动
        for (const LR0Item &item : shiftItems) {
            QString symbol = item.getSymbolAfterDot();
            int nextState = m_lr0dfa->getNextState(stateId, symbol);
            addShiftAction(stateId, symbol, nextState);
        }
        
        // 获取所有归约项目
        QList<LR0Item> reduceItems = getReduceItems(itemSet);
        
        // 添加归约行动
        for (const LR0Item &item : reduceItems) {
            QSet<QString> followSet = getFollowSetForReduceItem(item);
            for (const QString &symbol : followSet) {
                addReduceAction(stateId, symbol, item.productionIndex);
            }
        }
        
        // 检查是否是接受状态
        if (m_lr0dfa->isAcceptState(stateId)) {
            addAcceptAction(stateId);
        }
        
        // 添加GOTO行动
        // 获取所有非终结符
        QSet<QString> nonTerminals = m_firstFollow->getNonTerminals();
        for (const QString &nonTerminal : nonTerminals) {
            int nextState = m_lr0dfa->getNextState(stateId, nonTerminal);
            if (nextState != -1) {
                addGotoAction(stateId, nonTerminal, nextState);
            }
        }
    }
    
    return true;
}

void SLR1Checker::addShiftAction(int stateId, const QString &symbol, int nextState)
{
    m_slr1Table[stateId][symbol] = QString("s%1").arg(nextState);
}

void SLR1Checker::addReduceAction(int stateId, const QString &symbol, int productionIndex)
{
    m_slr1Table[stateId][symbol] = QString("r%1").arg(productionIndex);
}

void SLR1Checker::addAcceptAction(int stateId)
{
    m_slr1Table[stateId]["$"] = "acc";
}

void SLR1Checker::addGotoAction(int stateId, const QString &symbol, int nextState)
{
    m_slr1Table[stateId][symbol] = QString::number(nextState);
}

QSet<QString> SLR1Checker::getShiftSymbols(const LR0ItemSet &itemSet) const
{
    QSet<QString> symbols;
    
    for (const LR0Item &item : getShiftItems(itemSet)) {
        symbols.insert(item.getSymbolAfterDot());
    }
    
    return symbols;
}

QSet<QString> SLR1Checker::getReduceSymbols(const LR0ItemSet &itemSet) const
{
    QSet<QString> symbols;
    
    for (const LR0Item &item : getReduceItems(itemSet)) {
        QSet<QString> followSet = getFollowSetForReduceItem(item);
        symbols.unite(followSet);
    }
    
    return symbols;
}
