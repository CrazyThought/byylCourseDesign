/*
 * @file SLR.cpp
 * @id SLR-cpp
 * @brief 实现SLR(1)文法检查算法，用于判断文法是否为SLR(1)文法并检测冲突
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task2/SLR.h"
#include "task2/LR0.h"

/**
 * @brief 判断符号是否为终结符
 * 
 * @param terms 终结符集合
 * @param s 要检查的符号
 * @return bool 是终结符返回true，否则返回false
 */
static bool isTerminal(const QSet<QString>& terms, const QString& s)
{
    return terms.contains(s);
}

/**
 * @brief 检查文法是否为SLR(1)文法
 * 
 * 构建LR(0)项目集规范族，计算动作集，检测是否存在冲突
 * 
 * @param g 文法对象
 * @param ll1 LL1分析信息，包含FOLLOW集
 * @return SLRCheckResult SLR检查结果，包含冲突信息和是否为SLR(1)文法的标识
 */
SLRCheckResult SLR::check(const Grammar& g, const LL1Info& ll1)
{
    // 构建LR(0)项目集规范族和状态转换图
    auto gr = LR0Builder::build(g);
    
    // 动作集映射：状态 -> 终结符 -> 动作集合
    QMap<int, QMap<QString, QSet<QString>>> actionsSet;
    
    // 遍历所有状态
    for (int st = 0; st < gr.states.size(); ++st)
    {
        const auto& items = gr.states[st];
        
        // 遍历状态中的每个项目
        for (const auto& it : items)
        {
            if (it.dot < it.right.size())
            {
                // 点在产生式右部中间，计算移进动作
                QString a = it.right[it.dot];  // 点后的符号
                
                if (isTerminal(g.terminals, a))
                {
                    // 如果点后的符号是终结符，生成移进动作
                    int to = gr.edges.value(st).value(a, -1);
                    if (to >= 0)
                    {
                        actionsSet[st][a].insert(QString("s%1").arg(to));
                    }
                }
            }
            else
            {
                // 点在产生式右部末尾，计算归约动作
                auto followA = ll1.follow.value(it.left);  // 左部非终结符的FOLLOW集
                QString rhsText = it.right.isEmpty() ? QString("#") : it.right.join(" ");
                QString red = QString("r %1 -> %2").arg(it.left).arg(rhsText);
                
                // 在FOLLOW集的每个终结符上添加归约动作
                for (const auto& a : followA)
                {
                    actionsSet[st][a].insert(red);
                }
            }
        }
    }
    
    SLRCheckResult res;
    
    // 检测冲突
    for (auto sit = actionsSet.begin(); sit != actionsSet.end(); ++sit)
    {
        int st = sit.key();  // 当前状态
        
        // 遍历当前状态的所有终结符
        for (auto ait = sit.value().begin(); ait != sit.value().end(); ++ait)
        {
            const QString& a = ait.key();     // 当前终结符
            const auto& set = ait.value();    // 动作集合
            
            if (set.size() >= 2)
            {
                // 存在冲突，区分冲突类型
                bool hasShift = false, hasReduce = false;
                for (const auto& act : set)
                {
                    if (act.startsWith("s"))
                        hasShift = true;
                    if (act.startsWith("r"))
                        hasReduce = true;
                }
                
                // 构建冲突信息
                QStringList list = QStringList(set.begin(), set.end());
                std::sort(list.begin(), list.end());
                
                SLRConflict c;
                c.state = st;                  // 冲突状态
                c.terminal = a;                // 冲突终结符
                c.type = (hasShift && hasReduce) ? "shift/reduce" : "reduce/reduce";  // 冲突类型
                c.detail = list.join("|");     // 冲突详情
                
                res.conflicts.push_back(c);
            }
        }
    }
    
    // 检查是否为SLR(1)文法
    res.isSLR1 = res.conflicts.isEmpty();
    
    return res;
}
