/*
 * @file LR0.cpp
 * @id LR0-cpp
 * @brief 实现LR(0)分析表生成算法，包括LR(0)项目集闭包、GOTO操作和LR(0)图构建
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task2/LR0.h"
#include "task2/configconstants.h"

/**
 * @brief 检查项目是否存在于项目集中
 * 
 * @param set 项目集
 * @param it 要检查的项目
 * @return bool 项目存在返回true，否则返回false
 */
static bool containsItem(const QVector<LR0Item>& set, const LR0Item& it)
{
    for (const auto& x : set)
        if (x == it)
            return true;
    return false;
}

/**
 * @brief 计算LR(0)项目集的闭包
 * 
 * 对于给定的LR(0)项目集I，计算其闭包closure(I)
 * 
 * @param I 初始项目集
 * @param g 文法对象
 * @return QVector<LR0Item> 闭包后的项目集
 */
static QVector<LR0Item> closure(const QVector<LR0Item>& I, const Grammar& g)
{
    QVector<LR0Item> res = I;     // 结果项目集，初始为I
    bool changed = true;           // 标记是否有变化
    
    // 迭代计算闭包，直到不再变化
    while (changed)
    {
        changed = false;
        
        // 遍历当前项目集中的每个项目
        for (const auto& it : res)
        {
            // 检查项目中的点是否在产生式右部末尾之前
            if (it.dot < it.right.size())
            {
                const QString& B = it.right[it.dot];  // 获取点后的符号
                
                // 如果点后的符号是一个非终结符
                if (g.nonterminals.contains(B))
                {
                    // 获取该非终结符的所有产生式
                    const auto prods = g.productions.value(B);
                    
                    // 为每个产生式添加新的项目
                    for (const auto& p : prods)
                    {
                        // 创建新的项目：点在产生式右部开头
                        LR0Item ni{B, p.right, 0};
                        
                        // 如果项目不在结果集中，则添加
                        if (!containsItem(res, ni))
                        {
                            res.push_back(ni);
                            changed = true;
                        }
                    }
                }
            }
        }
    }
    
    return res;
}

/**
 * @brief 计算LR(0)项目集的GOTO操作
 * 
 * 对于给定的项目集I和符号X，计算goto(I, X)
 * 
 * @param I 初始项目集
 * @param X 符号
 * @param g 文法对象
 * @return QVector<LR0Item> goto(I, X)的结果项目集
 */
static QVector<LR0Item> gotoSet(const QVector<LR0Item>& I, const QString& X, const Grammar& g)
{
    QVector<LR0Item> moved;
    
    // 遍历项目集I中的每个项目
    for (const auto& it : I)
    {
        // 检查点是否在产生式右部末尾之前，且点后的符号为X
        if (it.dot < it.right.size() && it.right[it.dot] == X)
        {
            // 将点向右移动一位
            LR0Item adv = it;
            adv.dot++;
            moved.push_back(adv);
        }
    }
    
    // 计算移动后的项目集的闭包
    return closure(moved, g);
}

/**
 * @brief 检查两个项目集是否相等
 * 
 * @param a 第一个项目集
 * @param b 第二个项目集
 * @return bool 两个项目集相等返回true，否则返回false
 */
static bool equalSet(const QVector<LR0Item>& a, const QVector<LR0Item>& b)
{
    // 大小不同，直接返回false
    if (a.size() != b.size())
        return false;
    
    // 检查a中的每个项目是否都在b中
    for (const auto& x : a)
    {
        bool found = false;
        for (const auto& y : b)
            if (x == y)
            {
                found = true;
                break;
            }
        if (!found)
            return false;
    }
    
    return true;
}

/**
 * @brief 构建LR(0)项目集规范族和状态转换图
 * 
 * @param g 文法对象
 * @return LR0Graph LR(0)图结构，包含所有状态和边
 */
LR0Graph LR0Builder::build(const Grammar& g)
{
    // 构建增广文法
    Grammar aug = g;
    QString Sdash = g.startSymbol + ConfigConstants::augSuffix();  // 增广文法的起始符号
    
    // 如果增广文法的起始符号不存在，则添加
    if (!aug.productions.contains(Sdash))
    {
        Production p;
        p.left = Sdash;                       // 增广文法起始符号
        p.right = QVector<QString>{g.startSymbol};  // 产生式右部为原起始符号
        p.line = -1;                          // 行号标记为-1
        aug.productions[Sdash].push_back(p);  // 添加到产生式集合
        aug.nonterminals.insert(Sdash);       // 添加到非终结符集合
    }
    
    LR0Graph gr;  // LR(0)图结构
    
    // 构建初始项目集I0
    QVector<LR0Item> I0;
    I0.push_back(LR0Item{Sdash, QVector<QString>{g.startSymbol}, 0});  // 初始项目
    I0 = closure(I0, aug);  // 计算闭包
    gr.states.push_back(I0);  // 添加到状态列表
    
    QMap<int, QMap<QString, int>> edges;  // 边集合
    bool added = true;  // 标记是否有新状态添加
    
    // 迭代构建所有状态和边
    while (added)
    {
        added = false;
        int n = gr.states.size();
        
        // 遍历当前所有状态
        for (int i = 0; i < n; ++i)
        {
            QSet<QString> symbols;  // 收集当前状态中所有点后的符号
            
            // 遍历状态中的每个项目，收集点后的符号
            for (const auto& it : gr.states[i])
            {
                if (it.dot < it.right.size())
                    symbols.insert(it.right[it.dot]);
            }
            
            // 对每个符号计算goto操作
            for (const auto& X : symbols)
            {
                // 计算goto(I, X)
                auto J = gotoSet(gr.states[i], X, aug);
                if (J.isEmpty())
                    continue;
                
                // 检查是否已经存在该状态
                int existing = -1;
                for (int k = 0; k < gr.states.size(); ++k)
                    if (equalSet(gr.states[k], J))
                    {
                        existing = k;
                        break;
                    }
                
                // 如果状态不存在，则添加
                if (existing < 0)
                {
                    gr.states.push_back(J);
                    existing = gr.states.size() - 1;
                    added = true;
                }
                
                // 记录边
                gr.edges[i][X] = existing;
                edges[i][X] = existing;
            }
        }
    }
    
    gr.edges = edges;
    return gr;
}

/**
 * @brief 转义字符串中的特殊字符
 * 
 * @param s 输入字符串
 * @return QString 转义后的字符串
 */
static QString escape(const QString& s)
{
    QString o;
    for (QChar c : s)
    {
        if (c == '"' || c == '\\')
            o += '\\' + QString(c);
        else
            o += c;
    }
    return o;
}

/**
 * @brief 将LR(0)图转换为DOT格式
 * 
 * 将LR(0)项目集规范族和状态转换图转换为Graphviz DOT格式
 * 
 * @param gr LR(0)图结构
 * @return QString 生成的DOT格式字符串
 */
QString LR0Builder::toDot(const LR0Graph& gr)
{
    QString s;
    s += "digraph G {\nrankdir=LR\n";
    
    // 生成所有状态节点
    for (int i = 0; i < gr.states.size(); ++i)
    {
        // 聚合核心项：相同left+右部+点位合并计数
        QMap<QString, int> core;
        for (const auto& it : gr.states[i])
        {
            QString rhs;
            for (int j = 0; j < it.right.size(); ++j)
            {
                if (j == it.dot)
                    rhs += " •";
                rhs += " " + escape(it.right[j]);
            }
            if (it.dot >= it.right.size())
                rhs += " •";
            QString coreKey = escape(it.left) + " ->" + rhs;
            core[coreKey] = core.value(coreKey, 0) + 1;
        }
        
        QString label;
        for (auto it = core.begin(); it != core.end(); ++it)
        {
            // 引号已在escape中处理，这里整体再做转义
            QString line = it.key();
            label += line.replace("\"", "\\\"") + QString(" /%1\\n").arg(it.value());
        }
        
        // 添加节点定义
        s += QString("  n%1 [shape=box,label=\"%2\"]\n").arg(i).arg(label);
    }
    
    // 生成所有边
    for (int i = 0; i < gr.states.size(); ++i)
    {
        auto row = gr.edges.value(i);
        for (auto it = row.begin(); it != row.end(); ++it)
        {
            // 添加边定义
            s += QString("  n%1 -> n%2 [label=\"%3\"]\n")
                     .arg(i)
                     .arg(it.value())
                     .arg(escape(it.key()));
        }
    }
    
    s += "}\n";
    return s;
}