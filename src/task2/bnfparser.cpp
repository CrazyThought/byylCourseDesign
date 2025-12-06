/**
 * @file bnfparser.cpp
 * @brief BNF文法处理模块实现
 * @version 1.0
 * @author 编译原理课程设计小组
 * @date 2025-12-06
 * @copyright Copyright (c) 2025 编译原理课程设计
 * @details 实现了BNF文法的解析、验证和管理功能
 */

#include "task2/bnfparser.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

BNFParser::BNFParser()
{}

BNFParser::~BNFParser()
{
    clear();
}

bool BNFParser::parse(const QString &text)
{
    clear();
    m_originalText = text;
    
    QStringList lines = text.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);
    
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();
        
        // 跳过空行和注释行
        if (line.isEmpty() || line.startsWith("//")) {
            continue;
        }
        
        Production production;
        if (!parseLine(line, production)) {
            m_errorMessage = QString("第%1行解析失败: %2").arg(i + 1).arg(line);
            return false;
        }
        
        m_productions.append(production);
    }
    
    // 提取非终结符和终结符
    extractSymbols();
    
    // 验证文法格式
    if (!validateGrammar()) {
        return false;
    }
    
    return true;
}

bool BNFParser::parseLine(const QString &line, Production &production)
{
    // 查找箭头符号 "->"
    int arrowPos = line.indexOf("->");
    if (arrowPos == -1) {
        m_errorMessage = QString("未找到箭头符号 '->': %1").arg(line);
        return false;
    }
    
    // 解析非终结符
    QString nonTerminal = line.left(arrowPos).trimmed();
    if (nonTerminal.isEmpty()) {
        m_errorMessage = QString("非终结符不能为空: %1").arg(line);
        return false;
    }
    
    production.nonTerminal = nonTerminal;
    
    // 解析候选式
    QString candidatesStr = line.mid(arrowPos + 2).trimmed();
    if (candidatesStr.isEmpty()) {
        m_errorMessage = QString("候选式不能为空: %1").arg(line);
        return false;
    }
    
    // 拆分候选式（使用 | 分隔）
    QStringList candidateStrs = candidatesStr.split("|", Qt::SkipEmptyParts);
    if (candidateStrs.isEmpty()) {
        m_errorMessage = QString("候选式列表为空: %1").arg(line);
        return false;
    }
    
    for (const QString &candidateStr : candidateStrs) {
        QStringList candidate;
        if (!splitCandidate(candidateStr.trimmed(), candidate)) {
            return false;
        }
        production.candidates.append(candidate);
    }
    
    return true;
}

bool BNFParser::splitCandidate(const QString &candidateStr, QStringList &candidate)
{
    // 简单的空格分隔
    candidate = candidateStr.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    
    if (candidate.isEmpty()) {
        // 空串用 # 表示
        candidate.append("#");
    }
    
    return true;
}

void BNFParser::extractSymbols()
{
    // 收集所有非终结符
    for (const Production &production : m_productions) {
        m_nonTerminals.insert(production.nonTerminal);
    }
    
    // 收集所有终结符
    for (const Production &production : m_productions) {
        for (const QStringList &candidate : production.candidates) {
            for (const QString &symbol : candidate) {
                if (!m_nonTerminals.contains(symbol) && symbol != "#") {
                    m_terminals.insert(symbol);
                }
            }
        }
    }
    
    // 设置文法开始符号（第一个产生式的非终结符）
    if (!m_productions.isEmpty()) {
        m_startSymbol = m_productions.first().nonTerminal;
    }
}

bool BNFParser::validateGrammar() const
{
    if (m_productions.isEmpty()) {
        m_errorMessage = QString("文法中没有产生式");
        return false;
    }
    
    if (m_nonTerminals.isEmpty()) {
        m_errorMessage = QString("文法中没有非终结符");
        return false;
    }
    
    if (m_startSymbol.isEmpty()) {
        m_errorMessage = QString("无法确定文法开始符号");
        return false;
    }
    
    return true;
}

QList<Production> BNFParser::getProductions() const
{
    return m_productions;
}

QSet<QString> BNFParser::getNonTerminals() const
{
    return m_nonTerminals;
}

QSet<QString> BNFParser::getTerminals() const
{
    return m_terminals;
}

QString BNFParser::getStartSymbol() const
{
    return m_startSymbol;
}

QString BNFParser::getErrorMessage() const
{
    return m_errorMessage;
}

bool BNFParser::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_errorMessage = QString("无法打开文件: %1").arg(file.errorString());
        return false;
    }
    
    QTextStream in(&file);
    QString text = in.readAll();
    file.close();
    
    return parse(text);
}

bool BNFParser::saveToFile(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_errorMessage = QString("无法打开文件: %1").arg(file.errorString());
        return false;
    }
    
    QTextStream out(&file);
    out << m_originalText;
    file.close();
    
    return true;
}

void BNFParser::clear()
{
    m_productions.clear();
    m_nonTerminals.clear();
    m_terminals.clear();
    m_startSymbol.clear();
    m_errorMessage.clear();
    m_originalText.clear();

}

QString BNFParser::printGrammar() const
{
    QString result;
    QTextStream out(&result);
    
    out << "文法开始符号: " << m_startSymbol << "\n";
    out << "非终结符: " << m_nonTerminals.values().join(", ") << "\n";
    out << "终结符: " << m_terminals.values().join(", ") << "\n\n";
    out << "产生式:\n";
    
    for (const Production &production : m_productions) {
        out << production.nonTerminal << " -> ";
        for (int i = 0; i < production.candidates.size(); ++i) {
            if (i > 0) {
                out << " | ";
            }
            out << production.candidates[i].join(" ");
        }
        out << "\n";
    }
    
    return result;
}

QList<Production> BNFParser::getProductionsByNonTerminal(const QString &nonTerminal) const
{
    QList<Production> result;
    for (const Production &production : m_productions) {
        if (production.nonTerminal == nonTerminal) {
            result.append(production);
        }
    }
    return result;
}

bool BNFParser::isNonTerminal(const QString &symbol) const
{
    return m_nonTerminals.contains(symbol);
}

bool BNFParser::isTerminal(const QString &symbol) const
{
    return m_terminals.contains(symbol) || symbol == "#";
}
