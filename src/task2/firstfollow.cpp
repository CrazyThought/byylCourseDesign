/**
 * @file firstfollow.cpp
 * @brief First集合与Follow集合计算模块实现
 * @version 1.0
 * @author 编译原理课程设计小组
 * @date 2025-12-06
 * @copyright Copyright (c) 2025 编译原理课程设计
 * @details 实现了First集合与Follow集合的计算功能
 */

#include "task2/firstfollow.h"
#include "task2/bnfparser.h"
#include <QDebug>

FirstFollow::FirstFollow()
    : m_parser(nullptr)
    , m_hasEmptyProduction(false)
    , m_emptyString("#")
{
}

FirstFollow::~FirstFollow()
{
    clear();
}

void FirstFollow::setParser(BNFParser *parser)
{
    m_parser = parser;
}

bool FirstFollow::calculateFirst()
{
    if (!m_parser) {
        m_errorMessage = "BNF解析器未设置";
        return false;
    }
    
    clear();
    initFirst();
    
    // 迭代计算First集合，直到没有变化
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (const QString &nonTerminal : getNonTerminals()) {
            if (calculateSingleFirst(nonTerminal)) {
                changed = true;
            }
        }
    }
    
    return true;
}

bool FirstFollow::calculateFollow()
{
    if (!m_parser) {
        m_errorMessage = "BNF解析器未设置";
        return false;
    }
    
    // 确保First集合已经计算
    if (m_first.isEmpty()) {
        if (!calculateFirst()) {
            return false;
        }
    }
    
    initFollow();
    
    // 迭代计算Follow集合，直到没有变化
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (const QString &nonTerminal : getNonTerminals()) {
            if (calculateSingleFollow(nonTerminal)) {
                changed = true;
            }
        }
    }
    
    return true;
}

QSet<QString> FirstFollow::getFirst(const QString &nonTerminal) const
{
    return m_first.value(nonTerminal);
}

QSet<QString> FirstFollow::getFirstOfString(const QStringList &symbols) const
{
    QSet<QString> result;
    
    for (const QString &symbol : symbols) {
        if (isEmptyString(symbol)) {
            result.insert(m_emptyString);
        } else if (isTerminal(symbol)) {
            result.insert(symbol);
            return result;
        } else {
            QSet<QString> symbolFirst = getFirst(symbol);
            result.unite(removeEmpty(symbolFirst));
            
            if (!symbolFirst.contains(m_emptyString)) {
                return result;
            }
        }
    }
    
    // 所有符号都能推导出空串
    result.insert(m_emptyString);
    return result;
}

QSet<QString> FirstFollow::getFollow(const QString &nonTerminal) const
{
    return m_follow.value(nonTerminal);
}

QMap<QString, QSet<QString>> FirstFollow::getAllFirst() const
{
    return m_first;
}

QMap<QString, QSet<QString>> FirstFollow::getAllFollow() const
{
    return m_follow;
}

QString FirstFollow::getErrorMessage() const
{
    return m_errorMessage;
}

void FirstFollow::clear()
{
    m_first.clear();
    m_follow.clear();
    m_errorMessage.clear();
    m_hasEmptyProduction = false;
}

bool FirstFollow::isTerminal(const QString &symbol) const
{
    if (!m_parser) {
        return false;
    }
    
    return m_parser->isTerminal(symbol);
}

bool FirstFollow::isNonTerminal(const QString &symbol) const
{
    if (!m_parser) {
        return false;
    }
    
    return m_parser->isNonTerminal(symbol);
}

bool FirstFollow::isEmptyString(const QString &symbol) const
{
    return symbol == m_emptyString;
}

QString FirstFollow::printFirst() const
{
    QString result;
    for (const QString &nonTerminal : m_first.keys()) {
        result += nonTerminal + " -> { ";
        
        QSet<QString> firstSet = m_first[nonTerminal];
        QStringList firstList = firstSet.values();
        result += firstList.join(", ");
        
        result += " }\n";
    }
    
    return result;
}

QString FirstFollow::printFollow() const
{
    QString result;
    for (const QString &nonTerminal : m_follow.keys()) {
        result += nonTerminal + " -> { ";
        
        QSet<QString> followSet = m_follow[nonTerminal];
        QStringList followList = followSet.values();
        result += followList.join(", ");
        
        result += " }\n";
    }
    
    return result;
}

QSet<QString> FirstFollow::getNonTerminals() const
{
    if (!m_parser) {
        return QSet<QString>();
    }
    
    return m_parser->getNonTerminals();
}

QSet<QString> FirstFollow::getTerminals() const
{
    if (!m_parser) {
        return QSet<QString>();
    }
    
    return m_parser->getTerminals();
}

QString FirstFollow::getStartSymbol() const
{
    if (!m_parser) {
        return QString();
    }
    
    return m_parser->getStartSymbol();
}

QList<Production> FirstFollow::getProductions() const
{
    if (!m_parser) {
        return QList<Production>();
    }
    
    return m_parser->getProductions();
}

void FirstFollow::initFirst()
{
    // 初始化First集合
    for (const QString &nonTerminal : getNonTerminals()) {
        m_first[nonTerminal] = QSet<QString>();
    }
    
    // 检查是否有空产生式
    for (const Production &production : getProductions()) {
        for (const QStringList &candidate : production.candidates) {
            if (candidate.size() == 1 && isEmptyString(candidate[0])) {
                m_hasEmptyProduction = true;
                break;
            }
        }
    }
}

void FirstFollow::initFollow()
{
    // 初始化Follow集合
    for (const QString &nonTerminal : getNonTerminals()) {
        m_follow[nonTerminal] = QSet<QString>();
    }
    
    // 开始符号的Follow集合包含结束符
    m_follow[getStartSymbol()].insert("$");
}

bool FirstFollow::calculateSingleFirst(const QString &nonTerminal)
{
    bool changed = false;
    
    // 获取所有以该非终结符为左部的产生式
    QList<Production> productions = m_parser->getProductionsByNonTerminal(nonTerminal);
    
    for (const Production &production : productions) {
        for (const QStringList &candidate : production.candidates) {
            if (candidate.isEmpty()) {
                continue;
            }
            
            // 处理候选式中的第一个符号
            QString firstSymbol = candidate[0];
            
            if (isTerminal(firstSymbol)) {
                // 如果第一个符号是终结符，将其加入First集合
                int sizeBefore = m_first[nonTerminal].size();
                m_first[nonTerminal].insert(firstSymbol);
                if (m_first[nonTerminal].size() > sizeBefore) {
                    changed = true;
                }
            } else if (isNonTerminal(firstSymbol)) {
                // 如果第一个符号是非终结符，将其First集合加入当前非终结符的First集合
                QSet<QString> firstSet = getFirst(firstSymbol);
                firstSet.remove(m_emptyString);
                
                for (const QString &symbol : firstSet) {
                    int sizeBefore = m_first[nonTerminal].size();
                    m_first[nonTerminal].insert(symbol);
                    if (m_first[nonTerminal].size() > sizeBefore) {
                        changed = true;
                    }
                }
                
                // 如果第一个符号的First集合包含空串，继续处理下一个符号
                if (getFirst(firstSymbol).contains(m_emptyString)) {
                    // 递归处理后续符号
                    QStringList remainingSymbols = candidate.mid(1);
                    if (!remainingSymbols.isEmpty()) {
                        QSet<QString> remainingFirst = getFirstOfString(remainingSymbols);
                        for (const QString &symbol : remainingFirst) {
                            int sizeBefore = m_first[nonTerminal].size();
                            m_first[nonTerminal].insert(symbol);
                            if (m_first[nonTerminal].size() > sizeBefore) {
                                changed = true;
                            }
                        }
                    } else {
                        // 整个候选式都能推导出空串
                        int sizeBefore = m_first[nonTerminal].size();
                        m_first[nonTerminal].insert(m_emptyString);
                        if (m_first[nonTerminal].size() > sizeBefore) {
                            changed = true;
                        }
                    }
                }
            } else if (isEmptyString(firstSymbol)) {
                // 空串产生式
                int sizeBefore = m_first[nonTerminal].size();
                m_first[nonTerminal].insert(m_emptyString);
                if (m_first[nonTerminal].size() > sizeBefore) {
                    changed = true;
                }
            }
        }
    }
    
    return changed;
}

bool FirstFollow::calculateSingleFollow(const QString &nonTerminal)
{
    bool changed = false;
    
    // 遍历所有产生式
    for (const Production &production : getProductions()) {
        for (const QStringList &candidate : production.candidates) {
            // 在候选式中查找当前非终结符
            int pos = candidate.indexOf(nonTerminal);
            if (pos == -1) {
                continue;
            }
            
            // 处理非终结符后面的符号串
            QStringList remainingSymbols = candidate.mid(pos + 1);
            
            if (!remainingSymbols.isEmpty()) {
                // 获取后续符号串的First集合
                QSet<QString> firstOfRemaining = getFirstOfString(remainingSymbols);
                
                // 将First集合中除空串外的符号加入Follow集合
                for (const QString &symbol : firstOfRemaining) {
                    if (symbol != m_emptyString) {
                        int sizeBefore = m_follow[nonTerminal].size();
                        m_follow[nonTerminal].insert(symbol);
                        if (m_follow[nonTerminal].size() > sizeBefore) {
                            changed = true;
                        }
                    }
                }
                
                // 如果后续符号串的First集合包含空串，将左部非终结符的Follow集合加入当前非终结符的Follow集合
                if (firstOfRemaining.contains(m_emptyString)) {
                    QSet<QString> leftFollow = m_follow[production.nonTerminal];
                    for (const QString &symbol : leftFollow) {
                        int sizeBefore = m_follow[nonTerminal].size();
                        m_follow[nonTerminal].insert(symbol);
                        if (m_follow[nonTerminal].size() > sizeBefore) {
                            changed = true;
                        }
                    }
                }
            } else {
                // 非终结符在候选式末尾，将左部非终结符的Follow集合加入当前非终结符的Follow集合
                QSet<QString> leftFollow = m_follow[production.nonTerminal];
                for (const QString &symbol : leftFollow) {
                    int sizeBefore = m_follow[nonTerminal].size();
                    m_follow[nonTerminal].insert(symbol);
                    if (m_follow[nonTerminal].size() > sizeBefore) {
                        changed = true;
                    }
                }
            }
        }
    }
    
    return changed;
}

bool FirstFollow::updateFollow(const QString &nonTerminal, const QSet<QString> &followSet)
{
    bool changed = false;
    
    for (const QString &symbol : followSet) {
        int sizeBefore = m_follow[nonTerminal].size();
        m_follow[nonTerminal].insert(symbol);
        if (m_follow[nonTerminal].size() > sizeBefore) {
            changed = true;
        }
    }
    
    return changed;
}

bool FirstFollow::updateFirst(const QString &nonTerminal, const QSet<QString> &firstSet)
{
    bool changed = false;
    
    for (const QString &symbol : firstSet) {
        int sizeBefore = m_first[nonTerminal].size();
        m_first[nonTerminal].insert(symbol);
        if (m_first[nonTerminal].size() > sizeBefore) {
            changed = true;
        }
    }
    
    return changed;
}

bool FirstFollow::containsEmpty(const QStringList &symbols) const
{
    for (const QString &symbol : symbols) {
        if (isTerminal(symbol)) {
            return false;
        }
        
        if (!getFirst(symbol).contains(m_emptyString)) {
            return false;
        }
    }
    
    return true;
}

QSet<QString> FirstFollow::mergeSets(const QSet<QString> &set1, const QSet<QString> &set2) const
{
    QSet<QString> result = set1;
    result.unite(set2);
    return result;
}

QSet<QString> FirstFollow::removeEmpty(const QSet<QString> &set) const
{
    QSet<QString> result = set;
    result.remove(m_emptyString);
    return result;
}
