/*
 * @file GrammarParser.cpp
 * @id GrammarParser-cpp
 * @brief 实现文法解析器，从文件或字符串中解析生成文法对象
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task2/Grammar.h"
#include "task2/configconstants.h"
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QIODevice>

// 非终结符/终结符判断由 addSymbols 归类为准；此处不再使用命名约定
// 解析阶段的终结符判断不使用命名约定

/**
 * @brief 去除字符串首尾空白字符
 * 
 * @param s 输入字符串
 * @return QString 去除首尾空白后的字符串
 */
static QString trim(const QString& s)
{
    return s.trimmed();
}

/**
 * @brief 分割产生式右部为符号列表
 * 
 * 将产生式右部字符串分割为单个符号的列表，支持单词、单字符操作符和多字符操作符
 * 
 * @param rhs 产生式右部字符串
 * @return QVector<QString> 分割后的符号列表
 */
static QVector<QString> splitRhs(const QString& rhs)
{
    QVector<QString> v;
    QString s = rhs;
    int i = 0;
    
    // 判断是否为单词字符
    auto isWordChar = [](QChar c) { return c.isLetterOrNumber() || c == '_' || c == '-'; };
    
    // 判断是否为单字符操作符
    auto isSingleOp = [](QChar c)
    {
        static QSet<QChar> ops;
        if (ops.isEmpty())
        {
            const QChar arr[] = {'(', ')', '{', '}', '[', ']', ';', ',', '<', '>', '=',
                                 '+', '-', '*', '/', '%', '^'};
            for (QChar ch : arr) ops.insert(ch);
        }
        return ops.contains(c);
    };
    
    // 匹配多字符操作符
    auto matchMultiOp = [&](const QString& s, int i) -> QString
    {
        static const QVector<QString> mops = {"<=", ">=", "==", "!=", ":=", "++", "--"};
        for (const auto& op : mops)
        {
            int L = op.size();
            if (L > 0 && i + L <= s.size() && s.mid(i, L) == op)
                return op;
        }
        return QString();
    };
    
    // 主分割循环
    while (i < s.size())
    {
        QChar c = s[i];
        
        // 跳过空白字符
        if (c.isSpace())
        {
            i++;
            continue;
        }
        
        // 尝试匹配多字符操作符
        QString mop = matchMultiOp(s, i);
        if (!mop.isEmpty())
        {
            v.push_back(mop);
            i += mop.size();
            continue;
        }
        
        // 尝试匹配单字符操作符
        if (isSingleOp(c))
        {
            v.push_back(QString(c));
            i++;
            continue;
        }
        
        // 匹配单词
        QString w;
        while (i < s.size() && isWordChar(s[i]))
        {
            w += s[i];
            i++;
        }
        if (!w.isEmpty())
            v.push_back(w);
        else
            i++;
    }
    
    return v;
}

/**
 * @brief 检测直接左递归
 * 
 * 检查文法中是否存在直接左递归
 * 
 * @param g 文法对象
 * @param who 输出参数，存储检测到左递归的非终结符
 * @return bool 存在直接左递归返回true，否则返回false
 */
static bool detectDirectLeftRecursion(const Grammar& g, QString& who)
{
    Q_UNUSED(g);
    Q_UNUSED(who);
    // 暂未实现直接左递归检测
    return false;
}

/**
 * @brief 添加符号到文法的终结符和非终结符集合
 * 
 * 根据产生式规则，将所有符号归类为终结符或非终结符
 * 
 * @param g 文法对象，将被修改
 */
static void addSymbols(Grammar& g)
{
    // 收集所有产生式左部的符号（非终结符）
    QSet<QString> lhs;
    for (auto it = g.productions.begin(); it != g.productions.end(); ++it) {
        lhs.insert(it.key());
    }
    
    // 遍历所有产生式，归类符号
    for (auto it = g.productions.begin(); it != g.productions.end(); ++it)
    {
        // 产生式左部一定是非终结符
        g.nonterminals.insert(it.key());
        
        // 遍历每个产生式的右部
        for (const auto& p : it.value())
        {
            for (const auto& s : p.right)
            {
                // 跳过空串符号
                if (s == ConfigConstants::epsilonSymbol())
                    continue;
                
                // 如果符号出现在产生式左部，则为非终结符，否则为终结符
                if (lhs.contains(s))
                    g.nonterminals.insert(s);
                else
                    g.terminals.insert(s);
            }
        }
    }
}

/**
 * @brief 解析单行文法规则
 * 
 * 解析一行文法规则，添加到文法对象中
 * 
 * @param line 待解析的行
 * @param lineNo 行号，用于错误报告
 * @param g 文法对象，将被修改
 * @param err 错误信息输出
 * @return bool 解析成功返回true，失败返回false
 */
static bool parseLine(const QString& line, int lineNo, Grammar& g, QString& err)
{
    QString t = line;
    
    // 跳过空行和注释行
    if (t.trimmed().isEmpty())
        return true;
    if (t.trimmed().startsWith("//"))
        return true;
    
    // 检查是否包含产生式箭头
    if (t.indexOf("->") < 0)
    {
        err = QString::number(lineNo);
        return false;
    }
    
    // 分割产生式左部和右部
    auto parts = t.split("->");
    if (parts.size() != 2)
    {
        err = QString::number(lineNo);
        return false;
    }
    
    QString left = trim(parts[0]);  // 产生式左部
    QString rhs = trim(parts[1]);    // 产生式右部
    auto alts = rhs.split('|');      // 分割多个可选分支
    
    // 设置起始符号（第一个产生式的左部）
    if (g.startSymbol.isEmpty() && !left.isEmpty())
        g.startSymbol = left;
    
    // 处理每个可选分支
    for (auto a : alts)
    {
        Production p;
        p.left = left;                  // 产生式左部
        p.right = splitRhs(trim(a));    // 分割右部为符号列表
        p.line = lineNo;                // 记录行号
        g.productions[left].push_back(p);  // 添加到产生式集合
    }
    
    return true;
}

/**
 * @brief 解析文本内容生成文法
 * 
 * 从文本内容中解析文法规则，生成文法对象
 * 
 * @param text 包含文法规则的文本
 * @param error 错误信息输出
 * @return Grammar 生成的文法对象，失败时返回无效文法
 */
static Grammar parseText(const QString& text, QString& error)
{
    Grammar g;
    auto lines = text.split('\n');  // 按行分割文本
    
    // 逐行解析
    for (int i = 0; i < lines.size(); ++i)
    {
        QString l = lines[i];
        QString s = l.trimmed();
        
        // 处理标题行（全#字符）
        if (s.startsWith('#'))
        {
            bool allHash = true;
            for (int k = 0; k < s.size(); ++k)
                if (s[k] != '#')
                {
                    allHash = false;
                    break;
                }
            if (allHash)
                continue;
        }
        
        // 解析当前行
        QString err;
        if (!parseLine(l, i + 1, g, err))
        {
            error = err;
            return Grammar();  // 解析失败，返回无效文法
        }
    }
    
    // 添加符号到终结符和非终结符集合
    addSymbols(g);
    
    // 检测直接左递归
    QString who;
    if (detectDirectLeftRecursion(g, who))
    {
        error = who;
        return Grammar();  // 存在左递归，返回无效文法
    }
    
    return g;  // 解析成功，返回文法对象
}

namespace GrammarParser
{
    Grammar parseFile(const QString& path, QString& error);
    Grammar parseString(const QString& text, QString& error);
}  // namespace GrammarParser

/**
 * @brief 从字符串中解析文法
 * 
 * 解析给定的字符串，生成Grammar对象
 * 
 * @param text 包含文法规则的字符串
 * @param error 错误信息输出，解析失败时会存储详细的错误描述
 * @return Grammar 解析生成的文法对象，解析失败时返回无效的文法
 */
Grammar GrammarParser::parseString(const QString& text, QString& error)
{
    return parseText(text, error);
}

/**
 * @brief 从文件中解析文法
 * 
 * 读取指定路径的文件，解析其中的文法规则，生成Grammar对象
 * 
 * @param path 文法文件的路径
 * @param error 错误信息输出，解析失败时会存储详细的错误描述
 * @return Grammar 解析生成的文法对象，解析失败时返回无效的文法
 */
Grammar GrammarParser::parseFile(const QString& path, QString& error)
{
    QFile f(path);
    
    // 打开文件
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        error = "open";
        return Grammar();  // 打开失败，返回无效文法
    }
    
    // 读取文件内容
    QTextStream in(&f);
    auto content = in.readAll();
    f.close();
    
    // 解析文本内容
    return parseText(content, error);
}
