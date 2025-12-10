/*
 * 版权信息：Copyright (c) 2023 林展星
 * 文件名称：SyntaxParser.cpp
 *
 * 当前版本：1.0.0
 * 作    者：林展星
 * 完成日期：2023年12月07日
 *
 * 版本历史：
 * 1.0.0 2023-12-07 林展星 初始版本
 */
#include "task2/SyntaxParser.h"
#include "task2/configconstants.h"

static SyntaxASTNode* makeNode(const QString& sym)
{
    auto n    = new SyntaxASTNode();
    n->symbol = sym;
    return n;
}

static bool isTerminal(const QSet<QString>& terms, const QString& s)
{
    return terms.contains(s) || s == ConfigConstants::eofSymbol();
}

SyntaxResult parseTokens(const QVector<QString>& tokens, const Grammar& g, const LL1Info& info, const QMap<int, QString>& tokenMap)
{
    SyntaxResult     r;
    QVector<QString> input;
    
    // 处理词法分析器生成的tokens，将编码和词素合并为完整token
    int i = 0;
    while (i < tokens.size()) {
        QString current = tokens[i];
        bool isTokenCode = false;
        int tokenCode = current.toInt(&isTokenCode);
        
        if (isTokenCode) {
            // 检查是否为单编码token类型
            if (tokenMap.contains(tokenCode)) {
                QString tokenName = tokenMap[tokenCode];
                // 单编码token类型，需要向后继续识别词素内容
                if (i + 1 < tokens.size()) {
                    // 下一个元素是词素，合并为完整token
                    input.push_back(tokenName + " " + tokens[i + 1]);
                    i += 2;
                } else {
                    // 没有词素，只使用token名称
                    input.push_back(tokenName);
                    i++;
                }
            } else {
                // 没有对应的token映射，直接使用编码
                input.push_back(current);
                i++;
            }
        } else {
            // 不是编码，直接作为token
            input.push_back(current);
            i++;
        }
    }
    
    input.push_back(ConfigConstants::eofSymbol());
    QVector<QString> st;
    st.push_back(ConfigConstants::eofSymbol());
    st.push_back(g.startSymbol);
    QVector<SyntaxASTNode*> nodes;
    auto                    root = makeNode(g.startSymbol);
    nodes.push_back(makeNode("$"));
    nodes.push_back(root);
    int ip = 0;
    while (!st.isEmpty())
    {
        auto X = st.back();
        auto N = nodes.back();
        st.pop_back();
        nodes.pop_back();
        QString a = ip < input.size() ? input[ip] : ConfigConstants::eofSymbol();
        if (isTerminal(g.terminals, X) || X == "$")
        {
            if (X == a)
                ip++;
            else
            {
                r.errorPos = ip;
                r.errorMsg = QString("语法错误：预期终结符 '%1'，但实际收到 '%2'").arg(X).arg(a);
                r.expected = X;
                r.actual = a;
                r.stackTop = X;
                r.stackDepth = st.size();
                break;
            }
        }
        else
        {
            int idx = info.table.value(X).value(a, -1);
            if (idx < 0)
            {
                r.errorPos = ip;
                r.errorMsg = QString("语法错误：非终结符 '%1' 遇到意外的符号 '%2'，LL1表中没有对应的产生式")
                              .arg(X).arg(a);
                r.expected = "产生式";
                r.actual = a;
                r.stackTop = X;
                r.stackDepth = st.size();
                
                // 收集预期的符号列表
                QStringList expectedSymbols;
                for (auto it = info.table.value(X).begin(); it != info.table.value(X).end(); ++it) {
                    expectedSymbols.append(it.key());
                }
                if (!expectedSymbols.isEmpty()) {
                    r.expected = expectedSymbols.join(", ");
                } else {
                    r.expected = "无可用产生式";
                }
                break;
            }
            QVector<QString> rhs = g.productions[X][idx].right;
            if (rhs.size() == 1 && rhs[0] == "#")
            {
            }
            else
            {
                for (int i = rhs.size() - 1; i >= 0; --i)
                {
                    st.push_back(rhs[i]);
                    auto c = makeNode(rhs[i]);
                    N->children.push_back(c);
                    nodes.push_back(c);
                }
            }
        }
    }
    if (!root)
        root = makeNode(g.startSymbol);
    r.root = root;
    return r;
}