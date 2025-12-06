#include "task1/regexengine.h"
#include <QDebug>

RegexEngine::RegexEngine()
    : m_root(nullptr),
      m_isCompiled(false),
      m_nextState(0)
{}

RegexEngine::~RegexEngine()
{
    freeAST(m_root);
}

bool RegexEngine::compile(const QString &pattern)
{
    // 清理之前的编译结果
    freeAST(m_root);
    m_nfa.states.clear();
    m_nfa.transitions.clear();
    m_nfa.acceptStates.clear();
    m_isCompiled = false;
    m_errorMessage.clear();
    m_nextState = 0;
    
    try {
        // 词法分析
        QList<Token> tokens = lex(pattern);
        
        // 语法分析
        m_root = parse(tokens);
        if (!m_root) {
            m_errorMessage = "语法分析失败";
            return false;
        }
        
        // 构建NFA
        m_nfa = buildNFA(m_root);
        m_isCompiled = true;
        return true;
    } catch (const QString &error) {
        m_errorMessage = error;
        return false;
    }
    return false;
}

bool RegexEngine::match(const QString &input, int &matchLength)
{
    if (!m_isCompiled) {
        m_errorMessage = "正则表达式未编译";
        return false;
    }
    
    return simulateNFA(m_nfa, input, 0, matchLength);
}

bool RegexEngine::search(const QString &input, int &startPos, int &matchLength)
{
    if (!m_isCompiled) {
        m_errorMessage = "正则表达式未编译";
        return false;
    }
    
    for (int i = 0; i < input.length(); i++) {
        int len = 0;
        if (simulateNFA(m_nfa, input, i, len)) {
            startPos = i;
            matchLength = len;
            return true;
        }
    }
    return false;
}

QString RegexEngine::getErrorMessage() const
{
    return m_errorMessage;
}

// 词法分析器实现
QList<Token> RegexEngine::lex(const QString &pattern)
{
    QList<Token> tokens;
    int i = 0;
    int len = pattern.length();
    
    while (i < len) {
        QChar c = pattern.at(i);
        
        ushort cUnicode = c.unicode();
        switch (cUnicode) {
            case '.':
                { Token token = {TokenType::DOT, c}; tokens.append(token); }
                break;
            case '^':
                // 检查是否是字符集的否定
                if (!tokens.isEmpty() && tokens.last().type == TokenType::LEFT_BRACKET) {
                    { Token token = {TokenType::CARET_NEGATE, c}; tokens.append(token); }
                } else {
                    { Token token = {TokenType::CARET, c}; tokens.append(token); }
                }
                break;
            case '$':
                { Token token = {TokenType::DOLLAR, c}; tokens.append(token); }
                break;
            case '*':
                { Token token = {TokenType::STAR, c}; tokens.append(token); }
                break;
            case '+':
                { Token token = {TokenType::PLUS, c}; tokens.append(token); }
                break;
            case '?':
                { Token token = {TokenType::QUESTION, c}; tokens.append(token); }
                break;
            case '|':
                { Token token = {TokenType::PIPE, c}; tokens.append(token); }
                break;
            case '(':
                { Token token = {TokenType::LEFT_PAREN, c}; tokens.append(token); }
                break;
            case ')':
                { Token token = {TokenType::RIGHT_PAREN, c}; tokens.append(token); }
                break;
            case '[':
                { Token token = {TokenType::LEFT_BRACKET, c}; tokens.append(token); }
                break;
            case ']':
                { Token token = {TokenType::RIGHT_BRACKET, c}; tokens.append(token); }
                break;
            case '{':
                { Token token = {TokenType::LEFT_BRACE, c}; tokens.append(token); }
                break;
            case '}':
                { Token token = {TokenType::RIGHT_BRACE, c}; tokens.append(token); }
                break;
            case '-':
                { Token token = {TokenType::HYPHEN, c}; tokens.append(token); }
                break;
            case ',':
                { Token token = {TokenType::COMMA, c}; tokens.append(token); }
                break;
            case '\\':
                { Token token = {TokenType::ESCAPE, c}; tokens.append(token); }
                i++;
                if (i < len) {
                    QChar nextChar = pattern.at(i);
                    { Token token = {TokenType::CHARACTER, getEscapedChar(nextChar)}; tokens.append(token); }
                } else {
                    throw QString("意外的反斜杠结尾");
                }
                break;
            default:
                { Token token = {TokenType::CHARACTER, c}; tokens.append(token); }
                break;
        }
        i++;
    }
    // 不添加END_OF_INPUT标记，直接返回tokens
    return tokens;
}

// 语法分析器实现
ASTNode* RegexEngine::parse(const QList<Token> &tokens)
{
    int pos = 0;
    ASTNode *root = parseExpr(tokens, pos);
    
    // 检查是否有未处理完的令牌
    if (pos < tokens.size()) {
        throw QString("语法错误：意外的令牌");
    }
    
    return root;
}

ASTNode* RegexEngine::parseExpr(const QList<Token> &tokens, int &pos)
{
    ASTNode *left = parseTerm(tokens, pos);
    
    // 处理选择操作 |
    while (pos < tokens.size() && tokens[pos].type == TokenType::PIPE) {
        pos++;
        ASTNode *right = parseTerm(tokens, pos);
        ASTNode *choiceNode = createNode(ASTNodeType::CHOICE);
        choiceNode->left = left;
        choiceNode->right = right;
        left = choiceNode;
    }
    
    return left;
}

ASTNode* RegexEngine::parseTerm(const QList<Token> &tokens, int &pos)
{
    ASTNode *left = nullptr;
    
    // 处理连接操作
    while (pos < tokens.size()) {
        ASTNode *right = parseFactor(tokens, pos);
        if (!right) {
            break;
        }
        
        if (!left) {
            left = right;
        } else {
            ASTNode *concatNode = createNode(ASTNodeType::CONCAT);
            concatNode->left = left;
            concatNode->right = right;
            left = concatNode;
        }
    }
    
    return left;
}

ASTNode* RegexEngine::parseFactor(const QList<Token> &tokens, int &pos)
{
    ASTNode *atom = parseAtom(tokens, pos);
    if (!atom) {
        return nullptr;
    }
    
    return parseQuantifier(tokens, pos, atom);
}

ASTNode* RegexEngine::parseAtom(const QList<Token> &tokens, int &pos)
{
    if (pos >= tokens.size()) {
        return nullptr;
    }
    
    const Token &token = tokens[pos];
    
    switch (token.type) {
        case TokenType::DOT:
            pos++;
            return createNode(ASTNodeType::DOT);
            
        case TokenType::CARET:
            pos++;
            return createNode(ASTNodeType::CARET);
            
        case TokenType::DOLLAR:
            pos++;
            return createNode(ASTNodeType::DOLLAR);
            
        case TokenType::CHARACTER:
            pos++;
            return createCharacterNode(token.value);
            
        case TokenType::LEFT_PAREN:
            pos++;
            {   
                ASTNode *expr = parseExpr(tokens, pos);
                if (!expr) {
                    throw QString("语法错误：括号内为空");
                }
                
                if (pos >= tokens.size() || tokens[pos].type != TokenType::RIGHT_PAREN) {
                    throw QString("语法错误：缺少右括号");
                }
                pos++;
                
                ASTNode *groupNode = createNode(ASTNodeType::GROUP);
                groupNode->left = expr;
                return groupNode;
            }
            
        case TokenType::LEFT_BRACKET:
            pos++;
            {   
                bool isNegated = false;
                if (pos < tokens.size() && tokens[pos].type == TokenType::CARET_NEGATE) {
                    isNegated = true;
                    pos++;
                }
                
                ASTNode *charSetNode = parseCharSet(tokens, pos, isNegated);
                
                if (pos >= tokens.size() || tokens[pos].type != TokenType::RIGHT_BRACKET) {
                    throw QString("语法错误：缺少右括号");
                }
                pos++;
                
                return charSetNode;
            }
            
        default:
            return nullptr;
    }
}

ASTNode* RegexEngine::parseQuantifier(const QList<Token> &tokens, int &pos, ASTNode *atom)
{
    if (pos >= tokens.size()) {
        return atom;
    }
    
    const Token &token = tokens[pos];
    
    switch (token.type) {
        case TokenType::STAR:
            pos++;
            {
                ASTNode *repeatNode = createNode(ASTNodeType::REPEAT);
                repeatNode->left = atom;
                repeatNode->minRepeat = 0;
                repeatNode->maxRepeat = -1; // -1表示无限
                return repeatNode;
            }
            
        case TokenType::PLUS:
            pos++;
            {
                ASTNode *repeatNode = createNode(ASTNodeType::REPEAT);
                repeatNode->left = atom;
                repeatNode->minRepeat = 1;
                repeatNode->maxRepeat = -1;
                return repeatNode;
            }
            
        case TokenType::QUESTION:
            pos++;
            {
                ASTNode *repeatNode = createNode(ASTNodeType::REPEAT);
                repeatNode->left = atom;
                repeatNode->minRepeat = 0;
                repeatNode->maxRepeat = 1;
                return repeatNode;
            }
            
        case TokenType::LEFT_BRACE:
            pos++;
            {
                int min = parseNumber(tokens, pos);
                int max = min;
                
                if (pos < tokens.size() && tokens[pos].type == TokenType::COMMA) {
                    pos++;
                    max = -1; // 默认为无限
                    if (pos < tokens.size() && tokens[pos].type == TokenType::CHARACTER && isDigit(tokens[pos].value)) {
                        max = parseNumber(tokens, pos);
                    }
                }
                
                if (pos >= tokens.size() || tokens[pos].type != TokenType::RIGHT_BRACE) {
                    throw QString("语法错误：缺少右大括号");
                }
                pos++;
                
                if (max != -1 && min > max) {
                    throw QString("语法错误：重复次数最小值大于最大值");
                }
                
                ASTNode *repeatNode = createNode(ASTNodeType::REPEAT);
                repeatNode->left = atom;
                repeatNode->minRepeat = min;
                repeatNode->maxRepeat = max;
                return repeatNode;
            }
            
        default:
            return atom;
    }
}

ASTNode* RegexEngine::parseCharSet(const QList<Token> &tokens, int &pos, bool isNegated)
{
    QSet<QChar> charSet;
    
    while (pos < tokens.size() && tokens[pos].type != TokenType::RIGHT_BRACKET) {
        if (pos + 2 < tokens.size() && tokens[pos].type == TokenType::CHARACTER && 
            tokens[pos+1].type == TokenType::HYPHEN && 
            tokens[pos+2].type == TokenType::CHARACTER) {
            // 处理范围，如 a-z
            QChar start = tokens[pos].value;
            QChar end = tokens[pos+2].value;
            
            if (start > end) {
                throw QString("字符集范围无效");
            }
            
            for (QChar c = start; c <= end; c = QChar(c.unicode() + 1)) {
                charSet.insert(c);
            }
            pos += 3;
        } else if (tokens[pos].type == TokenType::CHARACTER) {
            // 处理单个字符
            charSet.insert(tokens[pos].value);
            pos++;
        } else {
            throw QString("字符集语法错误");
        }
    }
    
    return createCharSetNode(charSet, isNegated);
}

int RegexEngine::parseNumber(const QList<Token> &tokens, int &pos)
{
    if (pos >= tokens.size() || !isDigit(tokens[pos].value)) {
        throw QString("语法错误：需要数字");
    }
    
    int num = 0;
    while (pos < tokens.size() && isDigit(tokens[pos].value)) {
        num = num * 10 + (tokens[pos].value.unicode() - '0');
        pos++;
    }
    
    return num;
}

// AST节点创建
ASTNode* RegexEngine::createNode(ASTNodeType type)
{
    ASTNode *node = new ASTNode;
    node->type = type;
    node->character = QChar();
    node->left = nullptr;
    node->right = nullptr;
    node->minRepeat = 0;
    node->maxRepeat = 0;
    return node;
}

ASTNode* RegexEngine::createCharacterNode(QChar c)
{
    ASTNode *node = createNode(ASTNodeType::CHARACTER);
    node->character = c;
    return node;
}

ASTNode* RegexEngine::createRepeatNode(ASTNode *child, int min, int max)
{
    ASTNode *node = createNode(ASTNodeType::REPEAT);
    node->left = child;
    node->minRepeat = min;
    node->maxRepeat = max;
    return node;
}

ASTNode* RegexEngine::createCharSetNode(const QSet<QChar> &charSet, bool isNegated)
{
    ASTNodeType type = isNegated ? ASTNodeType::NEG_CHAR_SET : ASTNodeType::CHAR_SET;
    ASTNode *node = createNode(type);
    node->charSet = charSet;
    return node;
}

// NFA构建
NFA RegexEngine::buildNFA(ASTNode *root)
{
    if (!root) {
        throw QString("AST为空");
    }
    
    switch (root->type) {
        case ASTNodeType::CHARACTER:
            return buildBasicNFA(root->character);
        case ASTNodeType::DOT:
            return buildDotNFA();
        case ASTNodeType::CARET:
            return buildCaretNFA();
        case ASTNodeType::DOLLAR:
            return buildDollarNFA();
        case ASTNodeType::REPEAT:
            return buildRepeatNFA(buildNFA(root->left), root->minRepeat, root->maxRepeat);
        case ASTNodeType::CHOICE:
            return buildChoiceNFA(buildNFA(root->left), buildNFA(root->right));
        case ASTNodeType::CONCAT:
            return buildConcatNFA(buildNFA(root->left), buildNFA(root->right));
        case ASTNodeType::GROUP:
            return buildGroupNFA(buildNFA(root->left));
        case ASTNodeType::CHAR_SET:
        case ASTNodeType::NEG_CHAR_SET:
            return buildCharSetNFA(root->charSet, root->type == ASTNodeType::NEG_CHAR_SET);
        default:
            throw QString("未实现的AST节点类型");
    }
}

NFA RegexEngine::buildBasicNFA(QChar c)
{
    NFA nfa;
    NFAState start = m_nextState++;
    NFAState end = m_nextState++;
    
    nfa.states.insert(start);
    nfa.states.insert(end);
    nfa.startState = start;
    nfa.acceptStates.insert(end);
    
    NFATransition transition;
    transition.fromState = start;
    transition.input = c;
    transition.toState = end;
    nfa.transitions.append(transition);
    
    return nfa;
}

NFA RegexEngine::buildDotNFA()
{
    // 由于NFA转换只能匹配特定字符，我们需要为每个可能的字符创建转换
    // 但为了效率，我们将在模拟时特殊处理
    NFA nfa;
    NFAState start = m_nextState++;
    NFAState end = m_nextState++;
    
    nfa.states.insert(start);
    nfa.states.insert(end);
    nfa.startState = start;
    nfa.acceptStates.insert(end);
    
    // 使用特殊字符表示任意字符匹配
    NFATransition transition;
    transition.fromState = start;
    transition.input = '.';
    transition.toState = end;
    nfa.transitions.append(transition);
    
    return nfa;
}

NFA RegexEngine::buildCaretNFA()
{
    // ^ 表示匹配字符串开头，我们将在模拟时特殊处理
    NFA nfa;
    NFAState start = m_nextState++;
    NFAState end = m_nextState++;
    
    nfa.states.insert(start);
    nfa.states.insert(end);
    nfa.startState = start;
    nfa.acceptStates.insert(end);
    
    // 使用特殊字符表示字符串开头匹配
    NFATransition transition;
    transition.fromState = start;
    transition.input = '^';
    transition.toState = end;
    nfa.transitions.append(transition);
    
    return nfa;
}

NFA RegexEngine::buildDollarNFA()
{
    // $ 表示匹配字符串结尾，我们将在模拟时特殊处理
    NFA nfa;
    NFAState start = m_nextState++;
    NFAState end = m_nextState++;
    
    nfa.states.insert(start);
    nfa.states.insert(end);
    nfa.startState = start;
    nfa.acceptStates.insert(end);
    
    // 使用特殊字符表示字符串结尾匹配
    NFATransition transition;
    transition.fromState = start;
    transition.input = '$';
    transition.toState = end;
    nfa.transitions.append(transition);
    
    return nfa;
}

NFA RegexEngine::buildRepeatNFA(const NFA &nfa, int min, int max)
{
    NFA result;
    NFAState start = m_nextState++;
    NFAState end = m_nextState++;
    
    result.states.insert(start);
    result.states.insert(end);
    result.startState = start;
    result.acceptStates.insert(end);
    
    // 添加初始epsilon转换到NFA的开始
    NFATransition startEpsilon;
    startEpsilon.fromState = start;
    startEpsilon.input = QChar();
    startEpsilon.toState = nfa.startState;
    result.transitions.append(startEpsilon);
    
    // 复制原始NFA的状态和转换
    for (NFAState state : nfa.states) {
        result.states.insert(state);
    }
    for (const NFATransition &transition : nfa.transitions) {
        result.transitions.append(transition);
    }
    
    // 为*和+添加循环
    if (max != 1) {
        // 从NFA的接受状态添加epsilon转换回到开始状态
        for (NFAState acceptState : nfa.acceptStates) {
            NFATransition loopEpsilon;
            loopEpsilon.fromState = acceptState;
            loopEpsilon.input = QChar();
            loopEpsilon.toState = nfa.startState;
            result.transitions.append(loopEpsilon);
        }
    }
    
    // 为*和?添加直接到结束的epsilon转换
    if (min == 0) {
        NFATransition directEpsilon;
        directEpsilon.fromState = start;
        directEpsilon.input = QChar();
        directEpsilon.toState = end;
        result.transitions.append(directEpsilon);
    }
    
    // 添加从NFA的接受状态到结果结束状态的epsilon转换
    for (NFAState acceptState : nfa.acceptStates) {
        NFATransition endEpsilon;
        endEpsilon.fromState = acceptState;
        endEpsilon.input = QChar();
        endEpsilon.toState = end;
        result.transitions.append(endEpsilon);
    }
    
    return result;
}

NFA RegexEngine::buildChoiceNFA(const NFA &nfa1, const NFA &nfa2)
{
    NFA result;
    NFAState start = m_nextState++;
    NFAState end = m_nextState++;
    
    result.states.insert(start);
    result.states.insert(end);
    result.startState = start;
    result.acceptStates.insert(end);
    
    // 添加epsilon转换从新开始状态到两个NFA的开始状态
    NFATransition startEpsilon1;
    startEpsilon1.fromState = start;
    startEpsilon1.input = QChar();
    startEpsilon1.toState = nfa1.startState;
    result.transitions.append(startEpsilon1);
    
    NFATransition startEpsilon2;
    startEpsilon2.fromState = start;
    startEpsilon2.input = QChar();
    startEpsilon2.toState = nfa2.startState;
    result.transitions.append(startEpsilon2);
    
    // 复制两个NFA的状态和转换
    for (NFAState state : nfa1.states) {
        result.states.insert(state);
    }
    for (const NFATransition &transition : nfa1.transitions) {
        result.transitions.append(transition);
    }
    
    for (NFAState state : nfa2.states) {
        result.states.insert(state);
    }
    for (const NFATransition &transition : nfa2.transitions) {
        result.transitions.append(transition);
    }
    
    // 添加epsilon转换从两个NFA的接受状态到新结束状态
    for (NFAState acceptState : nfa1.acceptStates) {
        NFATransition endEpsilon;
        endEpsilon.fromState = acceptState;
        endEpsilon.input = QChar();
        endEpsilon.toState = end;
        result.transitions.append(endEpsilon);
    }
    
    for (NFAState acceptState : nfa2.acceptStates) {
        NFATransition endEpsilon;
        endEpsilon.fromState = acceptState;
        endEpsilon.input = QChar();
        endEpsilon.toState = end;
        result.transitions.append(endEpsilon);
    }
    
    return result;
}

NFA RegexEngine::buildConcatNFA(const NFA &nfa1, const NFA &nfa2)
{
    NFA result;
    
    // 复制第一个NFA的状态和转换
    for (NFAState state : nfa1.states) {
        result.states.insert(state);
    }
    for (const NFATransition &transition : nfa1.transitions) {
        result.transitions.append(transition);
    }
    
    // 复制第二个NFA的状态和转换
    for (NFAState state : nfa2.states) {
        result.states.insert(state);
    }
    for (const NFATransition &transition : nfa2.transitions) {
        result.transitions.append(transition);
    }
    
    // 设置结果NFA的开始状态为第一个NFA的开始状态
    result.startState = nfa1.startState;
    
    // 添加epsilon转换从第一个NFA的接受状态到第二个NFA的开始状态
    for (NFAState acceptState : nfa1.acceptStates) {
        NFATransition concatEpsilon;
        concatEpsilon.fromState = acceptState;
        concatEpsilon.input = QChar();
        concatEpsilon.toState = nfa2.startState;
        result.transitions.append(concatEpsilon);
    }
    
    // 设置结果NFA的接受状态为第二个NFA的接受状态
    for (NFAState acceptState : nfa2.acceptStates) {
        result.acceptStates.insert(acceptState);
    }
    
    return result;
}

NFA RegexEngine::buildGroupNFA(const NFA &nfa)
{
    // 分组只是影响捕获，对于匹配来说没有影响，直接返回原NFA
    return nfa;
}

NFA RegexEngine::buildCharSetNFA(const QSet<QChar> &charSet, bool isNegated)
{
    NFA nfa;
    NFAState start = m_nextState++;
    NFAState end = m_nextState++;
    
    nfa.states.insert(start);
    nfa.states.insert(end);
    nfa.startState = start;
    nfa.acceptStates.insert(end);
    
    // 使用特殊字符表示字符集匹配，在模拟时特殊处理
    NFATransition transition;
    transition.fromState = start;
    transition.input = isNegated ? '^' : '[';
    transition.toState = end;
    nfa.transitions.append(transition);
    
    // 存储字符集信息，用于模拟时使用
    // 注意：这只是一个临时解决方案，实际实现中应该将字符集信息与转换关联
    static QMap<NFAState, QPair<QSet<QChar>, bool>> charSetInfo;
    charSetInfo[start] = qMakePair(charSet, isNegated);
    
    return nfa;
}

// NFA模拟匹配
bool RegexEngine::simulateNFA(const NFA &nfa, const QString &input, int startPos, int &matchLength)
{
    // 初始化当前状态集为开始状态的epsilon闭包
    QSet<NFAState> currentStates = epsilonClosure(nfa, {nfa.startState});
    int maxMatch = -1;
    int inputLen = input.length();
    
    // 检查初始状态是否为接受状态
    QSet<NFAState> tempSet = nfa.acceptStates;
    tempSet.intersect(currentStates);
    if (!tempSet.isEmpty()) {
        maxMatch = 0;
    }
    
    // 模拟NFA执行
    for (int i = startPos; i < inputLen; i++) {
        QChar c = input.at(i);
        
        // 执行状态转移
        QSet<NFAState> nextStates;
        
        for (NFAState state : currentStates) {
            for (const NFATransition &transition : nfa.transitions) {
                if (transition.fromState == state) {
                    if (transition.input == QChar()) {
                        // epsilon转换，跳过
                        continue;
                    } else if (transition.input == '.') {
                        // 匹配任意字符，除了换行符
                        if (c != '\n') {
                            nextStates.insert(transition.toState);
                        }
                    } else if (transition.input == '^') {
                        // 匹配字符串开头，只有当i == startPos时匹配
                        if (i == startPos) {
                            nextStates.insert(transition.toState);
                        }
                    } else if (transition.input == '$') {
                        // 匹配字符串结尾，只有当i == inputLen - 1时匹配
                        if (i == inputLen - 1) {
                            nextStates.insert(transition.toState);
                        }
                    } else if (transition.input == '[') {
                        // 字符集匹配，需要查找字符集信息
                        // 注意：这只是一个临时解决方案
                        static QMap<NFAState, QPair<QSet<QChar>, bool>> charSetInfo;
                        if (charSetInfo.contains(state)) {
                            QSet<QChar> charSet = charSetInfo[state].first;
                            bool isNegated = charSetInfo[state].second;
                            if ((charSet.contains(c) && !isNegated) || (!charSet.contains(c) && isNegated)) {
                                nextStates.insert(transition.toState);
                            }
                        }
                    } else if (transition.input == c) {
                        // 直接匹配
                        nextStates.insert(transition.toState);
                    }
                }
            }
        }
        
        // 计算epsilon闭包
        currentStates = epsilonClosure(nfa, nextStates);
        
        // 检查是否有接受状态
        QSet<NFAState> tempSet2 = nfa.acceptStates;
        tempSet2.intersect(currentStates);
        if (!tempSet2.isEmpty()) {
            maxMatch = i - startPos + 1;
        }
        
        // 如果没有状态可以转移，停止模拟
        if (currentStates.isEmpty()) {
            break;
        }
    }
    
    // 检查是否匹配字符串结尾
    for (NFAState state : currentStates) {
        for (const NFATransition &transition : nfa.transitions) {
            if (transition.fromState == state && transition.input == '$') {
                // 匹配字符串结尾
                if (startPos + maxMatch == inputLen) {
                    matchLength = maxMatch;
                    return true;
                }
            }
        }
    }
    
    if (maxMatch != -1) {
        matchLength = maxMatch;
        return true;
    }
    
    return false;
}

// epsilon闭包计算
QSet<NFAState> RegexEngine::epsilonClosure(const NFA &nfa, const QSet<NFAState> &states)
{
    QSet<NFAState> closure = states;
    QList<NFAState> toProcess(states.begin(), states.end());
    
    while (!toProcess.isEmpty()) {
        NFAState state = toProcess.takeFirst();
        
        for (const NFATransition &transition : nfa.transitions) {
            if (transition.fromState == state && transition.input == QChar()) {
                if (!closure.contains(transition.toState)) {
                    closure.insert(transition.toState);
                    toProcess.append(transition.toState);
                }
            }
        }
    }
    
    return closure;
}

// 状态转移
QSet<NFAState> RegexEngine::move(const NFA &nfa, const QSet<NFAState> &states, QChar input)
{
    QSet<NFAState> result;
    
    for (NFAState state : states) {
        for (const NFATransition &transition : nfa.transitions) {
            if (transition.fromState == state && transition.input == input) {
                result.insert(transition.toState);
            }
        }
    }
    
    return result;
}

// 辅助函数
bool RegexEngine::isSpecialChar(QChar c)
{
    return c == '.' || c == '^' || c == '$' || c == '*' || c == '+' || c == '?' ||
           c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
           c == '|' || c == '-' || c == ',' || c == '\\';
}

QChar RegexEngine::getEscapedChar(QChar c)
{
    switch (c.unicode()) {
        case 'n': return '\n';
        case 't': return '\t';
        case 'r': return '\r';
        case '0': return '\0';
        case '\'': return '\'';
        case '"': return '"';
        default: return c;
    }
}

bool RegexEngine::isDigit(QChar c)
{
    return c >= '0' && c <= '9';
}

void RegexEngine::freeAST(ASTNode *root)
{
    if (root) {
        freeAST(root->left);
        freeAST(root->right);
        delete root;
    }
}
