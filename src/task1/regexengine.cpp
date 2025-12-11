/*
 * @file regexengine.cpp
 * @id regexengine-cpp
 * @brief 正则表达式引擎实现文件，包含词法分析、语法分析、AST构建、NFA构建和NFA模拟功能
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */

#include "task1/regexengine.h"
#include <QDebug>

/**
 * @brief RegexEngine构造函数
 * @details 初始化正则表达式引擎的成员变量
 */
RegexEngine::RegexEngine()
    : m_root(nullptr),
      m_isCompiled(false),
      m_nextState(0)
{}

/**
 * @brief RegexEngine析构函数
 * @details 释放AST根节点内存
 */
RegexEngine::~RegexEngine()
{
    freeAST(m_root);
}

/**
 * @brief 编译正则表达式
 * @param pattern 正则表达式字符串
 * @return 编译成功返回true，失败返回false
 * @details 清理之前的编译结果，进行词法分析、语法分析和NFA构建
 */
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
        // 词法分析：将正则表达式字符串转换为令牌列表
        QList<Token> tokens = lex(pattern);
        
        // 语法分析：将令牌列表转换为抽象语法树
        m_root = parse(tokens);
        if (!m_root) {
            m_errorMessage = "语法分析失败";
            return false;
        }
        
        // 构建NFA：根据抽象语法树构建非确定有限自动机
        m_nfa = buildNFA(m_root);
        m_isCompiled = true;
        return true;
    } catch (const QString &error) {
        m_errorMessage = error;
        return false;
    }
    return false;
}

/**
 * @brief 从输入字符串开头匹配正则表达式
 * @param input 输入字符串
 * @param matchLength 匹配到的长度（输出参数）
 * @return 匹配成功返回true，失败返回false
 * @details 使用构建好的NFA模拟匹配过程
 */
bool RegexEngine::match(const QString &input, int &matchLength)
{
    if (!m_isCompiled) {
        m_errorMessage = "正则表达式未编译";
        return false;
    }
    
    return simulateNFA(m_nfa, input, 0, matchLength);
}

/**
 * @brief 在输入字符串中搜索匹配的子串
 * @param input 输入字符串
 * @param startPos 匹配到的起始位置（输出参数）
 * @param matchLength 匹配到的长度（输出参数）
 * @return 搜索到匹配返回true，否则返回false
 * @details 从输入字符串的每个位置尝试匹配，返回第一个匹配结果
 */
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

/**
 * @brief 获取错误信息
 * @return 错误信息字符串
 * @details 返回最近一次编译或匹配失败的错误信息
 */
QString RegexEngine::getErrorMessage() const
{
    return m_errorMessage;
}

/**
 * @brief 词法分析器实现
 * @param pattern 正则表达式字符串
 * @return 令牌列表
 * @details 将正则表达式字符串转换为令牌列表，处理各种特殊字符和转义序列
 */
QList<Token> RegexEngine::lex(const QString &pattern)
{
    QList<Token> tokens;
    int i = 0;
    int len = pattern.length();
    
    while (i < len) {
        QChar c = pattern.at(i);
        
        ushort cUnicode = c.unicode();
        switch (cUnicode) {
            case '.': // 匹配任意字符（除换行符）
                { Token token = {TokenType::DOT, c}; tokens.append(token); }
                break;
            case '^': // 匹配字符串开头或字符集否定
                // 检查是否是字符集的否定（出现在[之后）
                if (!tokens.isEmpty() && tokens.last().type == TokenType::LEFT_BRACKET) {
                    { Token token = {TokenType::CARET_NEGATE, c}; tokens.append(token); }
                } else {
                    { Token token = {TokenType::CARET, c}; tokens.append(token); }
                }
                break;
            case '$': // 匹配字符串结尾
                { Token token = {TokenType::DOLLAR, c}; tokens.append(token); }
                break;
            case '*': // 零次或多次重复
                { Token token = {TokenType::STAR, c}; tokens.append(token); }
                break;
            case '+': // 一次或多次重复
                { Token token = {TokenType::PLUS, c}; tokens.append(token); }
                break;
            case '?': // 零次或一次重复
                { Token token = {TokenType::QUESTION, c}; tokens.append(token); }
                break;
            case '|': // 选择操作符
                { Token token = {TokenType::PIPE, c}; tokens.append(token); }
                break;
            case '(':// 左括号（分组）
                { Token token = {TokenType::LEFT_PAREN, c}; tokens.append(token); }
                break;
            case ')':// 右括号
                { Token token = {TokenType::RIGHT_PAREN, c}; tokens.append(token); }
                break;
            case '[':// 字符集开始
                { Token token = {TokenType::LEFT_BRACKET, c}; tokens.append(token); }
                break;
            case ']':// 字符集结束
                { Token token = {TokenType::RIGHT_BRACKET, c}; tokens.append(token); }
                break;
            case '{':// 重复次数开始
                { Token token = {TokenType::LEFT_BRACE, c}; tokens.append(token); }
                break;
            case '}':// 重复次数结束
                { Token token = {TokenType::RIGHT_BRACE, c}; tokens.append(token); }
                break;
            case '-':// 范围操作符（在字符集中）
                { Token token = {TokenType::HYPHEN, c}; tokens.append(token); }
                break;
            case ',':// 重复次数分隔符
                { Token token = {TokenType::COMMA, c}; tokens.append(token); }
                break;
            case '\\':// 转义字符
                { Token token = {TokenType::ESCAPE, c}; tokens.append(token); }
                i++;
                if (i < len) {
                    QChar nextChar = pattern.at(i);
                    { Token token = {TokenType::CHARACTER, getEscapedChar(nextChar)}; tokens.append(token); }
                } else {
                    throw QString("意外的反斜杠结尾");
                }
                break;
            default:// 普通字符
                { Token token = {TokenType::CHARACTER, c}; tokens.append(token); }
                break;
        }
        i++;
    }
    // 不添加END_OF_INPUT标记，直接返回tokens
    return tokens;
}

/**
 * @brief 语法分析器实现
 * @param tokens 令牌列表
 * @return AST根节点
 * @details 将令牌列表转换为抽象语法树，从expr开始解析
 */
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

/**
 * @brief 解析表达式（处理选择操作）
 * @param tokens 令牌列表
 * @param pos 当前位置（输入输出参数）
 * @return AST节点
 * @details 解析表达式，处理选择操作符 '|'，优先级最低
 */
ASTNode* RegexEngine::parseExpr(const QList<Token> &tokens, int &pos)
{
    ASTNode *left = parseTerm(tokens, pos);
    
    // 处理选择操作 |，如 A|B
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

/**
 * @brief 解析项（处理连接操作）
 * @param tokens 令牌列表
 * @param pos 当前位置（输入输出参数）
 * @return AST节点
 * @details 解析项，处理连接操作（隐式），优先级高于选择操作
 */
ASTNode* RegexEngine::parseTerm(const QList<Token> &tokens, int &pos)
{
    ASTNode *left = nullptr;
    
    // 处理连接操作，如 AB 表示 A 后跟 B
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

/**
 * @brief 解析因子（处理重复操作）
 * @param tokens 令牌列表
 * @param pos 当前位置（输入输出参数）
 * @return AST节点
 * @details 解析因子，处理重复操作符，优先级高于连接操作
 */
ASTNode* RegexEngine::parseFactor(const QList<Token> &tokens, int &pos)
{
    ASTNode *atom = parseAtom(tokens, pos);
    if (!atom) {
        return nullptr;
    }
    
    // 解析重复操作符，如 *, +, ?, {n}, {n,}, {n,m}
    return parseQuantifier(tokens, pos, atom);
}

/**
 * @brief 解析原子项（处理基本元素）
 * @param tokens 令牌列表
 * @param pos 当前位置（输入输出参数）
 * @return AST节点
 * @details 解析原子项，包括字符、.、^、$、括号分组和字符集，优先级最高
 */
ASTNode* RegexEngine::parseAtom(const QList<Token> &tokens, int &pos)
{
    if (pos >= tokens.size()) {
        return nullptr;
    }
    
    const Token &token = tokens[pos];
    
    switch (token.type) {
        case TokenType::DOT: // 匹配任意字符
            pos++;
            return createNode(ASTNodeType::DOT);
            
        case TokenType::CARET: // 匹配字符串开头
            pos++;
            return createNode(ASTNodeType::CARET);
            
        case TokenType::DOLLAR: // 匹配字符串结尾
            pos++;
            return createNode(ASTNodeType::DOLLAR);
            
        case TokenType::CHARACTER: // 普通字符
            pos++;
            return createCharacterNode(token.value);
            
        case TokenType::LEFT_PAREN: // 括号分组
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
            
        case TokenType::LEFT_BRACKET: // 字符集
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

/**
 * @brief 解析重复操作符
 * @param tokens 令牌列表
 * @param pos 当前位置（输入输出参数）
 * @param atom 原子项节点
 * @return AST节点
 * @details 解析各种重复操作符：*, +, ?, {n}, {n,}, {n,m}
 */
ASTNode* RegexEngine::parseQuantifier(const QList<Token> &tokens, int &pos, ASTNode *atom)
{
    if (pos >= tokens.size()) {
        return atom;
    }
    
    const Token &token = tokens[pos];
    
    switch (token.type) {
        case TokenType::STAR: // 零次或多次重复
            pos++;
            {
                ASTNode *repeatNode = createNode(ASTNodeType::REPEAT);
                repeatNode->left = atom;
                repeatNode->minRepeat = 0;
                repeatNode->maxRepeat = -1; // -1表示无限
                return repeatNode;
            }
            
        case TokenType::PLUS: // 一次或多次重复
            pos++;
            {
                ASTNode *repeatNode = createNode(ASTNodeType::REPEAT);
                repeatNode->left = atom;
                repeatNode->minRepeat = 1;
                repeatNode->maxRepeat = -1;
                return repeatNode;
            }
            
        case TokenType::QUESTION: // 零次或一次重复
            pos++;
            {
                ASTNode *repeatNode = createNode(ASTNodeType::REPEAT);
                repeatNode->left = atom;
                repeatNode->minRepeat = 0;
                repeatNode->maxRepeat = 1;
                return repeatNode;
            }
            
        case TokenType::LEFT_BRACE: // 精确重复次数
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

/**
 * @brief 解析字符集
 * @param tokens 令牌列表
 * @param pos 当前位置（输入输出参数）
 * @param isNegated 是否为否定字符集
 * @return AST节点
 * @details 解析字符集，如 [abc] 或 [^abc]，支持范围表示如 [a-z]
 */
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

/**
 * @brief 解析数字
 * @param tokens 令牌列表
 * @param pos 当前位置（输入输出参数）
 * @return 解析出的数字
 * @details 解析连续的数字字符为整数
 */
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

/**
 * @brief 创建AST节点
 * @param type 节点类型
 * @return AST节点指针
 * @details 创建指定类型的AST节点，并初始化默认值
 */
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

/**
 * @brief 创建字符节点
 * @param c 字符值
 * @return AST节点指针
 * @details 创建表示单个字符的AST节点
 */
ASTNode* RegexEngine::createCharacterNode(QChar c)
{
    ASTNode *node = createNode(ASTNodeType::CHARACTER);
    node->character = c;
    return node;
}

/**
 * @brief 创建重复节点
 * @param child 子节点
 * @param min 最小重复次数
 * @param max 最大重复次数（-1表示无限）
 * @return AST节点指针
 * @details 创建表示重复操作的AST节点
 */
ASTNode* RegexEngine::createRepeatNode(ASTNode *child, int min, int max)
{
    ASTNode *node = createNode(ASTNodeType::REPEAT);
    node->left = child;
    node->minRepeat = min;
    node->maxRepeat = max;
    return node;
}

/**
 * @brief 创建字符集节点
 * @param charSet 字符集合
 * @param isNegated 是否为否定字符集
 * @return AST节点指针
 * @details 创建表示字符集的AST节点，支持普通字符集和否定字符集
 */
ASTNode* RegexEngine::createCharSetNode(const QSet<QChar> &charSet, bool isNegated)
{
    ASTNodeType type = isNegated ? ASTNodeType::NEG_CHAR_SET : ASTNodeType::CHAR_SET;
    ASTNode *node = createNode(type);
    node->charSet = charSet;
    return node;
}

/**
 * @brief 根据AST构建NFA
 * @param root AST根节点
 * @return 构建好的NFA
 * @details 递归遍历AST，根据节点类型构建相应的NFA
 */
NFA RegexEngine::buildNFA(ASTNode *root)
{
    if (!root) {
        throw QString("AST为空");
    }
    
    switch (root->type) {
        case ASTNodeType::CHARACTER: // 普通字符
            return buildBasicNFA(root->character);
        case ASTNodeType::DOT: // 匹配任意字符
            return buildDotNFA();
        case ASTNodeType::CARET: // 匹配字符串开头
            return buildCaretNFA();
        case ASTNodeType::DOLLAR: // 匹配字符串结尾
            return buildDollarNFA();
        case ASTNodeType::REPEAT: // 重复操作
            return buildRepeatNFA(buildNFA(root->left), root->minRepeat, root->maxRepeat);
        case ASTNodeType::CHOICE: // 选择操作 |
            return buildChoiceNFA(buildNFA(root->left), buildNFA(root->right));
        case ASTNodeType::CONCAT: // 连接操作
            return buildConcatNFA(buildNFA(root->left), buildNFA(root->right));
        case ASTNodeType::GROUP: // 分组操作
            return buildGroupNFA(buildNFA(root->left));
        case ASTNodeType::CHAR_SET: // 字符集
        case ASTNodeType::NEG_CHAR_SET: // 否定字符集
            return buildCharSetNFA(root->charSet, root->type == ASTNodeType::NEG_CHAR_SET);
        default:
            throw QString("未实现的AST节点类型");
    }
}

/**
 * @brief 构建基本NFA（单个字符）
 * @param c 字符值
 * @return 构建好的NFA
 * @details 为单个字符构建NFA，包含一个开始状态和一个接受状态，通过该字符连接
 */
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

/**
 * @brief 构建点号NFA（匹配任意字符）
 * @return 构建好的NFA
 * @details 为点号（.）构建NFA，在模拟时特殊处理为匹配任意字符
 */
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

/**
 * @brief 构建 caret NFA（匹配字符串开头）
 * @return 构建好的NFA
 * @details 为脱字符（^）构建NFA，在模拟时特殊处理为匹配字符串开头
 */
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

/**
 * @brief 构建 dollar NFA（匹配字符串结尾）
 * @return 构建好的NFA
 * @details 为美元符号（$）构建NFA，在模拟时特殊处理为匹配字符串结尾
 */
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

/**
 * @brief 构建重复NFA
 * @param nfa 原始NFA
 * @param min 最小重复次数
 * @param max 最大重复次数（-1表示无限）
 * @return 构建好的NFA
 * @details 为重复操作（*, +, ?, {n,m}）构建NFA，添加epsilon转换实现重复
 */
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
    
    // 为*和+添加循环（从接受状态回到开始状态）
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
    
    // 为*和?添加直接到结束的epsilon转换（允许零次重复）
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

/**
 * @brief 模拟NFA匹配
 * @param nfa 非确定有限自动机
 * @param input 输入字符串
 * @param startPos 开始匹配位置
 * @param matchLength 匹配到的长度（输出参数）
 * @return 匹配成功返回true，失败返回false
 * @details 使用子集构造法模拟NFA执行，记录最大匹配长度
 */
bool RegexEngine::simulateNFA(const NFA &nfa, const QString &input, int startPos, int &matchLength)
{
    // 初始化当前状态集为开始状态的epsilon闭包
    QSet<NFAState> currentStates = epsilonClosure(nfa, {nfa.startState});
    int maxMatch = -1;
    int inputLen = input.length();
    
    // 检查初始状态是否为接受状态（空匹配）
    QSet<NFAState> tempSet = nfa.acceptStates;
    tempSet.intersect(currentStates);
    if (!tempSet.isEmpty()) {
        maxMatch = 0;
    }
    
    // 模拟NFA执行，逐个字符处理
    for (int i = startPos; i < inputLen; i++) {
        QChar c = input.at(i);
        
        // 执行状态转移，计算下一个状态集
        QSet<NFAState> nextStates;
        
        for (NFAState state : currentStates) {
            for (const NFATransition &transition : nfa.transitions) {
                if (transition.fromState == state) {
                    if (transition.input == QChar()) {
                        // epsilon转换，跳过，在epsilonClosure中处理
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
                        // 直接匹配字符
                        nextStates.insert(transition.toState);
                    }
                }
            }
        }
        
        // 计算下一个状态集的epsilon闭包
        currentStates = epsilonClosure(nfa, nextStates);
        
        // 检查是否有接受状态，更新最大匹配长度
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
    
    // 检查是否匹配字符串结尾（$）
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

/**
 * @brief 计算epsilon闭包
 * @param nfa 非确定有限自动机
 * @param states 初始状态集
 * @return epsilon闭包后的状态集
 * @details 计算状态集的epsilon闭包，即通过epsilon转换可以到达的所有状态
 */
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

/**
 * @brief 状态转移函数
 * @param nfa 非确定有限自动机
 * @param states 当前状态集
 * @param input 输入字符
 * @return 转移后的状态集
 * @details 计算当前状态集通过输入字符可以转移到的所有状态
 */
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

/**
 * @brief 检查是否为特殊字符
 * @param c 字符
 * @return 是特殊字符返回true，否则返回false
 * @details 检查字符是否为正则表达式中的特殊字符
 */
bool RegexEngine::isSpecialChar(QChar c)
{
    return c == '.' || c == '^' || c == '$' || c == '*' || c == '+' || c == '?' ||
           c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
           c == '|' || c == '-' || c == ',' || c == '\\';
}

/**
 * @brief 获取转义字符
 * @param c 转义后的字符
 * @return 实际字符
 * @details 将转义序列转换为实际字符，如\n转换为换行符
 */
QChar RegexEngine::getEscapedChar(QChar c)
{
    switch (c.unicode()) {
        case 'n': return '\n'; // 换行符
        case 't': return '\t'; // 制表符
        case 'r': return '\r'; // 回车符
        case '0': return '\0'; // 空字符
        case '\'': return '\''; // 单引号
        case '"': return '"'; // 双引号
        default: return c; // 其他字符直接返回
    }
}

/**
 * @brief 检查是否为数字字符
 * @param c 字符
 * @return 是数字返回true，否则返回false
 * @details 检查字符是否为0-9的数字
 */
bool RegexEngine::isDigit(QChar c)
{
    return c >= '0' && c <= '9';
}

/**
 * @brief 释放AST内存
 * @param root AST根节点
 * @details 递归释放AST的所有节点内存
 */
void RegexEngine::freeAST(ASTNode *root)
{
    if (root) {
        freeAST(root->left); // 递归释放左子树
        freeAST(root->right); // 递归释放右子树
        delete root; // 释放当前节点
    }
}
