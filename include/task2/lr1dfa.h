#ifndef LR1DFA_H
#define LR1DFA_H

#include <QString>
#include <QSet>
#include <QList>
#include <QMap>

// 包含BNFParser头文件以获取Production定义
#include "bnfparser.h"
class BNFParser;
class FirstFollow;

// LR(1)项目结构
struct LR1Item {
    Production production;          // 产生式
    int dotPosition;              // 点的位置
    QSet<QString> lookaheadSymbols; // 向前看符号集合
    int productionIndex;          // 产生式编号
    
    /**
     * @brief 重载相等运算符
     * @param other 另一个LR1Item
     * @return 是否相等
     */
    bool operator==(const LR1Item &other) const;
    
    /**
     * @brief 重载小于运算符，用于排序
     * @param other 另一个LR1Item
     * @return 是否小于
     */
    bool operator<(const LR1Item &other) const;
    
    /**
     * @brief 获取点后的符号
     * @return 点后的符号
     */
    QString getSymbolAfterDot() const;
    
    /**
     * @brief 检查点是否在产生式末尾
     * @return 是否在末尾
     */
    bool isDotAtEnd() const;
    
    /**
     * @brief 移动点到下一个位置
     * @return 新的LR1Item
     */
    LR1Item moveDotForward() const;
    
    /**
     * @brief 合并向前看符号
     * @param other 另一个LR1Item
     * @return 合并后的LR1Item
     */
    LR1Item mergeLookahead(const LR1Item &other) const;
    
    /**
     * @brief 打印LR1Item
     * @return 字符串表示
     */
    QString toString() const;
    
    /**
     * @brief 检查两个项目是否具有相同的产生式和点位置
     * @param other 另一个LR1Item
     * @return 是否相同
     */
    bool hasSameCore(const LR1Item &other) const;
};

/**
 * @brief 为LR1Item提供qHash函数，以便在QSet和QMap中使用
 * @param item LR1Item实例
 * @return 哈希值
 */
inline uint qHash(const LR1Item &item, uint seed = 0)
{
    // 对lookaheadSymbols集合进行哈希计算
    uint lookaheadHash = seed;
    for (const QString &symbol : item.lookaheadSymbols) {
        lookaheadHash ^= qHash(symbol, seed);
    }
    return qHash(item.productionIndex, seed) ^ qHash(item.dotPosition, seed) ^ lookaheadHash;
}

// LR(1)项目集结构
struct LR1ItemSet {
    QSet<LR1Item> items;    // 项目集合
    int stateId;            // 状态ID
    
    /**
     * @brief 重载相等运算符
     * @param other 另一个LR1ItemSet
     * @return 是否相等
     */
    bool operator==(const LR1ItemSet &other) const;
    
    /**
     * @brief 重载小于运算符，用于排序
     * @param other 另一个LR1ItemSet
     * @return 是否小于
     */
    bool operator<(const LR1ItemSet &other) const;
    
    /**
     * @brief 合并另一个项目集
     * @param other 另一个项目集
     */
    void merge(const LR1ItemSet &other);
    
    /**
     * @brief 检查是否包含指定核心项目
     * @param item 项目
     * @return 是否包含
     */
    bool containsCoreItem(const LR1Item &item) const;
    
    /**
     * @brief 打印项目集
     * @return 字符串表示
     */
    QString toString() const;
    
    /**
     * @brief 获取点后的所有符号
     * @return 符号集合
     */
    QSet<QString> getSymbolsAfterDot() const;
    
    /**
     * @brief 根据点后的符号获取项目子集
     * @param symbol 符号
     * @return 项目子集
     */
    LR1ItemSet getSubsetBySymbol(const QString &symbol) const;
    
    /**
     * @brief 根据核心项目获取项目
     * @param item 核心项目
     * @return 项目
     */
    LR1Item getItemByCore(const LR1Item &item) const;
};

// LR(1) DFA状态转移结构
struct LR1Transition {
    int fromState;          // 起始状态
    QString symbol;         // 输入符号
    int toState;            // 目标状态
    
    /**
     * @brief 重载相等运算符
     * @param other 另一个LR1Transition
     * @return 是否相等
     */
    bool operator==(const LR1Transition &other) const;
    
    /**
     * @brief 打印状态转移
     * @return 字符串表示
     */
    QString toString() const;
};

// LR(1) DFA生成类
class LR1DFA
{
public:
    LR1DFA();
    ~LR1DFA();
    
    /**
     * @brief 设置BNF解析器
     * @param parser BNF解析器指针
     */
    void setParser(BNFParser *parser);
    
    /**
     * @brief 设置FirstFollow对象
     * @param firstFollow FirstFollow对象指针
     */
    void setFirstFollow(FirstFollow *firstFollow);
    
    /**
     * @brief 生成LR(1) DFA
     * @return 是否生成成功
     */
    bool generate();
    
    /**
     * @brief 获取项目集规范族
     * @return 项目集规范族
     */
    QList<LR1ItemSet> getItemSets() const;
    
    /**
     * @brief 获取状态转移表
     * @return 状态转移表
     */
    QList<LR1Transition> getTransitions() const;
    
    /**
     * @brief 获取状态数量
     * @return 状态数量
     */
    int getStateCount() const;
    
    /**
     * @brief 获取开始状态ID
     * @return 开始状态ID
     */
    int getStartStateId() const;
    
    /**
     * @brief 获取接受状态ID
     * @return 接受状态ID
     */
    int getAcceptStateId() const;
    
    /**
     * @brief 获取错误信息
     * @return 错误信息
     */
    QString getErrorMessage() const;
    
    /**
     * @brief 清空结果
     */
    void clear();
    
    /**
     * @brief 打印LR(1) DFA信息
     * @return 字符串表示
     */
    QString printDFA() const;
    
    /**
     * @brief 获取指定状态的项目集
     * @param stateId 状态ID
     * @return 项目集
     */
    LR1ItemSet getItemSetByStateId(int stateId) const;
    
    /**
     * @brief 根据当前状态和输入符号获取下一个状态
     * @param stateId 当前状态ID
     * @param symbol 输入符号
     * @return 下一个状态ID，-1表示不存在
     */
    int getNextState(int stateId, const QString &symbol) const;

private:
    /**
     * @brief 计算项目集的闭包
     * @param itemSet 项目集
     * @return 闭包后的项目集
     */
    LR1ItemSet closure(const LR1ItemSet &itemSet) const;
    
    /**
     * @brief 执行GOTO操作
     * @param itemSet 项目集
     * @param symbol 输入符号
     * @return GOTO操作后的项目集
     */
    LR1ItemSet gotoOperation(const LR1ItemSet &itemSet, const QString &symbol) const;
    
    /**
     * @brief 初始化开始项目集
     * @return 开始项目集
     */
    LR1ItemSet initStartItemSet() const;
    
    /**
     * @brief 查找项目集在规范族中的索引
     * @param itemSet 项目集
     * @return 索引，-1表示不存在
     */
    int findItemSetIndex(const LR1ItemSet &itemSet) const;
    
    /**
     * @brief 计算FIRST集合
     * @param symbols 符号串
     * @param lookahead 向前看符号
     * @return FIRST集合
     */
    QSet<QString> computeFirst(const QStringList &symbols, const QSet<QString> &lookahead) const;
    
    /**
     * @brief 检查是否是接受项目
     * @param item 项目
     * @return 是否是接受项目
     */
    bool isAcceptItem(const LR1Item &item) const;
    
    /**
     * @brief 生成状态转移表
     * @return 是否生成成功
     */
    bool generateTransitions();
    
    /**
     * @brief 获取所有产生式
     * @return 产生式列表
     */
    QList<Production> getProductions() const;
    
    /**
     * @brief 获取所有非终结符
     * @return 非终结符集合
     */
    QSet<QString> getNonTerminals() const;
    
    /**
     * @brief 获取所有终结符
     * @return 终结符集合
     */
    QSet<QString> getTerminals() const;
    
    /**
     * @brief 获取文法开始符号
     * @return 文法开始符号
     */
    QString getStartSymbol() const;

private:
    BNFParser *m_parser;                   // BNF解析器指针
    FirstFollow *m_firstFollow;            // FirstFollow对象指针
    QList<LR1ItemSet> m_itemSets;         // 项目集规范族
    QList<LR1Transition> m_transitions;    // 状态转移表
    int m_stateCount;                      // 状态数量
    int m_startStateId;                    // 开始状态ID
    int m_acceptStateId;                   // 接受状态ID
    QString m_errorMessage;                // 错误信息
    QMap<int, LR1ItemSet> m_stateToItemSetMap; // 状态ID到项目集的映射
    QMap<LR1ItemSet, int> m_itemSetToStateMap; // 项目集到状态ID的映射
};

#endif // LR1DFA_H