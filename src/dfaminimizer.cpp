#include "dfaminimizer.h"
#include <QDebug>

DFAMinimizer::DFAMinimizer()
{
}

DFAMinimizer::~DFAMinimizer()
{
}

DFA DFAMinimizer::minimizeDFA(const DFA &dfa)
{
    m_errorMessage.clear();
    
    // 如果DFA为空，直接返回
    if (dfa.states.isEmpty()) {
        m_errorMessage = "输入DFA为空";
        return DFA();
    }
    
    // 使用Hopcroft算法获取等价类划分
    QList<QSet<DFAState>> partitions = hopcroftAlgorithm(dfa);
    
    if (partitions.isEmpty()) {
        m_errorMessage = "等价类划分失败";
        return DFA();
    }
    
    // 创建状态映射：旧状态 -> 新状态
    QMap<DFAState, DFAState> stateMap = createStateMap(partitions);
    
    // 构建最小化的DFA
    DFA minimizedDFA;
    minimizedDFA.alphabet = dfa.alphabet;
    
    // 设置新的起始状态
    minimizedDFA.startState = stateMap[dfa.startState];
    
    // 设置新的状态集合
    for (int i = 0; i < partitions.size(); ++i) {
        minimizedDFA.states.append(i);
    }
    
    // 设置新的接受状态集合
    for (DFAState state : dfa.acceptStates) {
        minimizedDFA.acceptStates.insert(stateMap[state]);
    }
    
    // 构建新的转换表
    QSet<QString> processedTransitions;
    
    for (const DFATransition &transition : dfa.transitions) {
        DFAState from = stateMap[transition.fromState];
        DFAState to = stateMap[transition.toState];
        QString key = QString::number(from) + "|" + transition.input + "|" + QString::number(to);
        
        if (!processedTransitions.contains(key)) {
            DFATransition newTransition;
            newTransition.fromState = from;
            newTransition.input = transition.input;
            newTransition.toState = to;
            minimizedDFA.transitions.append(newTransition);
            processedTransitions.insert(key);
        }
    }
    
    return minimizedDFA;
}

QString DFAMinimizer::getErrorMessage() const
{
    return m_errorMessage;
}

QList<QSet<DFAState>> DFAMinimizer::hopcroftAlgorithm(const DFA &dfa)
{
    QList<QSet<DFAState>> P; // 划分集合
    QList<QSet<DFAState>> W; // 工作集合
    
    // 初始划分：接受状态和非接受状态
    QSet<DFAState> F = dfa.acceptStates;
    QSet<DFAState> Q_F(dfa.states.begin(), dfa.states.end());
    QSet<DFAState> tempF = F;
    Q_F.subtract(tempF);
    
    if (!F.isEmpty()) {
        P.append(F);
        W.append(F);
    }
    if (!Q_F.isEmpty()) {
        P.append(Q_F);
        W.append(Q_F);
    }
    
    // 开始划分过程
    while (!W.isEmpty()) {
        QSet<DFAState> A = W.takeFirst();
        
        // 对每个输入字符
        for (const QString &c : dfa.alphabet) {
            if (c == "#") continue; // 跳过空串
            
            // 计算所有通过c转换到A中状态的状态集合
            QSet<DFAState> X;
            
            for (const DFATransition &transition : dfa.transitions) {
                if (transition.input == c && A.contains(transition.toState)) {
                    X.insert(transition.fromState);
                }
            }
            
            if (X.isEmpty()) continue;
            
            // 对P中的每个集合Y
            QList<QSet<DFAState>> newP;
            
            for (const QSet<DFAState> &Y : P) {
                QSet<DFAState> Y1;
                QSet<DFAState> Y2;
                for (DFAState state : Y) {
                    if (X.contains(state)) {
                        Y1.insert(state);
                    } else {
                        Y2.insert(state);
                    }
                }
                
                if (!Y1.isEmpty() && !Y2.isEmpty()) {
                    newP.append(Y1);
                    newP.append(Y2);
                    
                    // 更新工作集合W
                    if (W.contains(Y)) {
                        W.removeAll(Y);
                        W.append(Y1);
                        W.append(Y2);
                    } else {
                        if (Y1.size() <= Y2.size()) {
                            W.append(Y1);
                        } else {
                            W.append(Y2);
                        }
                    }
                } else {
                    newP.append(Y);
                }
            }
            
            P = newP;
        }
    }
    
    return P;
}

QSet<DFAState> DFAMinimizer::move(const DFA &dfa, const QSet<DFAState> &states, const QString &input)
{
    QSet<DFAState> result;
    
    for (DFAState state : states) {
        for (const DFATransition &transition : dfa.transitions) {
            if (transition.fromState == state && transition.input == input) {
                result.insert(transition.toState);
            }
        }
    }
    
    return result;
}

bool DFAMinimizer::isAcceptState(const DFA &dfa, DFAState state)
{
    return dfa.acceptStates.contains(state);
}

DFAState DFAMinimizer::getSetRepresentative(const QSet<DFAState> &states)
{
    if (states.isEmpty()) {
        return -1;
    }
    
    return *states.constBegin();
}

QMap<DFAState, DFAState> DFAMinimizer::createStateMap(const QList<QSet<DFAState>> &partitions)
{
    QMap<DFAState, DFAState> stateMap;
    
    for (int i = 0; i < partitions.size(); ++i) {
        for (DFAState state : partitions[i]) {
            stateMap[state] = i;
        }
    }
    
    return stateMap;
}
