#ifndef NFABUILDER_H
#define NFABUILDER_H

#include <QString>
#include <QList>
#include <QSet>
#include <QMap>
#include <QHash>
#include "regexprocessor.h"

// NFA状态
typedef int NFAState;

// NFA转换
typedef struct {
    NFAState fromState;
    QString input;
    NFAState toState;
} NFATransition;

// NFA结构
typedef struct {
    QList<NFAState> states;               // 所有状态
    QSet<QString> alphabet;               // 字母表
    QList<NFATransition> transitions;     // 转换规则
    NFAState startState;                  // 起始状态
    QSet<NFAState> acceptStates;          // 接受状态集合
    
    // 邻接表：state -> (symbol -> state集合)，用于优化算法
    QHash<NFAState, QHash<QString, QSet<NFAState>>> transitionTable;
} NFA;

class NFABuilder
{
public:
    NFABuilder();
    ~NFABuilder();

    // 从正则表达式构建NFA
    NFA buildNFA(const RegexItem &regexItem);

    // 获取构建过程中的错误信息
    QString getErrorMessage() const;

private:
    // Thompson构造法：构建基本NFA（单个字符）
    NFA buildBasicNFA(const QString &input);

    // Thompson构造法：连接操作
    NFA buildConcatenationNFA(const NFA &nfa1, const NFA &nfa2);

    // Thompson构造法：选择操作（|）
    NFA buildChoiceNFA(const NFA &nfa1, const NFA &nfa2);

    // Thompson构造法：闭包操作（*）
    NFA buildClosureNFA(const NFA &nfa);

    // Thompson构造法：正闭包操作（+）
    NFA buildPositiveClosureNFA(const NFA &nfa);

    // Thompson构造法：可选操作（?）
    NFA buildOptionalNFA(const NFA &nfa);

    // 处理字符集
    NFA buildCharacterSetNFA(const QString &charSet);

    // 解析正则表达式并构建NFA
    NFA parseRegex(const QString &regex);

    // 解析选择操作
    NFA parseChoice(const QString &regex, int &pos, QString &error);

    // 解析连接操作
    NFA parseConcatenation(const QString &regex, int &pos, QString &error);

    // 解析闭包操作
    NFA parseClosure(const QString &regex, int &pos, QString &error);

    // 解析基本元素
    NFA parsePrimary(const QString &regex, int &pos, QString &error);

    // 获取下一个可用状态
    NFAState getNextState();

    // 重置状态计数器
    void resetStateCounter();
    
    // 构建邻接表
    void buildTransitionTable(NFA &nfa);

private:
    NFAState m_nextState;
    QString m_errorMessage;
};

#endif // NFABUILDER_H