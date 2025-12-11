/*
 * @file LR1.cpp
 * @id LR1-cpp
 * @brief LR1分析器生成核心实现，包括LR1项集构造、LR1图构建和LR1分析表生成。
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task2/LR1.h"
#include "task2/LL1.h"
#include "task2/configconstants.h"

/**
 * @brief 判断符号是否为终结符
 * @param terms 终结符集合
 * @param s 待判断的符号
 * @return 如果是终结符或文件结束符，返回true；否则返回false
 */
static bool isTerminal(const QSet<QString>& terms, const QString& s)
{
    return terms.contains(s) || s == ConfigConstants::eofSymbol();
}

/**
 * @brief 计算序列的FIRST集
 * @param g 文法
 * @param info LL1信息，包含FIRST集
 * @param seq 符号序列
 * @param la 前瞻符号
 * @return 序列的FIRST集
 * @note 用于LR1项集的闭包计算
 */
static QSet<QString> firstSeqLL1(const Grammar&          g,
                                 const LL1Info&          info,
                                 const QVector<QString>& seq,
                                 const QString&          la)
{
    QSet<QString> res;
    if (seq.isEmpty())
    {
        res.insert(la);
        return res;
    }
    bool allEps = true;
    for (int i = 0; i < seq.size(); ++i)
    {
        const QString& X = seq[i];
        if (isTerminal(g.terminals, X) && X != ConfigConstants::epsilonSymbol())
        {
            res.insert(X);
            allEps = false;
            break;
        }
        // 非终结符：FIRST(X) 去除 #
        QSet<QString> f = info.first.value(X);
        for (const auto& a : f)
            if (a != ConfigConstants::epsilonSymbol())
                res.insert(a);
        if (!f.contains(ConfigConstants::epsilonSymbol()))
        {
            allEps = false;
            break;
        }
    }
    if (allEps)
        res.insert(la);
    return res;
}

/**
 * @brief 计算LR1项集的闭包
 * @param g 文法
 * @param info LL1信息，包含FIRST集
 * @param I 初始项集
 * @return 项集的闭包
 * @note 使用LL1的FIRST集来计算前瞻符号
 */
static QVector<LR1Item> closureLL1(const Grammar& g, const LL1Info& info, const QVector<LR1Item>& I)
{
    QVector<LR1Item> items = I;
    QSet<QString>    seen;  // serialize item as string key
    auto             key = [](const LR1Item& it)
    {
        return it.left + "->" + it.right.join(" ") + "." + QString::number(it.dot) + "/" +
               it.lookahead;
    };
    for (const auto& it : items) seen.insert(key(it));
    bool changed = true;
    while (changed)
    {
        changed = false;
        int n   = items.size();
        for (int i = 0; i < n; ++i)
        {
            const auto& it = items[i];
            if (it.dot < it.right.size())
            {
                QString B = it.right[it.dot];
                if (!isTerminal(g.terminals, B) && B != ConfigConstants::epsilonSymbol())
                {
                    // β = right[dot+1..]
                    QVector<QString> beta;
                    for (int k = it.dot + 1; k < it.right.size(); ++k) beta.push_back(it.right[k]);
                    auto lookaheads = firstSeqLL1(g, info, beta, it.lookahead);
                    for (const auto& prod : g.productions.value(B))
                    {
                        for (const auto& a : lookaheads)
                        {
                            LR1Item ni{prod.left, prod.right, 0, a};
                            QString sk = key(ni);
                            if (!seen.contains(sk))
                            {
                                items.push_back(ni);
                                seen.insert(sk);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return items;
}

/**
 * @brief 计算LR1项集的goTo函数
 * @param g 文法
 * @param info LL1信息，包含FIRST集
 * @param I 当前项集
 * @param X 转移符号
 * @return 转移后的项集
 * @note 先将项集I中所有点在X之前的项移到X之后，然后计算闭包
 */
static QVector<LR1Item> goToLL1(const Grammar&          g,
                                const LL1Info&          info,
                                const QVector<LR1Item>& I,
                                const QString&          X)
{
    if (X.isEmpty() || X == ConfigConstants::epsilonSymbol())
        return {};
    QVector<LR1Item> moved;
    for (const auto& it : I)
    {
        if (it.dot < it.right.size() && it.right[it.dot] == X)
        {
            LR1Item ni = it;
            ni.dot++;
            moved.push_back(ni);
        }
    }
    return closureLL1(g, info, moved);
}

/**
 * @brief 序列化LR1项集
 * @param I LR1项集
 * @return 序列化后的字符串
 * @note 用于判断两个项集是否相等
 */
static QString serializeSet(const QVector<LR1Item>& I)
{
    QString          s;
    QVector<QString> lines;
    for (const auto& it : I)
    {
        lines.push_back(it.left + "->" + it.right.join(" ") + "." + QString::number(it.dot) + "/" +
                        it.lookahead);
    }
    std::sort(lines.begin(), lines.end());
    return lines.join("\n");
}

/**
 * @brief 构建LR1分析图
 * @param g 文法
 * @return LR1分析图，包含状态和边
 * @note 从增广文法开始，构建完整的LR1项集族和转移关系
 */
LR1Graph LR1Builder::build(const Grammar& g)
{
    Grammar aug = g;
    if (!aug.startSymbol.isEmpty())
    {
        // 增广文法 S' -> S
        QString Sprime = g.startSymbol + ConfigConstants::augSuffix();
        if (!aug.productions.contains(Sprime))
        {
            aug.productions[Sprime].push_back({Sprime, QVector<QString>{g.startSymbol}, -1});
            aug.startSymbol = Sprime;
            aug.nonterminals.insert(Sprime);
        }
    }
    if (aug.startSymbol.isEmpty())
    {
        return LR1Graph{};
    }
    LL1Info          info = LL1::compute(aug);
    QVector<LR1Item> I0   = closureLL1(
        aug,
        info,
        QVector<LR1Item>{LR1Item{
            aug.startSymbol, aug.productions[aug.startSymbol][0].right, 0, ConfigConstants::eofSymbol()}});
    LR1Graph gr;
    gr.states.push_back(I0);
    QMap<QString, int> stateIndex;
    stateIndex[serializeSet(I0)] = 0;
    bool changed                 = true;
    while (changed)
    {
        changed = false;
        int S   = gr.states.size();
        for (int i = 0; i < S; ++i)
        {
            auto          I = gr.states[i];
            QSet<QString> nextSymbols;
            for (const auto& it : I)
            {
                if (it.dot < it.right.size())
                {
                    QString X = it.right[it.dot];
                    if (!X.isEmpty() && X != ConfigConstants::epsilonSymbol())
                        nextSymbols.insert(X);
                }
            }
            for (auto ns = nextSymbols.begin(); ns != nextSymbols.end(); ++ns)
            {
                QString X = *ns;
                auto    J = goToLL1(aug, info, I, X);
                if (J.isEmpty())
                    continue;
                QString keyJ = serializeSet(J);
                int     idx  = stateIndex.value(keyJ, -1);
                if (idx < 0)
                {
                    idx = gr.states.size();
                    gr.states.push_back(J);
                    stateIndex[keyJ] = idx;
                    changed          = true;
                }
                gr.edges[i][X] = idx;
            }
        }
    }
    return gr;
}

/**
 * @brief 生成LR1分析图的Dot格式
 * @param gr LR1分析图
 * @return Dot格式的字符串
 * @note 用于可视化LR1分析图
 */
QString LR1Builder::toDot(const LR1Graph& gr)
{
    QString dot = "digraph LR1 {\nrankdir=LR; node [shape=box,fontname=Helvetica];\n";
    for (int i = 0; i < gr.states.size(); ++i)
    {
        QMap<QString, int> core;
        for (const auto& it : gr.states[i])
        {
            QString rhs;
            for (int k = 0; k < it.right.size(); ++k)
            {
                if (k == it.dot)
                    rhs += " •";
                rhs += " " + it.right[k];
            }
            if (it.dot == it.right.size())
                rhs += " •";
            QString coreKey = it.left + " →" + rhs;
            core[coreKey]   = core.value(coreKey, 0) + 1;
        }
        QString label = QString("I%1\\n").arg(i);
        for (auto it = core.begin(); it != core.end(); ++it)
        {
            label += it.key() + QString(" /%1\\n").arg(it.value());
        }
        dot += QString("s%1 [label=\"%2\"];\n").arg(i).arg(label.replace("\"", "\\\""));
    }
    for (auto eit = gr.edges.begin(); eit != gr.edges.end(); ++eit)
    {
        int from = eit.key();
        for (auto sit = eit.value().begin(); sit != eit.value().end(); ++sit)
        {
            dot +=
                QString("s%1 -> s%2 [label=\"%3\"];\n").arg(from).arg(sit.value()).arg(sit.key());
        }
    }
    dot += "}\n";
    return dot;
}

/**
 * @brief 向动作表中添加动作
 * @param action 动作表
 * @param st 状态
 * @param a 符号
 * @param val 动作值
 * @note 如果同一状态和符号已有动作，将新动作与旧动作合并，用|分隔
 */
static void putAction(QMap<int, QMap<QString, QString>>& action,
                      int                                st,
                      const QString&                     a,
                      const QString&                     val)
{
    QString prev = action[st].value(a);
    if (!prev.isEmpty() && prev != val)
        action[st][a] = prev + "|" + val;
    else
        action[st][a] = val;
}

/**
 * @brief 计算归约编号
 * @param g 文法
 * @param outList 输出参数，归约编号与产生式的映射列表
 * @return 产生式到归约编号的映射
 * @note 用于生成归约动作
 */
static QMap<QString, int> computeReductionIndex(const Grammar&                g,
                                                QVector<QPair<int, QString>>& outList)
{
    QMap<QString, int> idx;
    int                k   = 0;
    QList<QString>     nts = QList<QString>(g.productions.keys());
    std::sort(nts.begin(), nts.end());
    for (const auto& A : nts)
    {
        // 跳过增广非终结符
        if (A.endsWith(ConfigConstants::augSuffix()))
            continue;
        const auto& alts = g.productions.value(A);
        for (const auto& p : alts)
        {
            QString key = A + "->" + p.right.join(" ");
            idx[key]    = k;
            outList.push_back({k, A + " -> " + p.right.join(" ")});
            ++k;
        }
    }
    return idx;
}

/**
 * @brief 计算LR1动作表
 * @param g 文法
 * @param gr LR1分析图
 * @return LR1动作表，包含Action表和Goto表
 * @note 遍历所有LR1项，生成移进、归约和接受动作
 */
LR1ActionTable LR1Builder::computeActionTable(const Grammar& g, const LR1Graph& gr)
{
    LR1ActionTable t;
    auto           redIndex = computeReductionIndex(g, t.reductions);
    for (int i = 0; i < gr.states.size(); ++i)
    {
        const auto& I = gr.states[i];
        for (const auto& it : I)
        {
            if (it.dot < it.right.size())
            {
                QString X = it.right[it.dot];
                if (isTerminal(g.terminals, X))
                {
                    auto edIt = gr.edges.constFind(i);
                    int  to   = -1;
                    if (edIt != gr.edges.constEnd())
                        to = edIt.value().value(X, -1);
                    if (to >= 0)
                        putAction(t.action, i, X, QString("s%1").arg(to));
                }
            }
            else
            {
                if (it.left.endsWith(ConfigConstants::augSuffix()) && it.lookahead == ConfigConstants::eofSymbol())
                {
                    putAction(t.action, i, ConfigConstants::eofSymbol(), "acc");
                }
                QString a = it.lookahead;
                if (!a.isEmpty() && a != "#")
                {
                    QString key = it.left + "->" + it.right.join(" ");
                    int     rk  = redIndex.value(key, -1);
                    QString r   = rk >= 0
                                      ? QString("r%1").arg(rk)
                                      : QString("r %1 -> %2").arg(it.left).arg(it.right.join(" "));
                    putAction(t.action, i, a, r);
                }
            }
        }
        auto edIt = gr.edges.constFind(i);
        if (edIt != gr.edges.constEnd())
        {
            const auto& edgesMap = edIt.value();
            for (auto eit = edgesMap.begin(); eit != edgesMap.end(); ++eit)
            {
                QString X  = eit.key();
                int     to = eit.value();
                if (!isTerminal(g.terminals, X) && X != "#")
                    t.gotoTable[i][X] = to;
            }
        }
    }
    return t;
}