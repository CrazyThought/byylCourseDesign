/*
 * @file LL1.h
 * @id LL1-h
 * @brief LL(1)语法分析器头文件
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#pragma once
#include <QString>
#include <QSet>
#include <QMap>
#include <QVector>
#include "Grammar.h"

/**
 * @brief LL(1)分析器信息结构体
 * @details 存储LL(1)分析器的FIRST集、FOLLOW集、分析表和冲突信息
 */
struct LL1Info
{
    QMap<QString, QSet<QString>>      first;      ///< FIRST集合映射
    QMap<QString, QSet<QString>>      follow;     ///< FOLLOW集合映射
    QMap<QString, QMap<QString, int>> table;      ///< LL(1)分析表
    QVector<QString>                  conflicts;  ///< 冲突信息列表
};

/**
 * @brief LL(1)分析器类
 * @details 用于计算LL(1)分析器的FIRST集、FOLLOW集和分析表
 */
class LL1
{
   public:
    /**
     * @brief 计算LL(1)分析器信息
     * @param g 语法规则
     * @return LL(1)分析器信息
     * @details 根据给定的语法规则计算FIRST集、FOLLOW集和分析表
     */
    static LL1Info compute(const Grammar& g);
};