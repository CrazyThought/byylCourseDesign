#ifndef REGEXENGINE_H
#define REGEXENGINE_H

#include <QString>
#include <QList>
#include <QSet>
#include <QMap>
#include <QChar>

// 词法单元类型
enum class TokenType {
    CHARACTER,       // 普通字符
    DOT,             // .
    CARET,           // ^
    DOLLAR,          // $
    STAR,            // *
    PLUS,            // +
    QUESTION,        // ?
    LEFT_PAREN,      // (
    RIGHT_PAREN,     // )
    LEFT_BRACKET,    // [
    RIGHT_BRACKET,   // ]
    LEFT_BRACE,      // {
    RIGHT_BRACE,     // }
    PIPE,            // |
    HYPHEN,          // -
    CARET_NEGATE,    // [^中的^
    COMMA,           // ,
    ESCAPE,          // \
    END_OF_INPUT     // 输入结束
};

// 词法单元结构
typedef struct {
    TokenType type;
    QChar value;
} Token;

// 抽象语法树节点类型
enum class ASTNodeType {
    CHARACTER,       // 普通字符
    DOT,             // .
    STAR,            // *
    PLUS,            // +
    QUESTION,        // ?
    REPEAT,          // {n}, {n,}, {n,m}
    CHOICE,          // |
    CONCAT,          // 连接
    GROUP,           // ()
    CHAR_SET,        // []
    NEG_CHAR_SET,    // [^]
    CARET,           // ^
    DOLLAR           // $
};

// 抽象语法树节点
typedef struct ASTNode {
    ASTNodeType type;
    QChar character;   // 用于普通字符
    struct ASTNode *left;   // 左子节点
    struct ASTNode *right;  // 右子节点
    int minRepeat;     // 用于重复节点
    int maxRepeat;     // 用于重复节点
    QSet<QChar> charSet; // 用于字符集
} ASTNode;

// NFA状态
typedef int NFAState;

// NFA转换
typedef struct {
    NFAState fromState;
    QChar input;       // QChar()表示epsilon转换
    NFAState toState;
} NFATransition;

// NFA结构
typedef struct {
    QSet<NFAState> states;
    QList<NFATransition> transitions;
    NFAState startState;
    QSet<NFAState> acceptStates;
} NFA;

// 正则表达式引擎类
class RegexEngine
{
public:
    RegexEngine();
    ~RegexEngine();
    
    // 编译正则表达式
    bool compile(const QString &pattern);
    
    // 匹配字符串，返回是否匹配成功，matchLength为匹配到的长度
    bool match(const QString &input, int &matchLength);
    
    // 搜索字符串，返回是否找到匹配，startPos为匹配开始位置，matchLength为匹配长度
    bool search(const QString &input, int &startPos, int &matchLength);
    
    // 获取错误信息
    QString getErrorMessage() const;
    
private:
    // 词法分析器
    QList<Token> lex(const QString &pattern);
    
    // 语法分析器
    ASTNode* parse(const QList<Token> &tokens);
    ASTNode* parseExpr(const QList<Token> &tokens, int &pos);
    ASTNode* parseTerm(const QList<Token> &tokens, int &pos);
    ASTNode* parseFactor(const QList<Token> &tokens, int &pos);
    ASTNode* parseAtom(const QList<Token> &tokens, int &pos);
    ASTNode* parseQuantifier(const QList<Token> &tokens, int &pos, ASTNode *atom);
    ASTNode* parseCharSet(const QList<Token> &tokens, int &pos, bool isNegated);
    int parseNumber(const QList<Token> &tokens, int &pos);
    
    // AST节点创建
    ASTNode* createNode(ASTNodeType type);
    ASTNode* createCharacterNode(QChar c);
    ASTNode* createRepeatNode(ASTNode *child, int min, int max);
    ASTNode* createCharSetNode(const QSet<QChar> &charSet, bool isNegated);
    
    // Thompson NFA构建
    NFA buildNFA(ASTNode *root);
    NFA buildBasicNFA(QChar c);
    NFA buildDotNFA();
    NFA buildCaretNFA();
    NFA buildDollarNFA();
    NFA buildRepeatNFA(const NFA &nfa, int min, int max);
    NFA buildChoiceNFA(const NFA &nfa1, const NFA &nfa2);
    NFA buildConcatNFA(const NFA &nfa1, const NFA &nfa2);
    NFA buildGroupNFA(const NFA &nfa);
    NFA buildCharSetNFA(const QSet<QChar> &charSet, bool isNegated);
    
    // NFA模拟匹配
    bool simulateNFA(const NFA &nfa, const QString &input, int startPos, int &matchLength);
    
    // epsilon闭包计算
    QSet<NFAState> epsilonClosure(const NFA &nfa, const QSet<NFAState> &states);
    
    // 状态转移
    QSet<NFAState> move(const NFA &nfa, const QSet<NFAState> &states, QChar input);
    
    // 辅助函数
    bool isSpecialChar(QChar c);
    QChar getEscapedChar(QChar c);
    bool isDigit(QChar c);
    
    // 清理函数
    void freeAST(ASTNode *root);
    
    // 成员变量
    ASTNode *m_root;
    NFA m_nfa;
    QString m_errorMessage;
    bool m_isCompiled;
    int m_nextState;
};

#endif // REGEXENGINE_H
