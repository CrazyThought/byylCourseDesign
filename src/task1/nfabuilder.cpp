#include "task1/nfabuilder.h"
#include <QDebug>

NFABuilder::NFABuilder()
    : m_nextState(0)
{
}

NFABuilder::~NFABuilder()
{
}

// 构建邻接表
void NFABuilder::buildTransitionTable(NFA &nfa)
{
    // 清空现有邻接表
    nfa.transitionTable.clear();
    
    // 遍历所有转换，构建邻接表
    for (const NFATransition &transition : nfa.transitions) {
        nfa.transitionTable[transition.fromState][transition.input].insert(transition.toState);
    }
}

NFA NFABuilder::buildNFA(const RegexItem &regexItem)
{
    resetStateCounter();
    m_errorMessage.clear();
    
    NFA nfa = parseRegex(regexItem.pattern);
    buildTransitionTable(nfa); // 构建邻接表
    return nfa;
}

QString NFABuilder::getErrorMessage() const
{
    return m_errorMessage;
}

NFA NFABuilder::buildBasicNFA(const QString &input)
{
    NFA nfa;
    
    // 创建两个状态
    NFAState start = getNextState();
    NFAState accept = getNextState();
    
    // 添加状态
    nfa.states << start << accept;
    
    // 添加转换
    NFATransition transition;
    transition.fromState = start;
    transition.input = input;
    transition.toState = accept;
    nfa.transitions << transition;
    
    // 添加输入字符到字母表
    if (input != "#") { // 跳过空串
        nfa.alphabet.insert(input);
    }
    
    // 设置起始状态和接受状态
    nfa.startState = start;
    nfa.acceptStates.insert(accept);
    
    return nfa;
}

NFA NFABuilder::buildConcatenationNFA(const NFA &nfa1, const NFA &nfa2)
{
    NFA result;
    
    // 合并状态
    result.states = nfa1.states + nfa2.states;
    
    // 合并字母表
    result.alphabet = nfa1.alphabet;
    result.alphabet.unite(nfa2.alphabet);
    
    // 合并转换
    result.transitions = nfa1.transitions + nfa2.transitions;
    
    // 连接两个NFA：将nfa1的所有接受状态连接到nfa2的起始状态（通过空串）
    for (NFAState acceptState : nfa1.acceptStates) {
        NFATransition transition;
        transition.fromState = acceptState;
        transition.input = "#"; // 空串
        transition.toState = nfa2.startState;
        result.transitions << transition;
    }
    
    // 设置起始状态和接受状态
    result.startState = nfa1.startState;
    result.acceptStates = nfa2.acceptStates;
    
    return result;
}

NFA NFABuilder::buildChoiceNFA(const NFA &nfa1, const NFA &nfa2)
{
    NFA result;
    
    // 创建新的起始状态和接受状态
    NFAState newStart = getNextState();
    NFAState newAccept = getNextState();
    
    // 合并状态
    result.states << newStart << nfa1.states << nfa2.states << newAccept;
    
    // 合并字母表
    result.alphabet = nfa1.alphabet;
    result.alphabet.unite(nfa2.alphabet);
    
    // 合并转换
    result.transitions = nfa1.transitions + nfa2.transitions;
    
    // 从新起始状态到两个NFA的起始状态（空串）
    NFATransition transition1;
    transition1.fromState = newStart;
    transition1.input = "#";
    transition1.toState = nfa1.startState;
    result.transitions << transition1;
    
    NFATransition transition2;
    transition2.fromState = newStart;
    transition2.input = "#";
    transition2.toState = nfa2.startState;
    result.transitions << transition2;
    
    // 从两个NFA的接受状态到新接受状态（空串）
    for (NFAState acceptState : nfa1.acceptStates) {
        NFATransition transition;
        transition.fromState = acceptState;
        transition.input = "#";
        transition.toState = newAccept;
        result.transitions << transition;
    }
    
    for (NFAState acceptState : nfa2.acceptStates) {
        NFATransition transition;
        transition.fromState = acceptState;
        transition.input = "#";
        transition.toState = newAccept;
        result.transitions << transition;
    }
    
    // 设置起始状态和接受状态
    result.startState = newStart;
    result.acceptStates.insert(newAccept);
    
    return result;
}

NFA NFABuilder::buildClosureNFA(const NFA &nfa)
{
    NFA result;
    
    // 创建新的起始状态和接受状态
    NFAState newStart = getNextState();
    NFAState newAccept = getNextState();
    
    // 合并状态
    result.states << newStart << nfa.states << newAccept;
    
    // 合并字母表
    result.alphabet = nfa.alphabet;
    
    // 合并转换
    result.transitions = nfa.transitions;
    
    // 从新起始状态到NFA起始状态（空串）
    NFATransition transition1;
    transition1.fromState = newStart;
    transition1.input = "#";
    transition1.toState = nfa.startState;
    result.transitions << transition1;
    
    // 从NFA接受状态到NFA起始状态（空串）
    for (NFAState acceptState : nfa.acceptStates) {
        NFATransition transition;
        transition.fromState = acceptState;
        transition.input = "#";
        transition.toState = nfa.startState;
        result.transitions << transition;
    }
    
    // 从NFA接受状态到新接受状态（空串）
    for (NFAState acceptState : nfa.acceptStates) {
        NFATransition transition;
        transition.fromState = acceptState;
        transition.input = "#";
        transition.toState = newAccept;
        result.transitions << transition;
    }
    
    // 从新起始状态到新接受状态（空串）（处理空匹配）
    NFATransition transition2;
    transition2.fromState = newStart;
    transition2.input = "#";
    transition2.toState = newAccept;
    result.transitions << transition2;
    
    // 设置起始状态和接受状态
    result.startState = newStart;
    result.acceptStates.insert(newAccept);
    
    return result;
}

NFA NFABuilder::buildPositiveClosureNFA(const NFA &nfa)
{
    // 正闭包 = 自身 + 闭包
    NFA closureNFA = buildClosureNFA(nfa);
    return buildConcatenationNFA(nfa, closureNFA);
}

NFA NFABuilder::buildOptionalNFA(const NFA &nfa)
{
    // 可选 = 自身 | ε
    NFA epsilonNFA = buildBasicNFA("#");
    return buildChoiceNFA(nfa, epsilonNFA);
}

NFA NFABuilder::buildCharacterSetNFA(const QString &charSet)
{
    // 处理字符集，例如 [a-z0-9]
    NFA result;
    
    // 创建新的起始状态和接受状态
    NFAState newStart = getNextState();
    NFAState newAccept = getNextState();
    
    // 添加状态
    result.states << newStart << newAccept;
    
    // 解析字符集
    QString set = charSet.mid(1, charSet.length() - 2); // 去掉 [ 和 ]
    
    int i = 0;
    while (i < set.length()) {
        if (i + 2 < set.length() && set.at(i + 1) == '-') {
            // 处理范围，如 a-z
            QChar startChar = set.at(i);
            QChar endChar = set.at(i + 2);
            
            // 使用整数类型迭代字符范围
            for (ushort code = startChar.unicode(); code <= endChar.unicode(); ++code) {
                QChar c(code);
                // 添加从起始状态到接受状态的转换
                NFATransition transition;
                transition.fromState = newStart;
                transition.input = c;
                transition.toState = newAccept;
                result.transitions << transition;
                
                // 添加到字母表
                result.alphabet.insert(QString(c));
            }
            
            i += 3;
        } else {
            // 处理单个字符
            QChar c = set.at(i);
            
            NFATransition transition;
            transition.fromState = newStart;
            transition.input = c;
            transition.toState = newAccept;
            result.transitions << transition;
            
            // 添加到字母表
            result.alphabet.insert(QString(c));
            
            i += 1;
        }
    }
    
    // 设置起始状态和接受状态
    result.startState = newStart;
    result.acceptStates.insert(newAccept);
    
    return result;
}

NFA NFABuilder::parseRegex(const QString &regex)
{
    // 实现递归下降解析器来处理正则表达式
    int pos = 0;
    QString error;
    
    // 解析选择操作（最低优先级）
    NFA nfa = parseChoice(regex, pos, error);
    
    if (!error.isEmpty()) {
        m_errorMessage = error;
        return NFA(); // 返回空NFA
    }
    
    if (pos < regex.length()) {
        m_errorMessage = QString("解析错误：在位置 %1 处有未处理的字符")
                         .arg(pos);
        return NFA();
    }
    
    return nfa;
}

NFA NFABuilder::parseChoice(const QString &regex, int &pos, QString &error)
{
    // 解析选择操作：concatenation (| concatenation)*
    NFA result = parseConcatenation(regex, pos, error);
    if (!error.isEmpty()) return result;
    
    while (pos < regex.length() && regex.at(pos) == '|') {
        pos++;
        NFA nextNFA = parseConcatenation(regex, pos, error);
        if (!error.isEmpty()) return result;
        
        result = buildChoiceNFA(result, nextNFA);
    }
    
    return result;
}

NFA NFABuilder::parseConcatenation(const QString &regex, int &pos, QString &error)
{
    // 解析连接操作：closure (closure)*
    NFA result = parseClosure(regex, pos, error);
    if (!error.isEmpty()) return result;
    
    while (pos < regex.length() && regex.at(pos) != '|' && regex.at(pos) != ')') {
        NFA nextNFA = parseClosure(regex, pos, error);
        if (!error.isEmpty()) return result;
        
        result = buildConcatenationNFA(result, nextNFA);
    }
    
    return result;
}

NFA NFABuilder::parseClosure(const QString &regex, int &pos, QString &error)
{
    // 解析闭包操作：primary (* | + | ?)*
    NFA result = parsePrimary(regex, pos, error);
    if (!error.isEmpty()) return result;
    
    while (pos < regex.length()) {
        QChar c = regex.at(pos);
        if (c == '*') {
            pos++;
            result = buildClosureNFA(result);
        } else if (c == '+') {
            pos++;
            result = buildPositiveClosureNFA(result);
        } else if (c == '?') {
            pos++;
            result = buildOptionalNFA(result);
        } else {
            break;
        }
    }
    
    return result;
}

NFA NFABuilder::parsePrimary(const QString &regex, int &pos, QString &error)
{
    // 解析基本元素：字符、字符集、或 (regex)
    if (pos >= regex.length()) {
        error = "解析错误：表达式意外结束";
        return NFA();
    }
    
    QChar c = regex.at(pos);
    
    if (c == '(') {
        // 处理括号
        pos++;
        NFA result = parseChoice(regex, pos, error);
        if (!error.isEmpty()) return result;
        
        if (pos >= regex.length() || regex.at(pos) != ')') {
            error = "解析错误：缺少右括号";
            return NFA();
        }
        pos++;
        return result;
    } else if (c == '[') {
        // 处理字符集
        int endPos = regex.indexOf(']', pos);
        if (endPos == -1) {
            error = "解析错误：缺少右方括号";
            return NFA();
        }
        
        QString charSet = regex.mid(pos, endPos - pos + 1);
        pos = endPos + 1;
        
        return buildCharacterSetNFA(charSet);
    } else if (c == '\\') {
        // 处理转义字符
        if (pos + 1 >= regex.length()) {
            error = "解析错误：转义字符后缺少字符";
            return NFA();
        }
        
        QString escapedChar = regex.mid(pos, 2);
        pos += 2;
        
        return buildBasicNFA(escapedChar.right(1));
    } else {
        // 处理普通字符
        pos++;
        return buildBasicNFA(QString(c));
    }
}

NFAState NFABuilder::getNextState()
{
    return m_nextState++;
}

void NFABuilder::resetStateCounter()
{
    m_nextState = 0;
}
