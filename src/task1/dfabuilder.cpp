/*
 * @file dfabuilder.cpp
 * @id dfabuilder-cpp
 * @brief 实现NFA到DFA的转换功能
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task1/dfabuilder.h"
#include <QDebug>
#include <algorithm>

/**
 * @brief 构造函数
 * 
 * 初始化DFA构建器，设置状态计数器初始值为0
 */
DFABuilder::DFABuilder()
    : m_nextState(0)
{
}

/**
 * @brief 析构函数
 * 
 * 清理DFA构建器资源
 */
DFABuilder::~DFABuilder()
{
}

/**
 * @brief 将NFA转换为DFA
 * 
 * 使用子集构造法将非确定有限自动机(NFA)转换为确定有限自动机(DFA)
 * 
 * @param nfa 要转换的NFA
 * @return DFA 转换后的DFA
 */
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
            // 设置起始状态的正则表达式索引
            if (nfa.acceptStateToRegexIndex.contains(nfaState)) {
                int regexIndex = nfa.acceptStateToRegexIndex[nfaState];
                dfa.acceptStateToRegexIndex[startDFAState] = regexIndex;
            }
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
                    // 设置接受状态的正则表达式索引
                    if (nfa.acceptStateToRegexIndex.contains(nfaState)) {
                        int regexIndex = nfa.acceptStateToRegexIndex[nfaState];
                        dfa.acceptStateToRegexIndex[targetDFAState] = regexIndex;
                    }
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

/**
 * @brief 获取错误信息
 * 
 * @return QString 错误信息
 */
QString DFABuilder::getErrorMessage() const
{
    return m_errorMessage;
}

/**
 * @brief 计算ε-闭包
 * 
 * 计算给定状态集合的ε-闭包，即从这些状态出发通过ε转换可达的所有状态
 * 
 * @param nfa 输入的NFA
 * @param states 初始状态集合
 * @return QSet<NFAState> ε-闭包结果
 */
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

/**
 * @brief 计算move操作
 * 
 * 计算从给定状态集合出发，通过特定输入字符转换可达的所有状态
 * 
 * @param nfa 输入的NFA
 * @param states 初始状态集合
 * @param input 输入字符
 * @return QSet<NFAState> move操作结果
 */
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

/**
 * @brief 获取下一个可用的DFA状态编号
 * 
 * @return DFAState 下一个状态编号
 */
DFAState DFABuilder::getNextState()
{
    return m_nextState++;
}

/**
 * @brief 重置状态计数器
 * 
 * 将状态计数器重置为0
 */
void DFABuilder::resetStateCounter()
{
    m_nextState = 0;
}
