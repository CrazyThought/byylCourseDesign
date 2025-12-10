/*
 * 版权信息：Copyright (c) 2023 林展星
 * 文件名称：TokenInfo.h
 *           定义TokenInfo结构体，用于存储token类型和词素
 *
 * 当前版本：1.0.0
 * 作    者：林展星
 * 完成日期：2023年12月7日
 *
 * 版本历史：
 * 1.0.0 2023-12-07 林展星 初始版本
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