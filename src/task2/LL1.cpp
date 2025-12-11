/*
 * @file LL1.cpp
 * @id LL1-cpp
 * @brief 实现LL(1)分析表生成算法，包括FIRST集、FOLLOW集计算和LL(1)分析表构建
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task2/LL1.h"
#include "task2/configconstants.h"

/**
 * @brief 计算符号序列的FIRST集
 * 
 * 计算给定符号序列的FIRST集，用于构建LL(1)分析表
 * 
 * @param g 文法对象
 * @param seq 符号序列
 * @param first 已计算的FIRST集映射
 * @return QSet<QString> 符号序列的FIRST集
 */
static QSet<QString> firstSeq(const Grammar&                      g,
                              const QVector<QString>&             seq,
                              const QMap<QString, QSet<QString>>& first)
{
    QSet<QString> r;
    bool allEps = true;  // 标记是否所有符号都可以推导出空串
    
    // 遍历符号序列
    for (const auto& s : seq)
    {
        if (g.terminals.contains(s))
        {
            // 遇到终结符，将其加入结果集
            r.insert(s);
            allEps = false;
            break;
        }
        
        // 非终结符，获取其FIRST集
        auto fs = first.value(s);
        
        // 将非空串符号加入结果集
        for (const auto& t : fs)
            if (t != ConfigConstants::epsilonSymbol())
                r.insert(t);
        
        // 如果该非终结符不能推导出空串，则整个序列不能推导出空串
        if (!fs.contains(ConfigConstants::epsilonSymbol()))
        {
            allEps = false;
            break;
        }
    }
    
    // 如果所有符号都可以推导出空串，则整个序列可以推导出空串
    if (allEps)
        r.insert(ConfigConstants::epsilonSymbol());
    
    return r;
}

/**
 * @brief 计算LL(1)分析表
 * 
 * 根据给定的文法，计算FIRST集、FOLLOW集，并构建LL(1)分析表
 * 
 * @param g 文法对象
 * @return LL1Info LL(1)分析信息，包含FIRST集、FOLLOW集、分析表和冲突信息
 */
LL1Info LL1::compute(const Grammar& g)
{
    LL1Info info;
    
    // 初始化FIRST集
    // 为每个非终结符和终结符创建FIRST集
    for (const auto& A : g.nonterminals) {
        info.first.insert(A, QSet<QString>());
    }
    for (const auto& a : g.terminals) {
        info.first.insert(a, QSet<QString>());
    }
    // 终结符的FIRST集就是其自身
    for (const auto& a : g.terminals) {
        info.first[a].insert(a);
    }
    
    // 迭代计算FIRST集，直到不再变化
    bool changed = true;
    while (changed)
    {
        changed = false;
        
        // 遍历所有产生式
        for (auto it = g.productions.begin(); it != g.productions.end(); ++it)
        {
            const QString& A = it.key();  // 产生式左部非终结符
            
            // 遍历该非终结符的所有产生式
            for (const auto& p : it.value())
            {
                // 计算产生式右部序列的FIRST集
                auto set = firstSeq(g, p.right, info.first);
                int prev = info.first[A].size();
                
                // 将计算结果合并到非终结符A的FIRST集中
                for (const auto& x : set) {
                    info.first[A].insert(x);
                }
                
                // 检查是否有变化
                if (info.first[A].size() > prev)
                    changed = true;
            }
        }
    }
    
    // 初始化FOLLOW集
    for (const auto& A : g.nonterminals) {
        info.follow.insert(A, QSet<QString>());
    }
    // 起始符号的FOLLOW集包含EOF符号
    if (!g.startSymbol.isEmpty()) {
        info.follow[g.startSymbol].insert(ConfigConstants::eofSymbol());
    }
    
    // 迭代计算FOLLOW集，直到不再变化
    changed = true;
    while (changed)
    {
        changed = false;
        
        // 遍历所有产生式
        for (auto it = g.productions.begin(); it != g.productions.end(); ++it)
        {
            const QString& A = it.key();  // 产生式左部非终结符
            
            // 遍历该非终结符的所有产生式
            for (const auto& p : it.value())
            {
                // 遍历产生式右部的每个符号
                for (int i = 0; i < p.right.size(); ++i)
                {
                    const auto& B = p.right[i];
                    
                    // 只处理非终结符
                    if (!g.nonterminals.contains(B))
                        continue;
                    
                    // 构建β序列（B之后的符号序列）
                    QVector<QString> beta;
                    for (int j = i + 1; j < p.right.size(); ++j) {
                        beta.push_back(p.right[j]);
                    }
                    
                    // 计算β的FIRST集
                    auto fbeta = firstSeq(g, beta, info.first);
                    int prev = info.follow[B].size();
                    
                    // 将fbeta中的非空串符号加入B的FOLLOW集
                    for (const auto& x : fbeta)
                        if (x != ConfigConstants::epsilonSymbol())
                            info.follow[B].insert(x);
                    
                    // 如果β可以推导出空串，则将A的FOLLOW集加入B的FOLLOW集
                    if (beta.isEmpty() || fbeta.contains(ConfigConstants::epsilonSymbol()))
                    {
                        for (const auto& x : info.follow[A]) {
                            info.follow[B].insert(x);
                        }
                    }
                    
                    // 检查是否有变化
                    if (info.follow[B].size() > prev)
                        changed = true;
                }
            }
        }
    }
    
    // 构建LL(1)分析表
    for (auto it = g.productions.begin(); it != g.productions.end(); ++it)
    {
        const QString& A = it.key();  // 非终结符
        
        // 遍历该非终结符的所有产生式
        for (int k = 0; k < it.value().size(); ++k)
        {
            const auto& p = it.value()[k];  // 当前产生式
            
            // 计算产生式右部的FIRST集
            auto fs = firstSeq(g, p.right, info.first);
            
            // 处理FIRST集中的终结符
            for (const auto& a : fs)
            {
                if (a == ConfigConstants::epsilonSymbol())
                    continue;
                
                // 检查冲突
                if (info.table[A].contains(a))
                    info.conflicts.push_back(A + "/" + a);
                
                // 在分析表中记录产生式编号
                info.table[A][a] = k;
            }
            
            // 如果产生式可以推导出空串，则处理FOLLOW集中的终结符
            if (fs.contains(ConfigConstants::epsilonSymbol()))
            {
                for (const auto& b : info.follow[A])
                {
                    // 检查冲突
                    if (info.table[A].contains(b))
                        info.conflicts.push_back(A + "/" + b);
                    
                    // 在分析表中记录产生式编号
                    info.table[A][b] = k;
                }
            }
        }
    }
    
    return info;
}