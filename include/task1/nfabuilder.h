/*
 * @file nfabuilder.h
 * @id nfabuilder-h
 * @brief 实现NFA（非确定性有限自动机）构建功能，基于Thompson构造法从正则表达式生成NFA
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#ifndef NFABUILDER_H
#define NFABUILDER_H

#include <QString>
#include <QList>
#include <QSet>
#include <QMap>
#include <QHash>
#include "regexprocessor.h"

/**
 * @brief NFA状态类型
 * 
 * 使用整数表示NFA的状态
 */
typedef int NFAState;

/**
 * @brief NFA转换结构体
 * 
 * 表示NFA的单个状态转换
 */
typedef struct {
    NFAState fromState;  ///< 起始状态
    QString input;       ///< 输入符号，空字符串表示epsilon转换
    NFAState toState;    ///< 目标状态
} NFATransition;

/**
 * @brief NFA结构
 * 
 * 表示非确定性有限自动机
 */
typedef struct {
    QList<NFAState> states;               ///< 所有状态列表
    QSet<QString> alphabet;               ///< 字母表，所有可能的输入符号
    QList<NFATransition> transitions;     ///< 转换规则列表
    NFAState startState;                  ///< 起始状态
    QSet<NFAState> acceptStates;          ///< 接受状态集合
    
    /**
     * @brief 邻接表：state -> (symbol -> state集合)，用于优化算法
     * 
     * 优化后的转换表，提高状态转移查询效率
     */
    QHash<NFAState, QHash<QString, QSet<NFAState>>> transitionTable;
    
    /**
     * @brief 接受状态到正则表达式索引的映射
     * 
     * 用于标识哪个正则表达式匹配成功
     */
    QMap<NFAState, int> acceptStateToRegexIndex;
} NFA;

/**
 * @brief NFA构建器类
 * 
 * 基于Thompson构造法从正则表达式构建NFA
 */
class NFABuilder
{
public:
    /**
     * @brief 构造函数
     */
    NFABuilder();
    
    /**
     * @brief 析构函数
     */
    ~NFABuilder();

    /**
     * @brief 从正则表达式构建NFA
     * 
     * @param regexItem 正则表达式项
     * @return NFA 构建的NFA
     */
    NFA buildNFA(const RegexItem &regexItem);

    /**
     * @brief 获取构建过程中的错误信息
     * 
     * @return QString 错误信息
     */
    QString getErrorMessage() const;

private:
    /**
     * @brief Thompson构造法：构建基本NFA（单个字符）
     * 
     * @param input 输入字符
     * @return NFA 构建的基本NFA
     */
    NFA buildBasicNFA(const QString &input);

    /**
     * @brief Thompson构造法：连接操作
     * 
     * 构建两个NFA的连接操作
     * 
     * @param nfa1 第一个NFA
     * @param nfa2 第二个NFA
     * @return NFA 连接后的NFA
     */
    NFA buildConcatenationNFA(const NFA &nfa1, const NFA &nfa2);

    /**
     * @brief Thompson构造法：选择操作（|）
     * 
     * 构建两个NFA的选择操作
     * 
     * @param nfa1 第一个NFA
     * @param nfa2 第二个NFA
     * @return NFA 选择后的NFA
     */
    NFA buildChoiceNFA(const NFA &nfa1, const NFA &nfa2);

    /**
     * @brief Thompson构造法：闭包操作（*）
     * 
     * 构建NFA的闭包操作
     * 
     * @param nfa 原始NFA
     * @return NFA 闭包后的NFA
     */
    NFA buildClosureNFA(const NFA &nfa);

    /**
     * @brief Thompson构造法：正闭包操作（+）
     * 
     * 构建NFA的正闭包操作
     * 
     * @param nfa 原始NFA
     * @return NFA 正闭包后的NFA
     */
    NFA buildPositiveClosureNFA(const NFA &nfa);

    /**
     * @brief Thompson构造法：可选操作（?）
     * 
     * 构建NFA的可选操作
     * 
     * @param nfa 原始NFA
     * @return NFA 可选操作后的NFA
     */
    NFA buildOptionalNFA(const NFA &nfa);

    /**
     * @brief 处理字符集
     * 
     * 构建字符集的NFA
     * 
     * @param charSet 字符集
     * @return NFA 字符集的NFA
     */
    NFA buildCharacterSetNFA(const QString &charSet);

    /**
     * @brief 解析正则表达式并构建NFA
     * 
     * 递归下降解析正则表达式并构建NFA
     * 
     * @param regex 正则表达式字符串
     * @return NFA 构建的NFA
     */
    NFA parseRegex(const QString &regex);

    /**
     * @brief 解析选择操作
     * 
     * @param regex 正则表达式字符串
     * @param pos 当前解析位置，会被修改
     * @param error 错误信息输出
     * @return NFA 选择操作的NFA
     */
    NFA parseChoice(const QString &regex, int &pos, QString &error);

    /**
     * @brief 解析连接操作
     * 
     * @param regex 正则表达式字符串
     * @param pos 当前解析位置，会被修改
     * @param error 错误信息输出
     * @return NFA 连接操作的NFA
     */
    NFA parseConcatenation(const QString &regex, int &pos, QString &error);

    /**
     * @brief 解析闭包操作
     * 
     * @param regex 正则表达式字符串
     * @param pos 当前解析位置，会被修改
     * @param error 错误信息输出
     * @return NFA 闭包操作的NFA
     */
    NFA parseClosure(const QString &regex, int &pos, QString &error);

    /**
     * @brief 解析基本元素
     * 
     * @param regex 正则表达式字符串
     * @param pos 当前解析位置，会被修改
     * @param error 错误信息输出
     * @return NFA 基本元素的NFA
     */
    NFA parsePrimary(const QString &regex, int &pos, QString &error);

    /**
     * @brief 获取下一个可用状态
     * 
     * @return NFAState 下一个可用状态
     */
    NFAState getNextState();

    /**
     * @brief 重置状态计数器
     */
    void resetStateCounter();
    
    /**
     * @brief 构建邻接表
     * 
     * 从转换规则列表构建优化的邻接表
     * 
     * @param nfa 要构建邻接表的NFA，会被修改
     */
    void buildTransitionTable(NFA &nfa);

private:
    NFAState m_nextState;  ///< 下一个可用状态计数器
    QString m_errorMessage;  ///< 错误信息
};

#endif // NFABUILDER_H