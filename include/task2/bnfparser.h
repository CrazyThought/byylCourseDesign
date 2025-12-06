#ifndef BNFPARSER_H
#define BNFPARSER_H

#include <QString>
#include <QList>
#include <QMap>
#include <QSet>

// BNF文法产生式结构
struct Production {
    QString nonTerminal;          // 非终结符
    QList<QStringList> candidates; // 候选式列表，每个候选式是符号列表
};

// BNF文法解析器类
class BNFParser
{
public:
    BNFParser();
    ~BNFParser();
    
    /**
     * @brief 解析BNF文法文本
     * @param text BNF文法文本
     * @return 是否解析成功
     */
    bool parse(const QString &text);
    
    /**
     * @brief 获取解析后的产生式列表
     * @return 产生式列表
     */
    QList<Production> getProductions() const;
    
    /**
     * @brief 获取非终结符集合
     * @return 非终结符集合
     */
    QSet<QString> getNonTerminals() const;
    
    /**
     * @brief 获取终结符集合
     * @return 终结符集合
     */
    QSet<QString> getTerminals() const;
    
    /**
     * @brief 获取文法开始符号
     * @return 文法开始符号
     */
    QString getStartSymbol() const;
    
    /**
     * @brief 获取错误信息
     * @return 错误信息
     */
    QString getErrorMessage() const;
    
    /**
     * @brief 加载BNF文法文件
     * @param filePath 文件路径
     * @return 是否加载成功
     */
    bool loadFromFile(const QString &filePath);
    
    /**
     * @brief 保存BNF文法到文件
     * @param filePath 文件路径
     * @return 是否保存成功
     */
    bool saveToFile(const QString &filePath) const;
    
    /**
     * @brief 清空解析结果
     */
    void clear();
    
    /**
     * @brief 验证文法格式
     * @return 是否格式正确
     */
    bool validateGrammar() const;
    
    /**
     * @brief 打印文法信息
     * @return 文法信息字符串
     */
    QString printGrammar() const;
    
    /**
     * @brief 根据非终结符获取其产生式
     * @param nonTerminal 非终结符
     * @return 产生式列表
     */
    QList<Production> getProductionsByNonTerminal(const QString &nonTerminal) const;

private:
    /**
     * @brief 解析单行产生式
     * @param line 行文本
     * @param production 产生式结构
     * @return 是否解析成功
     */
    bool parseLine(const QString &line, Production &production);
    
    /**
     * @brief 拆分候选式
     * @param candidateStr 候选式字符串
     * @param candidate 候选式符号列表
     * @return 是否拆分成功
     */
    bool splitCandidate(const QString &candidateStr, QStringList &candidate);
    
    /**
     * @brief 提取终结符和非终结符
     */
    void extractSymbols();
    
    /**
     * @brief 检查符号是否为非终结符
     * @param symbol 符号
     * @return 是否为非终结符
     */
    bool isNonTerminal(const QString &symbol) const;
    
    /**
     * @brief 检查符号是否为终结符
     * @param symbol 符号
     * @return 是否为终结符
     */
    bool isTerminal(const QString &symbol) const;
    
private:
    QList<Production> m_productions;    // 解析后的产生式列表
    QSet<QString> m_nonTerminals;        // 非终结符集合
    QSet<QString> m_terminals;           // 终结符集合
    QString m_startSymbol;               // 文法开始符号
    QString m_errorMessage;              // 错误信息
    QString m_originalText;              // 原始文本
    QMap<QString, int> m_productionIndices; // 产生式索引映射
};

#endif // BNFPARSER_H