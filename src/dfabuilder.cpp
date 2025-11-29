#include "dfabuilder.h"
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
    dfa.alphabet = nfa.alphabet; // DFA的字母表与NFA相同（不包含ε）
    
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
        
        // 找到所有通过ε转换可达的状态
        for (const NFATransition &transition : nfa.transitions) {
            if (transition.fromState == currentState && transition.input == "#") {
                if (!closure.contains(transition.toState)) {
                    closure.insert(transition.toState);
                    toProcess.append(transition.toState);
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
        // 找到所有通过input转换可达的状态
        for (const NFATransition &transition : nfa.transitions) {
            if (transition.fromState == state && transition.input == input) {
                result.insert(transition.toState);
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
