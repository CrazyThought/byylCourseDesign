/*
 * @file LR0.h
 * @id LR0-h
 * @brief LR(0)语法分析器头文件
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

/**
 * @brief LR(0)项目结构体
 * @details 表示LR(0)分析中的一个项目，包含产生式左部、右部和点的位置
 */
struct LR0Item
{
    QString          left;       ///< 产生式左部非终结符
    QVector<QString> right;      ///< 产生式右部符号序列
    int              dot = 0;    ///< 点的位置，范围为0到right.size()

    /**
     * @brief 比较两个LR(0)项目是否相等
     * @param o 另一个LR(0)项目
     * @return 相等返回true，否则返回false
     */
    bool operator==(const LR0Item& o) const
    {
        return left == o.left && right == o.right && dot == o.dot;
    }
};

/**
 * @brief LR(0)状态图结构体
 * @details 存储LR(0)分析的状态图，包含所有状态和状态之间的转移边
 */
struct LR0Graph
{
    QVector<QVector<LR0Item>>     states;  ///< 状态列表，每个状态是LR(0)项目的集合
    QMap<int, QMap<QString, int>> edges;   ///< 状态转移边，从状态i通过符号s转移到状态j
};

/**
 * @brief LR(0)状态图构建器类
 * @details 用于构建LR(0)状态图和生成DOT格式的可视化描述
 */
class LR0Builder
{
   public:
    /**
     * @brief 根据语法规则构建LR(0)状态图
     * @param g 语法规则
     * @return LR(0)状态图
     * @details 计算语法规则的LR(0)项目集规范族和状态转移
     */
    static LR0Graph build(const Grammar& g);

    /**
     * @brief 将LR(0)状态图转换为DOT格式
     * @param gr LR(0)状态图
     * @return DOT格式的字符串
     * @details 生成用于可视化LR(0)状态图的DOT语言描述
     */
    static QString toDot(const LR0Graph& gr);
};