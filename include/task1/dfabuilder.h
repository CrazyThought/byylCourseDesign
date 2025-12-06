#ifndef DFABUILDER_H
#define DFABUILDER_H

#include <QString>
#include <QList>
#include <QSet>
#include <QMap>
#include "nfabuilder.h"

// DFA状态
typedef int DFAState;

// NFA状态
typedef int NFAState;

/**
 * DFA转换规则结构体
 * 描述从一个状态通过特定输入字符转移到另一个状态的规则
 */
typedef struct {
    DFAState fromState;  // 起始状态
    QString input;       // 输入字符
    DFAState toState;    // 目标状态
} DFATransition;

/**
 * 确定有限自动机(DFA)结构体
 * DFA是一种特殊的有限自动机，对于每个状态和输入字符，只有一种转换可能
 */
typedef struct {
    QList<DFAState> states;                  // 所有状态
    QSet<QString> alphabet;                 // 字母表
    QList<DFATransition> transitions;      // 转换规则
    DFAState startState;                   // 起始状态
    QSet<DFAState> acceptStates;           // 接受状态集合
    // 状态映射：NFA状态列表 -> DFA状态
    QMap<QList<NFAState>, DFAState> stateMap;
    // 接受状态到正则表达式索引的映射
    QMap<DFAState, int> acceptStateToRegexIndex;
} DFA;

/**
 * DFA构建器类
 * 实现从NFA到DFA的转换，使用子集构造法
 * 主要功能包括：
 * 1. ε-闭包计算
 * 2. move操作计算
 * 3. NFA到DFA的完整转换
 */
class DFABuilder
{
public:
    DFABuilder();
    ~DFABuilder();

    /**
     * 将NFA转换为DFA
     * 使用子集构造法实现转换
     * 
     * @param nfa 输入的NFA
     * @return 转换后的DFA
     */
    DFA convertNFAToDFA(const NFA &nfa);
    
    /**
     * 获取错误信息
     * 
     * @return 错误信息字符串
     */
    QString getErrorMessage() const;

private:
    /**
     * 计算ε-闭包
     * 
     * @param nfa NFA对象
     * @param states 起始状态集合
     * @return ε-闭包结果集合
     */
    QSet<NFAState> epsilonClosure(const NFA &nfa, const QSet<NFAState> &states);
    
    /**
     * 计算move操作
     * 
     * @param nfa NFA对象
     * @param states 起始状态集合
     * @param input 输入字符
     * @return 通过输入字符可达的状态集合
     */
    QSet<NFAState> move(const NFA &nfa, const QSet<NFAState> &states, const QString &input);
    
    /**
     * 获取下一个可用的DFA状态编号
     * 
     * @return 新的状态编号
     */
    DFAState getNextState();
    
    /**
     * 重置状态计数器
     */
    void resetStateCounter();

    DFAState m_nextState;      // 下一个可用的状态编号
    QString m_errorMessage;    // 错误信息
};

#endif // DFABUILDER_H