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
        
        // 构建新的stateMap：收集该等价类中所有原始DFA状态对应的NFA状态集合
        QSet<NFAState> combinedNfaStates;
        
        // 创建临时映射来快速查找原始DFA状态对应的NFA状态集合
        QMap<DFAState, QList<NFAState>> tempDfaToNfaMap;
        for (auto it = dfa.stateMap.constBegin(); it != dfa.stateMap.constEnd(); ++it) {
            tempDfaToNfaMap[it.value()] = it.key();
        }
        
        for (DFAState originalState : partitions[i]) {
            // 如果找到了对应的NFA状态集合，就合并到combinedNfaStates中
            if (tempDfaToNfaMap.contains(originalState)) {
                const QList<NFAState> &nfaStates = tempDfaToNfaMap[originalState];
                for (NFAState nfaState : nfaStates) {
                    combinedNfaStates.insert(nfaState);
                }
            }
        }
        
        // 将合并后的NFA状态集合转换为列表并排序
        QList<NFAState> combinedNfaStatesList = combinedNfaStates.values();
        std::sort(combinedNfaStatesList.begin(), combinedNfaStatesList.end());
        
        // 设置最小化DFA的stateMap
        minimizedDFA.stateMap[combinedNfaStatesList] = i;
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
    
    // 构建邻接表来加速转换查找
    QHash<DFAState, QHash<QString, DFAState>> transitionTable;
    for (const DFATransition &t : dfa.transitions) {
        transitionTable[t.fromState][t.input] = t.toState;
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
            
            // 使用邻接表快速查找转换
            if (transitionTable.contains(representative) && transitionTable[representative].contains(input)) {
                DFAState originalToState = transitionTable[representative][input];
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

// 为DFA构建逆向邻接表（用于Hopcroft算法）
QHash<QString, QHash<DFAState, QSet<DFAState>>> buildReverseTransitionTable(const DFA &dfa)
{
    QHash<QString, QHash<DFAState, QSet<DFAState>>> reverseTable;
    
    for (const DFATransition &transition : dfa.transitions) {
        reverseTable[transition.input][transition.toState].insert(transition.fromState);
    }
    
    return reverseTable;
}

// 为DFA构建正向邻接表
QHash<DFAState, QHash<QString, DFAState>> buildForwardTransitionTable(const DFA &dfa)
{
    QHash<DFAState, QHash<QString, DFAState>> forwardTable;
    
    for (const DFATransition &transition : dfa.transitions) {
        forwardTable[transition.fromState][transition.input] = transition.toState;
    }
    
    return forwardTable;
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
    
    // 构建逆向邻接表：input -> (toState -> fromStates)
    QHash<QString, QHash<DFAState, QSet<DFAState>>> reverseTransitionTable = buildReverseTransitionTable(dfa);
    
    // 开始划分过程
    while (!W.isEmpty()) {
        QSet<DFAState> A = W.takeFirst();
        
        // 对每个输入字符
        for (const QString &c : dfa.alphabet) {
            if (c == "#") continue; // 跳过空串
            
            // 计算所有通过c转换到A中状态的状态集合X
            QSet<DFAState> X;
            
            // 使用逆向邻接表快速查找
            if (reverseTransitionTable.contains(c)) {
                const QHash<DFAState, QSet<DFAState>> &transitions = reverseTransitionTable[c];
                for (DFAState toState : A) {
                    if (transitions.contains(toState)) {
                        X.unite(transitions[toState]);
                    }
                }
            }
            
            if (X.isEmpty()) continue;
            
            // 创建一个临时列表来存储新的划分
            QList<QSet<DFAState>> tempP;
            
            // 遍历当前P的副本，避免在循环中修改正在迭代的集合
            for (const QSet<DFAState> &Y : P) {
                QSet<DFAState> Y1; // Y ∩ X
                QSet<DFAState> Y2; // Y \ X
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
    
    // 特殊处理：对于可选符号（如 '+' 和 '-'），确保它们不会在同一个状态中重复出现
    // 这是对标准Hopcroft算法的增强，用于处理正则表达式中的?量词语义
    P = enhanceHopcroftForOptionalSymbols(dfa, P);
    
    return P;
}

QList<QSet<DFAState>> DFAMinimizer::enhanceHopcroftForOptionalSymbols(const DFA &dfa, const QList<QSet<DFAState>> &partitions)
{
    QList<QSet<DFAState>> enhancedPartitions = partitions;
    
    // 检查是否存在可选符号问题
    // 对于正则表达式 (\+|\-)?digit+，我们需要确保状态不能同时接收 '+' 和 '-' 并转换到自身
    
    // 识别可能包含可选符号问题的状态
    QSet<QString> optionalSymbols = {"+", "-"};
    
    // 构建正向邻接表加速转换查找
    QHash<DFAState, QHash<QString, DFAState>> transitionTable = buildForwardTransitionTable(dfa);
    
    // 关键修复：检查每个划分中的状态是否具有语义冲突
    for (int i = 0; i < enhancedPartitions.size(); ++i) {
        const QSet<DFAState> &partition = enhancedPartitions[i];
        
        if (partition.size() <= 1) continue; // 单个状态不需要划分
        
        // 检查该划分中的状态是否具有语义冲突
        // 冲突条件：状态可以接收可选符号并转换到自身，或者状态可以接收可选符号但转换到不同的状态
        QSet<DFAState> statesWithOptionalTransitions;
        QSet<DFAState> statesWithoutOptionalTransitions;
        
        for (DFAState state : partition) {
            bool hasOptionalTransition = false;
            
            // 检查状态是否有任何可选符号的转换
            for (const QString &symbol : optionalSymbols) {
                if (transitionTable.contains(state) && transitionTable[state].contains(symbol)) {
                    hasOptionalTransition = true;
                    break;
                }
            }
            
            if (hasOptionalTransition) {
                statesWithOptionalTransitions.insert(state);
            } else {
                statesWithoutOptionalTransitions.insert(state);
            }
        }
        
        // 如果划分中包含有可选符号转换和无可选符号转换的状态，需要进一步划分
        if (!statesWithOptionalTransitions.isEmpty() && !statesWithoutOptionalTransitions.isEmpty()) {
            enhancedPartitions.removeAt(i);
            enhancedPartitions.insert(i, statesWithOptionalTransitions);
            enhancedPartitions.insert(i + 1, statesWithoutOptionalTransitions);
            i++; // 跳过新添加的划分
        }
    }
    
    // 进一步处理：确保接受状态不会与具有可选符号自循环的状态合并
    for (int i = 0; i < enhancedPartitions.size(); ++i) {
        const QSet<DFAState> &partition = enhancedPartitions[i];
        
        if (partition.size() <= 1) continue;
        
        // 检查划分中是否同时包含接受状态和具有可选符号自循环的状态
        bool hasAcceptState = false;
        bool hasOptionalSelfLoopState = false;
        QSet<DFAState> acceptStates;
        QSet<DFAState> nonAcceptStates;
        
        for (DFAState state : partition) {
            if (dfa.acceptStates.contains(state)) {
                hasAcceptState = true;
                acceptStates.insert(state);
            } else {
                nonAcceptStates.insert(state);
            }
            
            // 检查是否有可选符号的自循环
            for (const QString &symbol : optionalSymbols) {
                if (transitionTable.contains(state) && transitionTable[state].contains(symbol)) {
                    if (transitionTable[state][symbol] == state) {
                        hasOptionalSelfLoopState = true;
                        break;
                    }
                }
            }
        }
        
        // 如果接受状态与具有可选符号自循环的状态在同一划分中，需要分离
        if (hasAcceptState && hasOptionalSelfLoopState && !acceptStates.isEmpty() && !nonAcceptStates.isEmpty()) {
            enhancedPartitions.removeAt(i);
            enhancedPartitions.insert(i, acceptStates);
            enhancedPartitions.insert(i + 1, nonAcceptStates);
            i++; // 跳过新添加的划分
        }
    }
    
    return enhancedPartitions;
}

QSet<DFAState> DFAMinimizer::move(const DFA &dfa, const QSet<DFAState> &states, const QString &input)
{
    QSet<DFAState> result;
    
    // 构建邻接表来加速转换查找
    QHash<DFAState, QHash<QString, DFAState>> transitionTable;
    for (const DFATransition &t : dfa.transitions) {
        transitionTable[t.fromState][t.input] = t.toState;
    }
    
    for (DFAState state : states) {
        if (transitionTable.contains(state) && transitionTable[state].contains(input)) {
            result.insert(transitionTable[state][input]);
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
