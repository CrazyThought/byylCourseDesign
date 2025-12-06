#ifndef SLR1CHECKER_H
#define SLR1CHECKER_H

#include <QString>
#include <QSet>
#include <QList>
#include <QMap>

// 前向声明
class LR0DFA;
class FirstFollow;
struct LR0ItemSet;
struct LR0Item;

// 冲突类型枚举
enum ConflictType {
    NO_CONFLICT,          // 无冲突
    SHIFT_REDUCE_CONFLICT, // 移进-归约冲突
    REDUCE_REDUCE_CONFLICT // 归约-归约冲突
};

// 冲突信息结构
struct ConflictInfo {
    ConflictType type;         // 冲突类型
    int stateId;              // 冲突所在状态ID
    QString symbol;           // 冲突符号
    QList<int> reduceProductions; // 归约产生式列表
    int shiftState;           // 移进目标状态
    
    /**
     * @brief 打印冲突信息
     * @return 字符串表示
     */
    QString toString() const;
};

// SLR(1)文法判断类
class SLR1Checker
{
public:
    SLR1Checker();
    ~SLR1Checker();
    
    /**
     * @brief 设置LR(0) DFA
     * @param lr0dfa LR(0) DFA指针
     */
    void setLR0DFA(LR0DFA *lr0dfa);
    
    /**
     * @brief 设置FirstFollow对象
     * @param firstFollow FirstFollow对象指针
     */
    void setFirstFollow(FirstFollow *firstFollow);
    
    /**
     * @brief 判断文法是否为SLR(1)文法
     * @return 是否为SLR(1)文法
     */
    bool checkSLR1();
    
    /**
     * @brief 获取冲突信息列表
     * @return 冲突信息列表
     */
    QList<ConflictInfo> getConflicts() const;
    
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
     * @brief 打印冲突信息
     * @return 字符串表示
     */
    QString printConflicts() const;
    
    /**
     * @brief 获取冲突数量
     * @return 冲突数量
     */
    int getConflictCount() const;
    
    /**
     * @brief 检查是否有移进-归约冲突
     * @return 是否有移进-归约冲突
     */
    bool hasShiftReduceConflict() const;
    
    /**
     * @brief 检查是否有归约-归约冲突
     * @return 是否有归约-归约冲突
     */
    bool hasReduceReduceConflict() const;
    
    /**
     * @brief 获取移进-归约冲突数量
     * @return 移进-归约冲突数量
     */
    int getShiftReduceConflictCount() const;
    
    /**
     * @brief 获取归约-归约冲突数量
     * @return 归约-归约冲突数量
     */
    int getReduceReduceConflictCount() const;
    
    /**
     * @brief 获取SLR(1)分析表
     * @return SLR(1)分析表，格式：状态ID -> (符号 -> 动作)
     */
    QMap<int, QMap<QString, QString>> getSLR1Table() const;

private:
    /**
     * @brief 检查单个项目集是否有冲突
     * @param itemSet 项目集
     * @param stateId 状态ID
     * @return 冲突信息列表
     */
    QList<ConflictInfo> checkItemSet(const LR0ItemSet &itemSet, int stateId);
    
    /**
     * @brief 检查是否存在移进-归约冲突
     * @param itemSet 项目集
     * @param stateId 状态ID
     * @param symbol 符号
     * @param reduceProductions 归约产生式列表
     * @return 冲突信息，无冲突返回空
     */
    ConflictInfo checkShiftReduceConflict(const LR0ItemSet &itemSet, int stateId, const QString &symbol, const QList<int> &reduceProductions);
    
    /**
     * @brief 检查是否存在归约-归约冲突
     * @param itemSet 项目集
     * @param stateId 状态ID
     * @param symbol 符号
     * @param reduceProductions 归约产生式列表
     * @return 冲突信息，无冲突返回空
     */
    ConflictInfo checkReduceReduceConflict(const LR0ItemSet &itemSet, int stateId, const QString &symbol, const QList<int> &reduceProductions);
    
    /**
     * @brief 从项目集中获取所有归约项目
     * @param itemSet 项目集
     * @return 归约项目列表
     */
    QList<LR0Item> getReduceItems(const LR0ItemSet &itemSet) const;
    
    /**
     * @brief 从项目集中获取所有移进项目
     * @param itemSet 项目集
     * @return 移进项目列表
     */
    QList<LR0Item> getShiftItems(const LR0ItemSet &itemSet) const;
    
    /**
     * @brief 获取归约项目的Follow集合
     * @param item 归约项目
     * @return Follow集合
     */
    QSet<QString> getFollowSetForReduceItem(const LR0Item &item) const;
    
    /**
     * @brief 检查符号是否在Follow集合中
     * @param symbol 符号
     * @param followSet Follow集合
     * @return 是否在Follow集合中
     */
    bool isSymbolInFollowSet(const QString &symbol, const QSet<QString> &followSet) const;
    
    /**
     * @brief 生成SLR(1)分析表
     * @return 是否生成成功
     */
    bool generateSLR1Table();
    
    /**
     * @brief 添加移进行动到分析表
     * @param stateId 状态ID
     * @param symbol 符号
     * @param nextState 下一个状态
     */
    void addShiftAction(int stateId, const QString &symbol, int nextState);
    
    /**
     * @brief 添加归约行动到分析表
     * @param stateId 状态ID
     * @param symbol 符号
     * @param productionIndex 产生式编号
     */
    void addReduceAction(int stateId, const QString &symbol, int productionIndex);
    
    /**
     * @brief 添加接受行动到分析表
     * @param stateId 状态ID
     */
    void addAcceptAction(int stateId);
    
    /**
     * @brief 添加GOTO行动到分析表
     * @param stateId 状态ID
     * @param symbol 符号
     * @param nextState 下一个状态
     */
    void addGotoAction(int stateId, const QString &symbol, int nextState);
    
    /**
     * @brief 获取移进符号集合
     * @param itemSet 项目集
     * @return 移进符号集合
     */
    QSet<QString> getShiftSymbols(const LR0ItemSet &itemSet) const;
    
    /**
     * @brief 获取归约符号集合
     * @param itemSet 项目集
     * @return 归约符号集合
     */
    QSet<QString> getReduceSymbols(const LR0ItemSet &itemSet) const;

private:
    LR0DFA *m_lr0dfa;                    // LR(0) DFA指针
    FirstFollow *m_firstFollow;          // FirstFollow对象指针
    QList<ConflictInfo> m_conflicts;     // 冲突信息列表
    QString m_errorMessage;              // 错误信息
    bool m_isSLR1;                      // 是否为SLR(1)文法
    QMap<int, QMap<QString, QString>> m_slr1Table; // SLR(1)分析表
};

#endif // SLR1CHECKER_H