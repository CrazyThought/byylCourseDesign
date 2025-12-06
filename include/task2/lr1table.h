#ifndef LR1TABLE_H
#define LR1TABLE_H

#include <QString>
#include <QSet>
#include <QList>
#include <QMap>

// 前向声明
class LR1DFA;
class BNFParser;
struct LR1ItemSet;
struct LR1Item;

// 动作类型枚举
enum ActionType {
    ACTION_ERROR,       // 错误
    ACTION_SHIFT,       // 移进
    ACTION_REDUCE,      // 归约
    ACTION_ACCEPT,      // 接受
    ACTION_GOTO         // GOTO
};

// 动作结构
struct Action {
    ActionType type;     // 动作类型
    int value;          // 动作值（移进状态或归约产生式编号）
    
    /**
     * @brief 重载相等运算符
     * @param other 另一个Action
     * @return 是否相等
     */
    bool operator==(const Action &other) const;
    
    /**
     * @brief 打印动作
     * @return 字符串表示
     */
    QString toString() const;
};

// LR(1)分析表类
class LR1Table
{
public:
    LR1Table();
    ~LR1Table();
    
    /**
     * @brief 设置LR(1) DFA
     * @param lr1dfa LR(1) DFA指针
     */
    void setLR1DFA(LR1DFA *lr1dfa);
    
    /**
     * @brief 设置BNF解析器
     * @param parser BNF解析器指针
     */
    void setParser(BNFParser *parser);
    
    /**
     * @brief 生成LR(1)分析表
     * @return 是否生成成功
     */
    bool generateLR1Table();
    
    /**
     * @brief 获取Action表
     * @return Action表，格式：状态ID -> (符号 -> 动作)
     */
    QMap<int, QMap<QString, Action>> getActionTable() const;
    
    /**
     * @brief 获取Goto表
     * @return Goto表，格式：状态ID -> (非终结符 -> 目标状态)
     */
    QMap<int, QMap<QString, int>> getGotoTable() const;
    
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
     * @brief 打印LR(1)分析表
     * @return 字符串表示
     */
    QString printLR1Table() const;
    
    /**
     * @brief 根据状态和符号获取动作
     * @param stateId 状态ID
     * @param symbol 符号
     * @return 动作
     */
    Action getAction(int stateId, const QString &symbol) const;
    
    /**
     * @brief 根据状态和非终结符获取Goto目标
     * @param stateId 状态ID
     * @param nonTerminal 非终结符
     * @return 目标状态ID，-1表示不存在
     */
    int getGoto(int stateId, const QString &nonTerminal) const;
    
    /**
     * @brief 获取所有状态ID
     * @return 状态ID列表
     */
    QList<int> getAllStates() const;
    
    /**
     * @brief 获取所有终结符
     * @return 终结符集合
     */
    QSet<QString> getAllTerminals() const;
    
    /**
     * @brief 获取所有非终结符
     * @return 非终结符集合
     */
    QSet<QString> getAllNonTerminals() const;
    
    /**
     * @brief 保存LR(1)分析表到CSV文件
     * @param filePath 文件路径
     * @return 是否保存成功
     */
    bool saveToCSV(const QString &filePath) const;
    
    /**
     * @brief 检查LR(1)分析表是否有冲突
     * @return 是否有冲突
     */
    bool hasConflicts() const;
    
    /**
     * @brief 获取冲突数量
     * @return 冲突数量
     */
    int getConflictCount() const;
    
    /**
     * @brief 获取冲突信息
     * @return 冲突信息列表
     */
    QList<QString> getConflictInfo() const;

private:
    /**
     * @brief 处理移进行动
     * @param stateId 状态ID
     * @param itemSet 项目集
     * @return 是否处理成功
     */
    bool handleShiftActions(int stateId, const LR1ItemSet &itemSet);
    
    /**
     * @brief 处理归约动作
     * @param stateId 状态ID
     * @param itemSet 项目集
     * @return 是否处理成功
     */
    bool handleReduceActions(int stateId, const LR1ItemSet &itemSet);
    
    /**
     * @brief 处理接受动作
     * @param stateId 状态ID
     * @param itemSet 项目集
     * @return 是否处理成功
     */
    bool handleAcceptActions(int stateId, const LR1ItemSet &itemSet);
    
    /**
     * @brief 处理Goto动作
     * @param stateId 状态ID
     * @return 是否处理成功
     */
    bool handleGotoActions(int stateId);
    
    /**
     * @brief 检查动作是否有冲突
     * @param stateId 状态ID
     * @param symbol 符号
     * @param newAction 新动作
     * @return 是否有冲突
     */
    bool checkActionConflict(int stateId, const QString &symbol, const Action &newAction);
    
    /**
     * @brief 从项目集中获取归约项目
     * @param itemSet 项目集
     * @return 归约项目列表
     */
    QList<LR1Item> getReduceItems(const LR1ItemSet &itemSet) const;
    
    /**
     * @brief 从项目集中获取接受项目
     * @param itemSet 项目集
     * @return 接受项目列表
     */
    QList<LR1Item> getAcceptItems(const LR1ItemSet &itemSet) const;
    
    /**
     * @brief 获取所有终结符
     * @return 终结符集合
     */
    QSet<QString> getTerminals() const;
    
    /**
     * @brief 获取所有非终结符
     * @return 非终结符集合
     */
    QSet<QString> getNonTerminals() const;
    
    /**
     * @brief 获取所有产生式
     * @return 产生式列表
     */
    QList<Production> getProductions() const;
    
    /**
     * @brief 根据产生式索引获取产生式
     * @param index 产生式索引
     * @return 产生式
     */
    Production getProductionByIndex(int index) const;

private:
    LR1DFA *m_lr1dfa;                        // LR(1) DFA指针
    BNFParser *m_parser;                     // BNF解析器指针
    QMap<int, QMap<QString, Action>> m_actionTable; // Action表
    QMap<int, QMap<QString, int>> m_gotoTable;    // Goto表
    QString m_errorMessage;                  // 错误信息
    QList<QString> m_conflicts;              // 冲突信息列表
    int m_conflictCount;                     // 冲突数量
    QSet<QString> m_terminals;              // 终结符集合
    QSet<QString> m_nonTerminals;           // 非终结符集合
    QList<Production> m_productions;        // 产生式列表
};

#endif // LR1TABLE_H