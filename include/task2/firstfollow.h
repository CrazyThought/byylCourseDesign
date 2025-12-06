#ifndef FIRSTFOLLOW_H
#define FIRSTFOLLOW_H

#include <QString>
#include <QSet>
#include <QMap>
#include <QList>

// 前向声明
class BNFParser;
struct Production;

// First集合与Follow集合计算类
class FirstFollow
{
public:
    FirstFollow();
    ~FirstFollow();
    
    /**
     * @brief 设置BNF解析器
     * @param parser BNF解析器指针
     */
    void setParser(BNFParser *parser);
    
    /**
     * @brief 计算所有非终结符的First集合
     * @return 是否计算成功
     */
    bool calculateFirst();
    
    /**
     * @brief 计算所有非终结符的Follow集合
     * @return 是否计算成功
     */
    bool calculateFollow();
    
    /**
     * @brief 获取指定非终结符的First集合
     * @param nonTerminal 非终结符
     * @return First集合
     */
    QSet<QString> getFirst(const QString &nonTerminal) const;
    
    /**
     * @brief 获取指定符号串的First集合
     * @param symbols 符号串
     * @return First集合
     */
    QSet<QString> getFirstOfString(const QStringList &symbols) const;
    
    /**
     * @brief 获取指定非终结符的Follow集合
     * @param nonTerminal 非终结符
     * @return Follow集合
     */
    QSet<QString> getFollow(const QString &nonTerminal) const;
    
    /**
     * @brief 获取所有First集合
     * @return First集合映射
     */
    QMap<QString, QSet<QString>> getAllFirst() const;
    
    /**
     * @brief 获取所有Follow集合
     * @return Follow集合映射
     */
    QMap<QString, QSet<QString>> getAllFollow() const;
    
    /**
     * @brief 获取错误信息
     * @return 错误信息
     */
    QString getErrorMessage() const;
    
    /**
     * @brief 清空计算结果
     */
    void clear();
    
    /**
     * @brief 检查符号是否为终结符
     * @param symbol 符号
     * @return 是否为终结符
     */
    bool isTerminal(const QString &symbol) const;
    
    /**
     * @brief 检查符号是否为非终结符
     * @param symbol 符号
     * @return 是否为非终结符
     */
    bool isNonTerminal(const QString &symbol) const;
    
    /**
     * @brief 检查符号是否为空串（#）
     * @param symbol 符号
     * @return 是否为空串
     */
    bool isEmptyString(const QString &symbol) const;
    
    /**
     * @brief 打印First集合
     * @return First集合字符串
     */
    QString printFirst() const;
    
    /**
     * @brief 打印Follow集合
     * @return Follow集合字符串
     */
    QString printFollow() const;
    
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
    
    /**
     * @brief 获取产生式列表
     * @return 产生式列表
     */
    QList<Production> getProductions() const;

private:
    /**
     * @brief 初始化First集合
     */
    void initFirst();
    
    /**
     * @brief 初始化Follow集合
     */
    void initFollow();
    
    /**
     * @brief 计算单个非终结符的First集合
     * @param nonTerminal 非终结符
     * @return 是否计算成功
     */
    bool calculateSingleFirst(const QString &nonTerminal);
    
    /**
     * @brief 计算单个非终结符的Follow集合
     * @param nonTerminal 非终结符
     * @return 是否计算成功
     */
    bool calculateSingleFollow(const QString &nonTerminal);
    
    /**
     * @brief 更新Follow集合
     * @param nonTerminal 非终结符
     * @param followSet Follow集合
     * @return 是否有更新
     */
    bool updateFollow(const QString &nonTerminal, const QSet<QString> &followSet);
    
    /**
     * @brief 更新First集合
     * @param nonTerminal 非终结符
     * @param firstSet First集合
     * @return 是否有更新
     */
    bool updateFirst(const QString &nonTerminal, const QSet<QString> &firstSet);
    
    /**
     * @brief 检查First集合是否包含空串
     * @param symbols 符号串
     * @return 是否包含空串
     */
    bool containsEmpty(const QStringList &symbols) const;
    
    /**
     * @brief 合并两个集合
     * @param set1 集合1
     * @param set2 集合2
     * @return 合并后的集合
     */
    QSet<QString> mergeSets(const QSet<QString> &set1, const QSet<QString> &set2) const;
    
    /**
     * @brief 从集合中移除空串
     * @param set 集合
     * @return 移除空串后的集合
     */
    QSet<QString> removeEmpty(const QSet<QString> &set) const;

private:
    BNFParser *m_parser;                   // BNF解析器指针
    QMap<QString, QSet<QString>> m_first;  // First集合映射
    QMap<QString, QSet<QString>> m_follow; // Follow集合映射
    QString m_errorMessage;                // 错误信息
    bool m_hasEmptyProduction;             // 是否有空产生式
    const QString m_emptyString;           // 空串表示
};

#endif // FIRSTFOLLOW_H