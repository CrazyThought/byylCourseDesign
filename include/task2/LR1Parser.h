/*
 * @file LR1Parser.h
 * @id LR1Parser-h
 * @brief LR1语法分析器头文件，支持步骤记录与语义AST构建
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#pragma once
#include <QString>
#include <QVector>
#include <QMap>
#include "Grammar.h"
#include "LR1.h"
#include "AST.h"
#include "TokenInfo.h"

/**
 * @brief 语法分析树节点结构体
 * @struct ParseTreeNode
 * @details 表示语法分析树的一个节点，包含节点符号和子节点列表
 */
struct ParseTreeNode
{
    QString                 symbol;         ///< 节点符号
    QVector<ParseTreeNode*> children;       ///< 子节点列表
};

/**
 * @brief 语法分析步骤结构体
 * @struct ParseStep
 * @details 表示语法分析过程中的一个步骤，包含步骤号、栈状态、剩余输入、动作和产生式
 */
struct ParseStep
{
    int                          step = 0;      ///< 步骤号
    QVector<QPair<int, QString>> stack;         ///< 当前栈状态，元素为(状态号, 符号)
    QVector<TokenInfo>           rest;          ///< 剩余输入token列表
    QString                      action;        ///< 当前动作，如"s10"、"r5"、"acc"、"error"
    QString                      production;    ///< 当前使用的产生式
};

/**
 * @brief 语法分析结果结构体
 * @struct ParseResult
 * @details 表示语法分析的完整结果，包含分析步骤、语法树、语义树和错误信息
 */
struct ParseResult
{
    QVector<ParseStep> steps;               ///< 语法分析步骤列表
    ParseTreeNode*     root     = nullptr;   ///< 语法分析树根节点，nullptr表示解析失败
    int                errorPos = -1;        ///< 错误位置，-1表示无错误
    QString            errorMsg = "";       ///< 错误信息
    SemanticASTNode*   astRoot  = nullptr;   ///< 语义AST根节点，nullptr表示无语义树
    QVector<ParseStep> semanticSteps;        ///< 语义分析步骤列表
};

/**
 * @brief LR1语法分析器类
 * @class LR1Parser
 * @details 实现LR1语法分析算法，支持普通语法分析和带语义动作的语法分析
 */
class LR1Parser
{
   public:
    /**
     * @brief 执行普通LR1语法分析
     * @param tokens 终结符token序列，末尾隐含 `$`
     * @param g 文法
     * @param t LR(1) 动作/GOTO 表
     * @return 包含分析步骤与解析树的结果
     */
    static ParseResult parse(const QVector<TokenInfo>& tokens,
                             const Grammar&          g,
                             const LR1ActionTable&   t);
    
    /**
     * @brief 执行带语义动作的LR1语法分析
     * @param tokens 终结符token序列，末尾隐含 `$`
     * @param g 文法
     * @param t LR(1) 动作/GOTO 表
     * @param actions 语义动作配置，键为非终结符，值为产生式对应的角色列表
     * @param roleMeaning 角色含义映射，键为角色代码，值为角色名称（"skip"、"root"、"child"）
     * @param rootPolicy 根节点选择策略
     * @param childOrder 子节点顺序
     * @return 包含分析步骤、解析树和语义树的结果
     */
    static ParseResult parseWithSemantics(const QVector<TokenInfo>&                    tokens,
                                          const Grammar&                              g,
                                          const LR1ActionTable&                       t,
                                          const QMap<QString, QVector<QVector<int>>>& actions,
                                          const QMap<int, QString>&                   roleMeaning,
                                          const QString&                              rootPolicy,
                                          const QString&                              childOrder);
};