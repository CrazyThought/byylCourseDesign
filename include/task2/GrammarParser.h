/*
 * @file GrammarParser.h
 * @id GrammarParser-h
 * @brief 提供文法解析功能，用于从文件或字符串中解析生成文法对象
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#pragma once
#include "Grammar.h"
#include <QString>

/**
 * @brief 文法解析器命名空间
 * 
 * 提供从文件或字符串中解析文法规则的功能，
 * 生成Grammar对象用于后续的语法分析器构建
 */
namespace GrammarParser
{
    /**
     * @brief 从文件中解析文法
     * 
     * 读取指定路径的文件，解析其中的文法规则，生成Grammar对象
     * 
     * @param path 文法文件的路径
     * @param error 错误信息输出，解析失败时会存储详细的错误描述
     * @return Grammar 解析生成的文法对象，解析失败时返回无效的文法
     */
    Grammar parseFile(const QString& path, QString& error);
    
    /**
     * @brief 从字符串中解析文法
     * 
     * 解析给定的字符串，生成Grammar对象
     * 
     * @param text 包含文法规则的字符串
     * @param error 错误信息输出，解析失败时会存储详细的错误描述
     * @return Grammar 解析生成的文法对象，解析失败时返回无效的文法
     */
    Grammar parseString(const QString& text, QString& error);
}  // namespace GrammarParser