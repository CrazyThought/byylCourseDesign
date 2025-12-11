/*
 * @file DotGenerator.cpp
 * @id dotgenerator-cpp
 * @brief 实现DOT图生成功能，用于将语法树、解析树和语义树转换为可视化的DOT格式
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task2/AST.h"
#include "task2/LR1Parser.h"
#include <QString>
#include <QTextStream>
#include <QFile>
#include <QMap>

/**
 * @brief 递归生成语法AST节点的DOT表示
 * 
 * @param o 输出文本流
 * @param n 当前AST节点
 * @param id 节点ID计数器，会被修改
 * @param ids 节点到ID的映射表，会被修改
 */
static void emitNode(QTextStream&                     o,
                     const SyntaxASTNode*             n,
                     int&                             id,
                     QMap<const SyntaxASTNode*, int>& ids)
{
    int nid = ++id;       // 生成唯一节点ID
    ids.insert(n, nid);   // 记录节点到ID的映射
    
    // 输出节点定义
    o << "  n" << nid << " [label=\"" << n->symbol << "\"]\n";
    
    // 递归处理子节点
    for (auto c : n->children)
    {
        emitNode(o, c, id, ids);
        int cid = ids.value(c);
        // 输出边定义
        o << "  n" << nid << " -> n" << cid << "\n";
    }
}

/**
 * @brief 将语法抽象语法树(AST)转换为DOT格式
 * 
 * 将语法分析器生成的抽象语法树转换为Graphviz DOT格式，用于可视化展示
 * 
 * @param root 语法AST的根节点
 * @return QString 生成的DOT格式字符串
 */
QString syntaxAstToDot(SyntaxASTNode* root)
{
    QString s;
    QTextStream o(&s);
    
    // DOT文件头部
    o << "digraph G {\nrankdir=TB\n";
    
    int id = 0;                            // 节点ID计数器
    QMap<const SyntaxASTNode*, int> ids;   // 节点到ID的映射
    
    if (root) {
        // 递归生成所有节点和边
        emitNode(o, root, id, ids);
    }
    
    // DOT文件尾部
    o << "}\n";
    
    return s;
}

/**
 * @brief 递归生成解析树节点的DOT表示
 * 
 * @param o 输出文本流
 * @param n 当前解析树节点
 * @param id 节点ID计数器，会被修改
 * @param ids 节点到ID的映射表，会被修改
 */
static void emitParseNode(QTextStream&                     o,
                          const ParseTreeNode*             n,
                          int&                             id,
                          QMap<const ParseTreeNode*, int>& ids)
{
    int nid = ++id;       // 生成唯一节点ID
    ids.insert(n, nid);   // 记录节点到ID的映射
    
    // 输出节点定义
    o << "  n" << nid << " [label=\"" << n->symbol << "\"]\n";
    
    // 递归处理子节点
    for (auto c : n->children)
    {
        emitParseNode(o, c, id, ids);
        int cid = ids.value(c);
        // 输出边定义
        o << "  n" << nid << " -> n" << cid << "\n";
    }
}

/**
 * @brief 将解析树转换为DOT格式
 * 
 * 将解析过程生成的解析树转换为Graphviz DOT格式，用于可视化展示
 * 
 * @param root 解析树的根节点
 * @return QString 生成的DOT格式字符串
 */
QString parseTreeToDot(ParseTreeNode* root)
{
    QString s;
    QTextStream o(&s);
    
    // DOT文件头部
    o << "digraph G {\nrankdir=TB\n";
    
    int id = 0;                            // 节点ID计数器
    QMap<const ParseTreeNode*, int> ids;   // 节点到ID的映射
    
    if (root) {
        // 递归生成所有节点和边
        emitParseNode(o, root, id, ids);
    }
    
    // DOT文件尾部
    o << "}\n";
    
    return s;
}

/**
 * @brief 递归生成带token信息的解析树节点的DOT表示
 * 
 * @param o 输出文本流
 * @param n 当前解析树节点
 * @param id 节点ID计数器，会被修改
 * @param ids 节点到ID的映射表，会被修改
 * @param tokens token序列
 * @param leafIdx 当前处理的token索引，会被修改
 */
static void emitParseNodeWithLeafTokens(QTextStream&                     o,
                                        const ParseTreeNode*             n,
                                        int&                             id,
                                        QMap<const ParseTreeNode*, int>& ids,
                                        const QVector<QString>&          tokens,
                                        int&                             leafIdx)
{
    int nid = ++id;       // 生成唯一节点ID
    ids.insert(n, nid);   // 记录节点到ID的映射
    
    bool isLeaf = n->children.isEmpty();
    int start = leafIdx;
    QString label;
    
    if (isLeaf) {
        // 叶子节点，显示token信息
        QString tok = (leafIdx < tokens.size()) ? tokens[leafIdx] : QString();
        label = tok.isEmpty() ? n->symbol : QString("%1/%2").arg(n->symbol).arg(tok);
        
        if (leafIdx < tokens.size()) {
            leafIdx++;
        }
    } else {
        // 非叶子节点，递归处理子节点
        for (auto c : n->children) {
            emitParseNodeWithLeafTokens(o, c, id, ids, tokens, leafIdx);
        }
        
        int end = qMax(start, leafIdx - 1);
        QString preview;
        
        // 生成token预览信息
        if (!tokens.isEmpty() && start >= 0 && start < tokens.size()) {
            int upto = qMin(end, tokens.size() - 1);
            int cnt = qMin(upto - start + 1, 6);
            
            for (int i = 0; i < cnt; ++i) {
                if (i) {
                    preview += " ";
                }
                preview += tokens[start + i];
            }
            
            if (upto - start + 1 > cnt) {
                preview += " ...";
            }
        }
        
        if (!preview.isEmpty()) {
            label = QString("%1 [%2..%3]: %4").arg(n->symbol).arg(start).arg(end).arg(preview);
        } else {
            label = n->symbol;
        }
    }
    
    // 输出节点定义
    o << "  n" << nid << " [label=\"" << label << "\"]\n";
    
    // 输出边定义
    for (auto c : n->children) {
        int cid = ids.value(c);
        o << "  n" << nid << " -> n" << cid << "\n";
    }
}

/**
 * @brief 将解析树与token序列结合转换为DOT格式
 * 
 * 将解析树与对应的token序列结合，生成包含token信息的Graphviz DOT格式
 * 
 * @param root 解析树的根节点
 * @param tokens 对应的token序列
 * @return QString 生成的DOT格式字符串
 */
QString parseTreeToDotWithTokens(ParseTreeNode* root, const QVector<QString>& tokens)
{
    QString s;
    QTextStream o(&s);
    
    // DOT文件头部
    o << "digraph G {\nrankdir=TB\n";
    
    // 添加token预览作为图标题
    if (!tokens.isEmpty()) {
        QString preview;
        int previewN = qMin(tokens.size(), 8);
        
        for (int i = 0; i < previewN; ++i) {
            if (i) {
                preview += " ";
            }
            preview += tokens[i];
        }
        
        o << "label=\"Tokens: " << preview << (tokens.size() > previewN ? " ..." : "")
          << "\"\nlabelloc=t\nfontsize=12\n";
    }
    
    int id = 0;                            // 节点ID计数器
    QMap<const ParseTreeNode*, int> ids;   // 节点到ID的映射
    int leafIdx = 0;                       // 当前处理的token索引
    
    if (root) {
        // 递归生成所有节点和边，包含token信息
        emitParseNodeWithLeafTokens(o, root, id, ids, tokens, leafIdx);
    }
    
    // DOT文件尾部
    o << "}\n";
    
    return s;
}

/**
 * @brief 递归生成语义AST节点的DOT表示
 * 
 * @param o 输出文本流
 * @param n 当前语义AST节点
 * @param id 节点ID计数器，会被修改
 * @param ids 节点到ID的映射表，会被修改
 */
static void emitSemanticNode(QTextStream&                       o,
                             const SemanticASTNode*             n,
                             int&                               id,
                             QMap<const SemanticASTNode*, int>& ids)
{
    int nid = ++id;       // 生成唯一节点ID
    ids.insert(n, nid);   // 记录节点到ID的映射
    
    // 输出节点定义
    o << "  n" << nid << " [label=\"" << n->tag << "\"]\n";
    
    // 递归处理子节点
    for (auto c : n->children) {
        emitSemanticNode(o, c, id, ids);
        int cid = ids.value(c);
        // 输出边定义
        o << "  n" << nid << " -> n" << cid << "\n";
    }
}

/**
 * @brief 将语义抽象语法树(AST)转换为DOT格式
 * 
 * 将语义分析生成的抽象语法树转换为Graphviz DOT格式，用于可视化展示
 * 
 * @param root 语义AST的根节点
 * @return QString 生成的DOT格式字符串
 */
QString semanticAstToDot(SemanticASTNode* root)
{
    QString s;
    QTextStream o(&s);
    
    // DOT文件头部
    o << "digraph G {\nrankdir=TB\n";
    
    int id = 0;                              // 节点ID计数器
    QMap<const SemanticASTNode*, int> ids;   // 节点到ID的映射
    
    if (root) {
        // 递归生成所有节点和边
        emitSemanticNode(o, root, id, ids);
    }
    
    // DOT文件尾部
    o << "}\n";
    
    return s;
}