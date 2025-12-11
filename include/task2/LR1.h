/*
 * @file LR1.h
 * @id LR1-h
 * @brief LR(1)语法分析器头文件
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
 * @brief LR(1)项目结构体
 * @details 表示LR(1)分析中的一个项目，包含产生式左部、右部、点的位置和前瞻符号
 */
struct LR1Item
{
    QString          left;       ///< 产生式左部非终结符
    QVector<QString> right;      ///< 产生式右部符号序列
    int              dot = 0;    ///< 点的位置，范围为0到right.size()
    QString          lookahead;  ///< 前瞻符号，用于决定归约动作

    /**
     * @brief 比较两个LR(1)项目是否相等
     * @param o 另一个LR(1)项目
     * @return 相等返回true，否则返回false
     */
    bool operator==(const LR1Item& o) const
    {
        return left == o.left && right == o.right && dot == o.dot && lookahead == o.lookahead;
    }
};

/**
 * @brief LR(1)状态图结构体
 * @details 存储LR(1)分析的状态图，包含所有状态和状态之间的转移边
 */
struct LR1Graph
{
    QVector<QVector<LR1Item>>     states;  ///< 状态列表，每个状态是LR(1)项目的集合
    QMap<int, QMap<QString, int>> edges;   ///< 状态转移边，从状态i通过符号s转移到状态j
};

/**
 * @brief LR(1)动作表结构体
 * @details 存储LR(1)分析表，包含动作表、跳转表和归约信息
 */
struct LR1ActionTable
{
    QMap<int, QMap<QString, QString>> action;      ///< 动作表，状态+输入符号→动作
    QMap<int, QMap<QString, int>>     gotoTable;   ///< 跳转表，状态+非终结符→下一个状态
    QVector<QPair<int, QString>>      reductions;  ///< 归约信息列表
};

/**
 * @brief LR(1)状态图和动作表构建器类
 * @details 用于构建LR(1)状态图、生成DOT格式的可视化描述和计算动作表
 */
class LR1Builder
{
   public:
    /**
     * @brief 根据语法规则构建LR(1)状态图
     * @param g 语法规则
     * @return LR(1)状态图
     * @details 计算语法规则的LR(1)项目集规范族和状态转移
     */
    static LR1Graph build(const Grammar& g);

    /**
     * @brief 将LR(1)状态图转换为DOT格式
     * @param gr LR(1)状态图
     * @return DOT格式的字符串
     * @details 生成用于可视化LR(1)状态图的DOT语言描述
     */
    static QString toDot(const LR1Graph& gr);

    /**
     * @brief 根据LR(1)状态图计算动作表
     * @param g 语法规则
     * @param gr LR(1)状态图
     * @return LR(1)动作表
     * @details 计算LR(1)分析的动作表和跳转表
     */
    static LR1ActionTable computeActionTable(const Grammar& g, const LR1Graph& gr);
};
