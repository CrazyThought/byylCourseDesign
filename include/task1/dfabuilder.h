#ifndef DFABUILDER_H
#define DFABUILDER_H

#include <QString>
#include <QList>
#include <QSet>
#include <QMap>
#include "nfabuilder.h"

// DFA状态
typedef int DFAState;

// DFA转换
typedef struct {
    DFAState fromState;
    QString input;
    DFAState toState;
} DFATransition;

// DFA结构
typedef struct {
    QList<DFAState> states;
    QSet<QString> alphabet;
    QList<DFATransition> transitions;
    DFAState startState;
    QSet<DFAState> acceptStates;
    // 状态映射：NFA状态列表 -> DFA状态
    QMap<QList<NFAState>, DFAState> stateMap;
    // 接受状态到正则表达式索引的映射
    QMap<DFAState, int> acceptStateToRegexIndex;
} DFA;

class DFABuilder
{
public:
    DFABuilder();
    ~DFABuilder();

    // 将NFA转换为DFA
    DFA convertNFAToDFA(const NFA &nfa);

    // 获取错误信息
    QString getErrorMessage() const;

private:
    // 计算ε-闭包
    QSet<NFAState> epsilonClosure(const NFA &nfa, const QSet<NFAState> &states);

    // 计算状态集合在输入字符下的移动
    QSet<NFAState> move(const NFA &nfa, const QSet<NFAState> &states, const QString &input);

    // 获取下一个可用的DFA状态
    DFAState getNextState();

    // 重置状态计数器
    void resetStateCounter();

private:
    DFAState m_nextState;
    QString m_errorMessage;
};

#endif // DFABUILDER_H