#include "task1/dfabuilder.h"
#include <QDebug>
#include <algorithm>

DFABuilder::DFABuilder()
    : m_nextState(0)
{
}

DFABuilder::~DFABuilder()
{
}

DFA DFABuilder::convertNFAToDFA(const NFA &nfa)
{
    resetStateCounter();
    m_errorMessage.clear();
    
    DFA dfa;
    
    // 构建DFA的字母表：收集NFA中所有非空转换字符
    dfa.alphabet = nfa.alphabet;
    // 额外检查NFA的所有转换，确保字母表包含所有必要字符
    for (const NFATransition &transition : nfa.transitions) {
        if (transition.input != "#") {
            dfa.alphabet.insert(transition.input);
        }
    }
    
    // 计算起始状态的ε-闭包
    QSet<NFAState> startNFAStates;
    startNFAStates.insert(nfa.startState);
    QSet<NFAState> startClosure = epsilonClosure(nfa, startNFAStates);
    
    // 标记起始状态为已访问
    QMap<QList<NFAState>, DFAState> stateMap;
    QList<QList<NFAState>> unprocessedStates;
    
    // 创建起始DFA状态
    DFAState startDFAState = getNextState();
    QList<NFAState> startClosureList(startClosure.begin(), startClosure.end());
    std::sort(startClosureList.begin(), startClosureList.end());
    stateMap[startClosureList] = startDFAState;
    unprocessedStates.append(startClosureList);
    dfa.states.append(startDFAState);
    dfa.startState = startDFAState;
    
    // 检查起始状态是否为接受状态
    for (NFAState nfaState : startClosure) {
        if (nfa.acceptStates.contains(nfaState)) {
            dfa.acceptStates.insert(startDFAState);
            break;
        }
    }
    
    // 处理所有未处理的状态
    while (!unprocessedStates.isEmpty()) {
        QList<NFAState> currentNFAStatesList = unprocessedStates.takeFirst();
        DFAState currentDFAState = stateMap[currentNFAStatesList];
        QSet<NFAState> currentNFAStates(currentNFAStatesList.begin(), currentNFAStatesList.end());
        
        // 对每个输入字符
        for (const QString &input : dfa.alphabet) {
            if (input == "#") continue; // 跳过空串
            
            // 计算move(currentNFAStates, input)
            QSet<NFAState> moveResult = move(nfa, currentNFAStates, input);
            
            // 计算moveResult的ε-闭包
            QSet<NFAState> closureResult = epsilonClosure(nfa, moveResult);
            
            if (closureResult.isEmpty()) {
                continue; // 没有转换，跳过
            }
            
            DFAState targetDFAState;
            
            // 检查这个闭包是否已经映射到DFA状态
            QList<NFAState> closureResultList(closureResult.begin(), closureResult.end());
            std::sort(closureResultList.begin(), closureResultList.end());
            if (stateMap.contains(closureResultList)) {
                targetDFAState = stateMap[closureResultList];
            } else {
                // 创建新的DFA状态
                targetDFAState = getNextState();
                stateMap[closureResultList] = targetDFAState;
                unprocessedStates.append(closureResultList);
                dfa.states.append(targetDFAState);
                
                // 检查是否为接受状态
                for (NFAState nfaState : closureResult) {
                    if (nfa.acceptStates.contains(nfaState)) {
                        dfa.acceptStates.insert(targetDFAState);
                        break;
                    }
                }
            }
            
            // 添加DFA转换
            DFATransition transition;
            transition.fromState = currentDFAState;
            transition.input = input;
            transition.toState = targetDFAState;
            dfa.transitions.append(transition);
        }
    }
    
    // 确保每个状态对每个输入字符都有转换（添加遗漏的转换）
    for (DFAState state : dfa.states) {
        for (const QString &input : dfa.alphabet) {
            if (input == "#") continue;
            
            // 检查是否已经存在该转换
            bool hasTransition = false;
            for (const DFATransition &t : dfa.transitions) {
                if (t.fromState == state && t.input == input) {
                    hasTransition = true;
                    break;
                }
            }
            
            if (!hasTransition) {
                // 添加遗漏的转换（这里可以考虑添加到一个错误状态）
                DFATransition transition;
                transition.fromState = state;
                transition.input = input;
                transition.toState = state; // 暂时转换到自身，实际应该转换到错误状态
                dfa.transitions.append(transition);
            }
        }
    }
    
    // 保存状态映射
    dfa.stateMap = stateMap;
    
    return dfa;
}

QString DFABuilder::getErrorMessage() const
{
    return m_errorMessage;
}

QSet<NFAState> DFABuilder::epsilonClosure(const NFA &nfa, const QSet<NFAState> &states)
{
    QSet<NFAState> closure = states;
    QList<NFAState> toProcess(states.begin(), states.end());
    
    while (!toProcess.isEmpty()) {
        NFAState currentState = toProcess.takeFirst();
        
        // 使用邻接表查找所有通过ε转换可达的状态
        if (nfa.transitionTable.contains(currentState)) {
            const QHash<QString, QSet<NFAState>> &transitions = nfa.transitionTable[currentState];
            if (transitions.contains("#")) {
                const QSet<NFAState> &epsilonStates = transitions["#"];
                for (NFAState nextState : epsilonStates) {
                    if (!closure.contains(nextState)) {
                        closure.insert(nextState);
                        toProcess.append(nextState);
                    }
                }
            }
        }
    }
    
    return closure;
}

QSet<NFAState> DFABuilder::move(const NFA &nfa, const QSet<NFAState> &states, const QString &input)
{
    QSet<NFAState> result;
    
    for (NFAState state : states) {
        // 使用邻接表查找所有通过input转换可达的状态
        if (nfa.transitionTable.contains(state)) {
            const QHash<QString, QSet<NFAState>> &transitions = nfa.transitionTable[state];
            if (transitions.contains(input)) {
                result.unite(transitions[input]);
            }
        }
    }
    
    return result;
}

DFAState DFABuilder::getNextState()
{
    return m_nextState++;
}

void DFABuilder::resetStateCounter()
{
    m_nextState = 0;
}
