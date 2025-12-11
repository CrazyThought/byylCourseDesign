/*
 * @file dfaminimizer.h
 * @id dfaminimizer-h
 * @brief 实现DFA最小化功能，使用Hopcroft算法将DFA转换为等价的最小DFA
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#ifndef DFAMINIMIZER_H
#define DFAMINIMIZER_H

#include <QString>
#include <QSet>
#include <QList>
#include <QMap>
#include "dfabuilder.h" // 包含DFA的定义

/**
 * @brief DFA最小化器类
 * 
 * 使用Hopcroft算法实现DFA的最小化，将DFA转换为等价的最小DFA
 */
class DFAMinimizer
{
public:
    /**
     * @brief 构造函数
     */
    DFAMinimizer();
    
    /**
     * @brief 析构函数
     */
    ~DFAMinimizer();
    
    /**
     * @brief 使用Hopcroft算法最小化DFA
     * 
     * @param dfa 要最小化的DFA
     * @return DFA 最小化后的DFA
     */
    DFA minimizeDFA(const DFA &dfa);
    
    /**
     * @brief 获取错误信息
     * 
     * @return QString 错误信息
     */
    QString getErrorMessage() const;
    
    /**
     * @brief 存储正则表达式与接受态之间的映射关系
     * 
     * key: 正则表达式索引，value: 该正则表达式对应的所有接受态
     */
    QMap<int, QSet<DFAState>> regexToAcceptStatesMap;
    
private:
    /**
     * @brief Hopcroft算法的实现
     * 
     * 使用Hopcroft算法对DFA进行最小化分区
     * 
     * @param dfa 输入DFA
     * @return QList<QSet<DFAState>> 最小化后的状态分区
     */
    QList<QSet<DFAState>> hopcroftAlgorithm(const DFA &dfa);
    
    /**
     * @brief 计算在输入字符下的状态集合的移动
     * 
     * @param dfa 输入DFA
     * @param states 状态集合
     * @param input 输入字符
     * @return QSet<DFAState> 移动后的状态集合
     */
    QSet<DFAState> move(const DFA &dfa, const QSet<DFAState> &states, const QString &input);
    
    /**
     * @brief 检查状态是否在接受状态集合中
     * 
     * @param dfa 输入DFA
     * @param state 要检查的状态
     * @return bool 是接受状态返回true，否则返回false
     */
    bool isAcceptState(const DFA &dfa, DFAState state);
    
    /**
     * @brief 辅助函数：获取状态集合的代表
     * 
     * @param states 状态集合
     * @return DFAState 状态集合的代表状态
     */
    DFAState getSetRepresentative(const QSet<DFAState> &states);
    
    /**
     * @brief 辅助函数：创建新的DFA状态映射
     * 
     * @param partitions 状态分区
     * @return QMap<DFAState, DFAState> 新的状态映射
     */
    QMap<DFAState, DFAState> createStateMap(const QList<QSet<DFAState>> &partitions);
    
    /**
     * @brief 增强Hopcroft算法以处理可选符号的语义限制
     * 
     * 对Hopcroft算法进行扩展，以处理可选符号的语义限制
     * 
     * @param dfa 输入DFA
     * @param partitions 初始状态分区
     * @return QList<QSet<DFAState>> 增强后的状态分区
     */
    QList<QSet<DFAState>> enhanceHopcroftForOptionalSymbols(const DFA &dfa, const QList<QSet<DFAState>> &partitions);
    
    /**
     * @brief 错误信息
     */
    QString m_errorMessage;
};

#endif // DFAMINIMIZER_H
