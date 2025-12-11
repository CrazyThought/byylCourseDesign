/*
 * @file DotGenerator.h
 * @id DotGenerator-h
 * @brief 提供DOT图生成功能，用于将语法树、解析树和语义树转换为可视化的DOT格式
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#pragma once
#include <QString>
#include "AST.h"
#include "LR1Parser.h"

/**
 * @brief 将语法抽象语法树(AST)转换为DOT格式
 * 
 * 将语法分析器生成的抽象语法树转换为Graphviz DOT格式，用于可视化展示
 * 
 * @param root 语法AST的根节点
 * @return QString 生成的DOT格式字符串
 */
QString syntaxAstToDot(SyntaxASTNode* root);

/**
 * @brief 将解析树转换为DOT格式
 * 
 * 将解析过程生成的解析树转换为Graphviz DOT格式，用于可视化展示
 * 
 * @param root 解析树的根节点
 * @return QString 生成的DOT格式字符串
 */
QString parseTreeToDot(ParseTreeNode* root);

/**
 * @brief 将解析树与token序列结合转换为DOT格式
 * 
 * 将解析树与对应的token序列结合，生成包含token信息的Graphviz DOT格式
 * 
 * @param root 解析树的根节点
 * @param tokens 对应的token序列
 * @return QString 生成的DOT格式字符串
 */
QString parseTreeToDotWithTokens(ParseTreeNode* root, const QVector<QString>& tokens);

/**
 * @brief 将语义抽象语法树(AST)转换为DOT格式
 * 
 * 将语义分析生成的抽象语法树转换为Graphviz DOT格式，用于可视化展示
 * 
 * @param root 语义AST的根节点
 * @return QString 生成的DOT格式字符串
 */
QString semanticAstToDot(SemanticASTNode* root);