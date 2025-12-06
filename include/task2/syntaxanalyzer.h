#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H

#include <QString>
#include <QSet>
#include <QList>
#include <QMap>
#include <QTreeWidgetItem>

// 前向声明
class LR1Table;
class BNFParser;
struct Action;

// 语法树节点类型枚举
enum SyntaxTreeNodeType {
    NODE_ROOT,          // 根节点
    NODE_NON_TERMINAL,   // 非终结符
    NODE_TERMINAL,       // 终结符
    NODE_PRODUCTION,     // 产生式
    NODE_STATEMENT,      // 语句
    NODE_EXPRESSION      // 表达式
};

// 语法树节点结构
struct SyntaxTreeNode {
    SyntaxTreeNodeType type;       // 节点类型
    QString value;                // 节点值（符号名称）
    QList<SyntaxTreeNode> children; // 子节点列表
    int productionIndex;          // 产生式编号
    
    /**
     * @brief 打印节点
     * @param indent 缩进
     * @return 字符串表示
     */
    QString toString(int indent = 0) const;
    
    /**
     * @brief 添加子节点
     * @param child 子节点
     */
    void addChild(const SyntaxTreeNode &child);
    
    /**
     * @brief 转换为QTreeWidgetItem
     * @return QTreeWidgetItem指针
     */
    QTreeWidgetItem *toTreeWidgetItem() const;
};

// 词法单元结构
struct Token {
    QString type;        // 类型
    QString value;       // 值
    int line;           // 行号
    int column;         // 列号
    
    /**
     * @brief 重载相等运算符
     * @param other 另一个Token
     * @return 是否相等
     */
    bool operator==(const Token &other) const;
    
    /**
     * @brief 打印词法单元
     * @return 字符串表示
     */
    QString toString() const;
};

// 分析步骤结构
struct AnalysisStep {
    int step;           // 步骤编号
    QString stateStack; // 状态栈
    QString symbolStack; // 符号栈
    QString inputString; // 输入串
    QString action;     // 动作
    
    /**
     * @brief 打印分析步骤
     * @return 字符串表示
     */
    QString toString() const;
};

// 语法分析结果结构
struct AnalysisResult {
    bool success;                      // 是否成功
    QString errorMessage;              // 错误信息
    QList<AnalysisStep> steps;         // 分析步骤列表
    SyntaxTreeNode syntaxTree;         // 语法树
    QList<Token> tokens;               // 词法单元列表
    
    /**
     * @brief 打印分析结果
     * @return 字符串表示
     */
    QString toString() const;
};

// 语法分析器类
class SyntaxAnalyzer
{
public:
    SyntaxAnalyzer();
    ~SyntaxAnalyzer();
    
    /**
     * @brief 设置LR(1)分析表
     * @param lr1table LR(1)分析表指针
     */
    void setLR1Table(LR1Table *lr1table);
    
    /**
     * @brief 设置BNF解析器
     * @param parser BNF解析器指针
     */
    void setParser(BNFParser *parser);
    
    /**
     * @brief 从文件加载词法单元
     * @param filePath 文件路径
     * @return 是否加载成功
     */
    bool loadTokensFromFile(const QString &filePath);
    
    /**
     * @brief 从字符串加载词法单元
     * @param tokenString 词法单元字符串
     * @return 是否加载成功
     */
    bool loadTokensFromString(const QString &tokenString);
    
    /**
     * @brief 执行语法分析
     * @return 分析结果
     */
    AnalysisResult analyze();
    
    /**
     * @brief 获取当前分析结果
     * @return 分析结果
     */
    AnalysisResult getResult() const;
    
    /**
     * @brief 清空结果
     */
    void clear();
    
    /**
     * @brief 获取错误信息
     * @return 错误信息
     */
    QString getErrorMessage() const;
    
    /**
     * @brief 保存语法树为图片
     * @param filePath 文件路径
     * @return 是否保存成功
     */
    bool saveSyntaxTreeToImage(const QString &filePath) const;
    
    /**
     * @brief 保存分析步骤到文件
     * @param filePath 文件路径
     * @return 是否保存成功
     */
    bool saveStepsToFile(const QString &filePath) const;
    
    /**
     * @brief 获取词法单元列表
     * @return 词法单元列表
     */
    QList<Token> getTokens() const;
    
    /**
     * @brief 设置开始符号
     * @param startSymbol 开始符号
     */
    void setStartSymbol(const QString &startSymbol);
    
    /**
     * @brief 获取开始符号
     * @return 开始符号
     */
    QString getStartSymbol() const;

private:
    /**
     * @brief 初始化语法分析
     */
    void initAnalysis();
    
    /**
     * @brief 执行单步分析
     * @param result 分析结果
     * @return 是否继续
     */
    bool stepAnalysis(AnalysisResult &result);
    
    /**
     * @brief 处理移进行动
     * @param action 动作
     * @param result 分析结果
     * @return 是否成功
     */
    bool handleShiftAction(const Action &action, AnalysisResult &result);
    
    /**
     * @brief 处理归约动作
     * @param action 动作
     * @param result 分析结果
     * @return 是否成功
     */
    bool handleReduceAction(const Action &action, AnalysisResult &result);
    
    /**
     * @brief 处理接受动作
     * @param result 分析结果
     * @return 是否成功
     */
    bool handleAcceptAction(AnalysisResult &result);
    
    /**
     * @brief 处理GOTO动作
     * @param nonTerminal 非终结符
     * @param result 分析结果
     * @return 是否成功
     */
    bool handleGotoAction(const QString &nonTerminal, AnalysisResult &result);
    
    /**
     * @brief 更新语法树
     * @param result 分析结果
     * @param productionIndex 产生式编号
     */
    void updateSyntaxTree(AnalysisResult &result, int productionIndex);
    
    /**
     * @brief 解析词法单元字符串
     * @param tokenString 词法单元字符串
     * @return 词法单元列表
     */
    QList<Token> parseTokens(const QString &tokenString) const;
    
    /**
     * @brief 获取当前输入符号
     * @return 输入符号
     */
    QString getCurrentInputSymbol() const;
    
    /**
     * @brief 移动到下一个输入符号
     */
    void nextInputSymbol();
    
    /**
     * @brief 状态栈转为字符串
     * @return 字符串表示
     */
    QString stateStackToString() const;
    
    /**
     * @brief 符号栈转为字符串
     * @return 字符串表示
     */
    QString symbolStackToString() const;
    
    /**
     * @brief 剩余输入转为字符串
     * @return 字符串表示
     */
    QString remainingInputToString() const;

private:
    LR1Table *m_lr1table;                     // LR(1)分析表指针
    BNFParser *m_parser;                     // BNF解析器指针
    AnalysisResult m_result;                 // 分析结果
    QString m_errorMessage;                  // 错误信息
    
    // 分析过程中的临时变量
    QList<int> m_stateStack;                 // 状态栈
    QList<QString> m_symbolStack;            // 符号栈
    QList<Token> m_tokens;                   // 词法单元列表
    int m_currentTokenIndex;                 // 当前词法单元索引
    QString m_startSymbol;                   // 开始符号
    
    // 语法树相关
    SyntaxTreeNode m_syntaxTree;             // 语法树
    QList<SyntaxTreeNode*> m_nodeStack;      // 节点栈
    
    // 常量
    const QString m_endMarker;               // 结束标记
};

#endif // SYNTAXANALYZER_H