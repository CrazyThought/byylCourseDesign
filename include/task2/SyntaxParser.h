/*
 * @file SyntaxParser.h
 * @id SyntaxParser-h
 * @brief 定义语法解析器的接口和结果结构体，提供基于LL1分析的语法解析功能
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#pragma once
#include <QVector>
#include <QString>
#include "Grammar.h"
#include "LL1.h"
#include "AST.h"

/**
 * @brief 语法解析结果结构体
 * 
 * 存储语法解析的结果，包括生成的抽象语法树(AST)、错误信息（如果有）、
 * 预期符号、实际符号以及解析栈状态等信息
 */
struct SyntaxResult
{
    SyntaxASTNode* root       = nullptr; ///< 抽象语法树的根节点，解析成功时生成
    int            errorPos   = -1;      ///< 错误位置，-1表示无错误
    QString        errorMsg   = "";      ///< 错误信息描述
    QString        expected   = "";      ///< 预期的符号
    QString        actual     = "";      ///< 实际遇到的符号
    QString        stackTop   = "";      ///< 解析栈顶部的符号
    int            stackDepth = 0;       ///< 解析栈的深度
};

/**
 * @brief 解析token序列生成语法树
 * 
 * 使用LL1分析表对token序列进行语法解析，生成抽象语法树(AST)，
 * 并返回解析结果，包括成功的AST或失败的错误信息
 * 
 * @param tokens 待解析的token序列
 * @param g 语法对象，包含语法规则和产生式
 * @param info LL1分析信息，包含分析表和其他辅助数据
 * @param tokenMap token映射表，键为token类型ID，值为token类型名称
 * @return SyntaxResult 解析结果，包含AST或错误信息
 */
SyntaxResult parseTokens(const QVector<QString>& tokens, const Grammar& g, const LL1Info& info, const QMap<int, QString>& tokenMap = QMap<int, QString>());