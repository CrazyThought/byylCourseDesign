/*
 * @file SyntaxParser.cpp
 * @id SyntaxParser-cpp
 * @brief 实现基于LL1分析的语法解析器，将token序列转换为抽象语法树(AST)
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task2/SyntaxParser.h"
#include "task2/configconstants.h"

/**
 * @brief 创建语法AST节点
 * 
 * 动态分配内存并初始化一个语法AST节点
 * 
 * @param sym 节点的符号名称
 * @return SyntaxASTNode* 创建的AST节点指针
 */
static SyntaxASTNode* makeNode(const QString& sym)
{
    auto n    = new SyntaxASTNode();
    n->symbol = sym;
    return n;
}

/**
 * @brief 判断符号是否为终结符
 * 
 * 检查给定符号是否为终结符或EOF符号
 * 
 * @param terms 终结符集合
 * @param s 要检查的符号
 * @return bool 是终结符返回true，否则返回false
 */
static bool isTerminal(const QSet<QString>& terms, const QString& s)
{
    return terms.contains(s) || s == ConfigConstants::eofSymbol();
}

/**
 * @brief 解析token序列生成语法树
 * 
 * 使用LL1分析表对token序列进行语法解析，生成抽象语法树(AST)，
 * 并返回解析结果，包括成功的AST或失败的错误信息
 * 
 * @param tokens 待解析的token序列，由词法分析器生成
 * @param g 语法对象，包含语法规则和产生式
 * @param info LL1分析信息，包含分析表和其他辅助数据
 * @param tokenMap token映射表，键为token类型ID，值为token类型名称
 * @return SyntaxResult 解析结果，包含AST或错误信息
 */
SyntaxResult parseTokens(const QVector<QString>& tokens, const Grammar& g, const LL1Info& info, const QMap<int, QString>& tokenMap)
{
    SyntaxResult     r;        // 解析结果对象
    QVector<QString> input;    // 处理后的输入token序列
    
    // 处理词法分析器生成的tokens，将编码和词素合并为完整token
    int i = 0;
    while (i < tokens.size()) {
        QString current = tokens[i];
        bool isTokenCode = false;
        int tokenCode = current.toInt(&isTokenCode);
        
        if (isTokenCode) {
            // 当前token是数字编码
            if (tokenMap.contains(tokenCode)) {
                QString tokenName = tokenMap[tokenCode];
                // 检查是否有对应的词素
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
            // 不是数字编码，直接作为token
            input.push_back(current);
            i++;
        }
    }
    
    // 添加EOF符号到输入序列末尾
    input.push_back(ConfigConstants::eofSymbol());
    
    // 初始化解析栈和AST节点栈
    QVector<QString> st;                  // 解析栈
    st.push_back(ConfigConstants::eofSymbol());
    st.push_back(g.startSymbol);
    
    QVector<SyntaxASTNode*> nodes;        // AST节点栈
    auto root = makeNode(g.startSymbol);  // 根节点
    nodes.push_back(makeNode("$"));       // EOF对应的节点
    nodes.push_back(root);                // 起始符号对应的节点
    
    int ip = 0;  // 输入指针，指向当前待处理的输入符号
    
    // 主解析循环
    while (!st.isEmpty()) {
        // 获取栈顶符号和对应的AST节点
        auto X = st.back();
        auto N = nodes.back();
        st.pop_back();
        nodes.pop_back();
        
        // 获取当前输入符号
        QString a = ip < input.size() ? input[ip] : ConfigConstants::eofSymbol();
        
        // 处理终结符情况
        if (isTerminal(g.terminals, X) || X == "$") {
            if (X == a) {
                // 匹配成功，移动输入指针
                ip++;
            } else {
                // 匹配失败，记录错误信息
                r.errorPos = ip;
                r.errorMsg = QString("语法错误：预期终结符 '%1'，但实际收到 '%2'").arg(X).arg(a);
                r.expected = X;
                r.actual = a;
                r.stackTop = X;
                r.stackDepth = st.size();
                break;
            }
        } else {
            // 处理非终结符情况
            // 查找LL1分析表中的产生式索引
            int idx = info.table.value(X).value(a, -1);
            if (idx < 0) {
                // 没有找到对应的产生式，记录错误信息
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
            
            // 获取产生式右部
            QVector<QString> rhs = g.productions[X][idx].right;
            if (rhs.size() == 1 && rhs[0] == ConfigConstants::epsilonSymbol()) {
                // 空产生式，不生成子节点
            } else {
                // 非空产生式，将右部符号逆序入栈（因为栈是后进先出）
                for (int i = rhs.size() - 1; i >= 0; --i) {
                    st.push_back(rhs[i]);
                    // 为每个符号创建AST节点，并添加到父节点的子节点列表
                    auto c = makeNode(rhs[i]);
                    N->children.push_back(c);
                    nodes.push_back(c);
                }
            }
        }
    }
    
    // 确保根节点存在
    if (!root) {
        root = makeNode(g.startSymbol);
    }
    r.root = root;
    return r;
}