/*
 * @file SLR.h
 * @id SLR-h
 * @brief SLR(1)语法分析器头文件
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#pragma once
#include <QString>
#include <QVector>
#include <QMap>
#include <QSet>
#include "Grammar.h"
#include "LL1.h"

/**
 * @brief SLR冲突结构体
 * @details 表示SLR(1)分析中的一个冲突，包含冲突的状态、终结符、类型和详细信息
 */
struct SLRConflict
{
    int     state;      ///< 冲突发生的状态编号
    QString terminal;   ///< 导致冲突的终结符
    QString type;       ///< 冲突类型（如"移进-归约"或"归约-归约"）
    QString detail;     ///< 冲突的详细描述
};

/**
 * @brief SLR检查结果结构体
 * @details 表示SLR(1)检查的结果，包含是否为SLR(1)文法和冲突列表
 */
struct SLRCheckResult
{
    bool                 isSLR1 = false;  ///< 是否为SLR(1)文法
    QVector<SLRConflict> conflicts;       ///< 冲突列表
};

/**
 * @brief SLR(1)分析器类
 * @details 用于检查文法是否为SLR(1)文法
 */
class SLR
{
   public:
    /**
     * @brief 检查文法是否为SLR(1)文法
     * @param g 语法规则
     * @param ll1 LL(1)分析器信息，包含FIRST集和FOLLOW集
     * @return SLR检查结果
     * @details 检查给定文法是否为SLR(1)文法，返回检查结果和冲突信息
     */
    static SLRCheckResult check(const Grammar& g, const LL1Info& ll1);
};