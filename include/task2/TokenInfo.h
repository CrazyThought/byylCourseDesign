/*
 * @file TokenInfo.h
 * @id TokenInfo-h
 * @brief 定义TokenInfo结构体，用于存储词法分析器生成的token类型和词素信息
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#pragma once
#include <QString>

/**
 * @brief Token信息结构体，包含token类型和词素
 */
typedef struct TokenInfo {
    QString tokenType; // token类型
    QString lexeme;    // 词素，没有词素则为空字符串
} TokenInfo;