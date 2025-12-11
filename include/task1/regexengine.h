/*
 * @file regexengine.h
 * @id regexengine-h
 * @brief 实现正则表达式引擎，包括词法分析、语法分析、AST构建和NFA模拟
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#ifndef REGEXENGINE_H
#define REGEXENGINE_H

#include <QString>
#include <QList>
#include <QSet>
#include <QMap>
#include <QChar>

/**
 * @brief 词法单元类型
 * 
 * 定义正则表达式词法分析器生成的词法单元类型
 */
enum class TokenType {
    CHARACTER,       ///< 普通字符
    DOT,             ///< 匹配任意字符（除换行符外）
    CARET,           ///< 行首匹配
    DOLLAR,          ///< 行尾匹配
    STAR,            ///< 零次或多次重复
    PLUS,            ///< 一次或多次重复
    QUESTION,        ///< 零次或一次重复
    LEFT_PAREN,      ///< 左括号，用于分组
    RIGHT_PAREN,     ///< 右括号，用于分组
    LEFT_BRACKET,    ///< 左方括号，用于字符集
    RIGHT_BRACKET,   ///< 右方括号，用于字符集
    LEFT_BRACE,      ///< 左大括号，用于重复次数
    RIGHT_BRACE,     ///< 右大括号，用于重复次数
    PIPE,            ///< 或操作符
    HYPHEN,          ///< 连字符，用于字符范围
    CARET_NEGATE,     ///< 字符集内的取反符号
    COMMA,           ///< 逗号，用于重复次数范围
    ESCAPE,          ///< 转义字符
    END_OF_INPUT     ///< 输入结束标记
};

/**
 * @brief 词法单元结构
 * 
 * 表示词法分析器生成的单个词法单元
 */
typedef struct {
    TokenType type;  ///< 词法单元类型
    QChar value;     ///< 词法单元值，对于部分类型可能无效
} Token;

/**
 * @brief 抽象语法树节点类型
 * 
 * 定义正则表达式抽象语法树的节点类型
 */
enum class ASTNodeType {
    CHARACTER,       ///< 普通字符节点
    DOT,             ///< 任意字符匹配节点
    STAR,            ///< 零次或多次重复节点
    PLUS,            ///< 一次或多次重复节点
    QUESTION,        ///< 零次或一次重复节点
    REPEAT,          ///< 精确重复次数节点 {n}, {n,}, {n,m}
    CHOICE,          ///< 或操作节点 |
    CONCAT,          ///< 连接操作节点
    GROUP,           ///< 分组节点 ()
    CHAR_SET,        ///< 字符集节点 []
    NEG_CHAR_SET,    ///< 否定字符集节点 [^]
    CARET,           ///< 行首匹配节点 ^
    DOLLAR           ///< 行尾匹配节点 $
};

/**
 * @brief 抽象语法树节点
 * 
 * 表示正则表达式的抽象语法树节点
 */
typedef struct ASTNode {
    ASTNodeType type;       ///< 节点类型
    QChar character;        ///< 用于普通字符节点
    struct ASTNode *left;   ///< 左子节点
    struct ASTNode *right;  ///< 右子节点
    int minRepeat;          ///< 用于重复节点，最小重复次数
    int maxRepeat;          ///< 用于重复节点，最大重复次数
    QSet<QChar> charSet;    ///< 用于字符集节点，包含的字符集合
} ASTNode;

/**
 * @brief NFA状态类型
 * 
 * 表示NFA的状态，使用整数表示
 */
typedef int NFAState;

/**
 * @brief NFA转换
 * 
 * 表示NFA的状态转换
 */
typedef struct {
    NFAState fromState;     ///< 起始状态
    QChar input;            ///< 输入字符，QChar()表示epsilon转换
    NFAState toState;       ///< 目标状态
} NFATransition;

/**
 * @brief NFA结构
 * 
 * 表示非确定性有限自动机
 */
typedef struct {
    QSet<NFAState> states;              ///< 所有状态集合
    QList<NFATransition> transitions;   ///< 所有转换集合
    NFAState startState;                ///< 起始状态
    QSet<NFAState> acceptStates;        ///< 接受状态集合
} NFA;

/**
 * @brief 正则表达式引擎类
 * 
 * 实现正则表达式的编译和匹配功能
 */
class RegexEngine
{
public:
    /**
     * @brief 构造函数
     */
    RegexEngine();
    
    /**
     * @brief 析构函数
     */
    ~RegexEngine();
    
    /**
     * @brief 编译正则表达式
     * 
     * 将正则表达式字符串编译为NFA
     * 
     * @param pattern 正则表达式字符串
     * @return bool 编译成功返回true，失败返回false
     */
    bool compile(const QString &pattern);
    
    /**
     * @brief 匹配字符串
     * 
     * 从字符串开头开始匹配正则表达式
     * 
     * @param input 输入字符串
     * @param matchLength 输出参数，匹配到的长度
     * @return bool 匹配成功返回true，失败返回false
     */
    bool match(const QString &input, int &matchLength);
    
    /**
     * @brief 搜索字符串
     * 
     * 在字符串中搜索正则表达式的匹配
     * 
     * @param input 输入字符串
     * @param startPos 输出参数，匹配开始位置
     * @param matchLength 输出参数，匹配长度
     * @return bool 找到匹配返回true，否则返回false
     */
    bool search(const QString &input, int &startPos, int &matchLength);
    
    /**
     * @brief 获取错误信息
     * 
     * 获取编译或匹配过程中的错误信息
     * 
     * @return QString 错误信息
     */
    QString getErrorMessage() const;
    
private:
    /**
     * @brief 词法分析器
     * 
     * 将正则表达式字符串转换为词法单元列表
     * 
     * @param pattern 正则表达式字符串
     * @return QList<Token> 词法单元列表
     */
    QList<Token> lex(const QString &pattern);
    
    /**
     * @brief 语法分析器入口
     * 
     * 将词法单元列表转换为抽象语法树
     * 
     * @param tokens 词法单元列表
     * @return ASTNode* 抽象语法树根节点
     */
    ASTNode* parse(const QList<Token> &tokens);
    
    /**
     * @brief 解析表达式
     * 
     * 解析表达式级别的语法规则
     * 
     * @param tokens 词法单元列表
     * @param pos 当前解析位置，会被修改
     * @return ASTNode* 解析生成的AST节点
     */
    ASTNode* parseExpr(const QList<Token> &tokens, int &pos);
    
    /**
     * @brief 解析项
     * 
     * 解析项级别的语法规则
     * 
     * @param tokens 词法单元列表
     * @param pos 当前解析位置，会被修改
     * @return ASTNode* 解析生成的AST节点
     */
    ASTNode* parseTerm(const QList<Token> &tokens, int &pos);
    
    /**
     * @brief 解析因子
     * 
     * 解析因子级别的语法规则
     * 
     * @param tokens 词法单元列表
     * @param pos 当前解析位置，会被修改
     * @return ASTNode* 解析生成的AST节点
     */
    ASTNode* parseFactor(const QList<Token> &tokens, int &pos);
    
    /**
     * @brief 解析原子
     * 
     * 解析原子级别的语法规则
     * 
     * @param tokens 词法单元列表
     * @param pos 当前解析位置，会被修改
     * @return ASTNode* 解析生成的AST节点
     */
    ASTNode* parseAtom(const QList<Token> &tokens, int &pos);
    
    /**
     * @brief 解析量词
     * 
     * 解析量词（重复次数）
     * 
     * @param tokens 词法单元列表
     * @param pos 当前解析位置，会被修改
     * @param atom 原子节点
     * @return ASTNode* 解析生成的带量词的AST节点
     */
    ASTNode* parseQuantifier(const QList<Token> &tokens, int &pos, ASTNode *atom);
    
    /**
     * @brief 解析字符集
     * 
     * 解析字符集 [] 或 [^]
     * 
     * @param tokens 词法单元列表
     * @param pos 当前解析位置，会被修改
     * @param isNegated 是否是否定字符集
     * @return ASTNode* 解析生成的字符集AST节点
     */
    ASTNode* parseCharSet(const QList<Token> &tokens, int &pos, bool isNegated);
    
    /**
     * @brief 解析数字
     * 
     * 解析重复次数中的数字
     * 
     * @param tokens 词法单元列表
     * @param pos 当前解析位置，会被修改
     * @return int 解析出的数字
     */
    int parseNumber(const QList<Token> &tokens, int &pos);
    
    /**
     * @brief 创建AST节点
     * 
     * 创建指定类型的AST节点
     * 
     * @param type 节点类型
     * @return ASTNode* 创建的AST节点
     */
    ASTNode* createNode(ASTNodeType type);
    
    /**
     * @brief 创建普通字符节点
     * 
     * 创建普通字符类型的AST节点
     * 
     * @param c 字符值
     * @return ASTNode* 创建的AST节点
     */
    ASTNode* createCharacterNode(QChar c);
    
    /**
     * @brief 创建重复节点
     * 
     * 创建重复次数类型的AST节点
     * 
     * @param child 子节点
     * @param min 最小重复次数
     * @param max 最大重复次数
     * @return ASTNode* 创建的AST节点
     */
    ASTNode* createRepeatNode(ASTNode *child, int min, int max);
    
    /**
     * @brief 创建字符集节点
     * 
     * 创建字符集类型的AST节点
     * 
     * @param charSet 字符集合
     * @param isNegated 是否是否定字符集
     * @return ASTNode* 创建的AST节点
     */
    ASTNode* createCharSetNode(const QSet<QChar> &charSet, bool isNegated);
    
    /**
     * @brief 构建NFA
     * 
     * 从AST构建NFA
     * 
     * @param root AST根节点
     * @return NFA 构建的NFA
     */
    NFA buildNFA(ASTNode *root);
    
    /**
     * @brief 构建基本字符NFA
     * 
     * 为普通字符构建NFA
     * 
     * @param c 字符
     * @return NFA 构建的NFA
     */
    NFA buildBasicNFA(QChar c);
    
    /**
     * @brief 构建任意字符匹配NFA
     * 
     * 为 . 构建NFA
     * 
     * @return NFA 构建的NFA
     */
    NFA buildDotNFA();
    
    /**
     * @brief 构建行首匹配NFA
     * 
     * 为 ^ 构建NFA
     * 
     * @return NFA 构建的NFA
     */
    NFA buildCaretNFA();
    
    /**
     * @brief 构建行尾匹配NFA
     * 
     * 为 $ 构建NFA
     * 
     * @return NFA 构建的NFA
     */
    NFA buildDollarNFA();
    
    /**
     * @brief 构建重复NFA
     * 
     * 为重复操作构建NFA
     * 
     * @param nfa 原始NFA
     * @param min 最小重复次数
     * @param max 最大重复次数
     * @return NFA 构建的NFA
     */
    NFA buildRepeatNFA(const NFA &nfa, int min, int max);
    
    /**
     * @brief 构建或操作NFA
     * 
     * 为 | 操作构建NFA
     * 
     * @param nfa1 第一个NFA
     * @param nfa2 第二个NFA
     * @return NFA 构建的NFA
     */
    NFA buildChoiceNFA(const NFA &nfa1, const NFA &nfa2);
    
    /**
     * @brief 构建连接操作NFA
     * 
     * 为连接操作构建NFA
     * 
     * @param nfa1 第一个NFA
     * @param nfa2 第二个NFA
     * @return NFA 构建的NFA
     */
    NFA buildConcatNFA(const NFA &nfa1, const NFA &nfa2);
    
    /**
     * @brief 构建分组NFA
     * 
     * 为分组 () 构建NFA
     * 
     * @param nfa 原始NFA
     * @return NFA 构建的NFA
     */
    NFA buildGroupNFA(const NFA &nfa);
    
    /**
     * @brief 构建字符集NFA
     * 
     * 为字符集 [] 构建NFA
     * 
     * @param charSet 字符集合
     * @param isNegated 是否是否定字符集
     * @return NFA 构建的NFA
     */
    NFA buildCharSetNFA(const QSet<QChar> &charSet, bool isNegated);
    
    /**
     * @brief 模拟NFA匹配
     * 
     * 使用NFA模拟匹配输入字符串
     * 
     * @param nfa NFA
     * @param input 输入字符串
     * @param startPos 匹配起始位置
     * @param matchLength 输出参数，匹配长度
     * @return bool 匹配成功返回true，失败返回false
     */
    bool simulateNFA(const NFA &nfa, const QString &input, int startPos, int &matchLength);
    
    /**
     * @brief 计算epsilon闭包
     * 
     * 计算给定状态集合的epsilon闭包
     * 
     * @param nfa NFA
     * @param states 初始状态集合
     * @return QSet<NFAState> epsilon闭包
     */
    QSet<NFAState> epsilonClosure(const NFA &nfa, const QSet<NFAState> &states);
    
    /**
     * @brief 状态转移
     * 
     * 计算给定状态集合在输入字符下的转移
     * 
     * @param nfa NFA
     * @param states 初始状态集合
     * @param input 输入字符
     * @return QSet<NFAState> 转移后的状态集合
     */
    QSet<NFAState> move(const NFA &nfa, const QSet<NFAState> &states, QChar input);
    
    /**
     * @brief 检查是否为特殊字符
     * 
     * 检查字符是否为正则表达式特殊字符
     * 
     * @param c 字符
     * @return bool 是特殊字符返回true，否则返回false
     */
    bool isSpecialChar(QChar c);
    
    /**
     * @brief 获取转义字符值
     * 
     * 获取转义字符对应的实际字符
     * 
     * @param c 转义字符
     * @return QChar 实际字符
     */
    QChar getEscapedChar(QChar c);
    
    /**
     * @brief 检查是否为数字
     * 
     * 检查字符是否为数字
     * 
     * @param c 字符
     * @return bool 是数字返回true，否则返回false
     */
    bool isDigit(QChar c);
    
    /**
     * @brief 释放AST
     * 
     * 释放AST的内存
     * 
     * @param root AST根节点
     */
    void freeAST(ASTNode *root);
    
    ASTNode *m_root;          ///< 抽象语法树根节点
    NFA m_nfa;               ///< 编译后的NFA
    QString m_errorMessage;   ///< 错误信息
    bool m_isCompiled;        ///< 是否已编译
    int m_nextState;          ///< 用于生成新的NFA状态
};

#endif // REGEXENGINE_H
