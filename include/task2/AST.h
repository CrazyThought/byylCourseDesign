/*
 * @file AST.h
 * @id AST-h
 * @brief 定义抽象语法树(AST)节点结构体，包括语法AST和语义AST
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#pragma once
#include <QString>
#include <QVector>

/**
 * @brief 语法抽象语法树节点结构体
 * 
 * 用于表示语法分析阶段生成的抽象语法树，包含符号名称和子节点列表
 */
struct SyntaxASTNode
{
    QString                 symbol;   ///< 节点的符号名称
    QVector<SyntaxASTNode*> children; ///< 子节点列表，按语法规则顺序排列
};

/**
 * @brief 语义抽象语法树节点结构体
 * 
 * 用于表示语义分析阶段生成的抽象语法树，包含标签和子节点列表
 */
struct SemanticASTNode
{
    QString                   tag;      ///< 节点的标签，用于语义分析
    QVector<SemanticASTNode*> children; ///< 子节点列表，按语义规则顺序排列
};