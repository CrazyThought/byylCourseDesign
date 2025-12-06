#include "task1/dfaminimizer.h"
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
    
    // 设置新的起始状态，添加边界检查
    if (!stateMap.contains(dfa.startState)) {
        m_errorMessage = QString("起始状态 %1 不在状态映射中").arg(dfa.startState);
        return DFA();
    }
    minimizedDFA.startState = stateMap[dfa.startState];
    
    // 设置新的状态集合
    for (int i = 0; i < partitions.size(); ++i) {
        minimizedDFA.states.append(i);
    }
    
    // 设置新的接受状态集合，添加边界检查
    for (DFAState state : dfa.acceptStates) {
        if (stateMap.contains(state)) {
            minimizedDFA.acceptStates.insert(stateMap[state]);
        } else {
            // 记录错误，但继续执行
            m_errorMessage += QString("警告：接受状态 %1 不在状态映射中\n").arg(state);
        }
    }
    
    // 构建新的转换表
    QSet<QString> processedTransitions;
    
    // 遍历所有等价类和输入字符，确保生成所有必要的转换
    for (int i = 0; i < partitions.size(); ++i) {
        DFAState newState = i;
        
        // 选择等价类中的一个代表状态
        DFAState representative = *partitions[i].begin();
        
        // 为每个输入字符生成转换
        for (const QString &input : dfa.alphabet) {
            if (input == "#") continue;
            
            // 找到代表状态的原始转换
            DFAState originalToState = -1;
            for (const DFATransition &t : dfa.transitions) {
                if (t.fromState == representative && t.input == input) {
                    originalToState = t.toState;
                    break;
                }
            }
            
            if (originalToState != -1) {
                // 获取转换后的新状态
                DFAState newToState = stateMap[originalToState];
                
                // 检查是否已经处理过该转换
                QString key = QString::number(newState) + "|" + input + "|" + QString::number(newToState);
                if (!processedTransitions.contains(key)) {
                    DFATransition newTransition;
                    newTransition.fromState = newState;
                    newTransition.input = input;
                    newTransition.toState = newToState;
                    minimizedDFA.transitions.append(newTransition);
                    processedTransitions.insert(key);
                }
            }
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
            
            // 创建一个临时列表来存储新的划分
            QList<QSet<DFAState>> tempP;
            
            // 遍历当前P的副本，避免在循环中修改正在迭代的集合
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
                    tempP.append(Y1);
                    tempP.append(Y2);
                    
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
                    tempP.append(Y);
                }
            }
            
            // 使用临时列表更新P
            P = tempP;
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
