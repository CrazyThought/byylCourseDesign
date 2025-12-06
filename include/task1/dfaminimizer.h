#ifndef DFAMINIMIZER_H
#define DFAMINIMIZER_H

#include <QString>
#include <QSet>
#include <QList>
#include <QMap>
#include "dfabuilder.h" // 包含DFA的定义

class DFAMinimizer
{
public:
    DFAMinimizer();
    ~DFAMinimizer();
    
    // 使用Hopcroft算法最小化DFA
    DFA minimizeDFA(const DFA &dfa);
    
    // 获取错误信息
    QString getErrorMessage() const;
    
private:
    // Hopcroft算法的实现
    QList<QSet<DFAState>> hopcroftAlgorithm(const DFA &dfa);
    
    // 计算在输入字符下的状态集合的移动
    QSet<DFAState> move(const DFA &dfa, const QSet<DFAState> &states, const QString &input);
    
    // 检查状态是否在接受状态集合中
    bool isAcceptState(const DFA &dfa, DFAState state);
    
    // 辅助函数：获取状态集合的代表
    DFAState getSetRepresentative(const QSet<DFAState> &states);
    
    // 辅助函数：创建新的DFA状态映射
    QMap<DFAState, DFAState> createStateMap(const QList<QSet<DFAState>> &partitions);
    
    // 错误信息
    QString m_errorMessage;
};

#endif // DFAMINIMIZER_H
