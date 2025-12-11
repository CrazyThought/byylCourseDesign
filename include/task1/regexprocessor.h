/*
 * @file regexprocessor.h
 * @id regexprocessor-h
 * @brief 提供正则表达式处理功能，包括正则表达式的解析、验证和转换
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#ifndef REGEXPROCESSOR_H
#define REGEXPROCESSOR_H

#include <QString>
#include <QList>
#include <QMap>
#include <QStringList>

/**
 * @brief 正则表达式项结构
 * 
 * 表示单个正则表达式的完整信息，包括名称、模式、编码等
 */
typedef struct {
    QString name;          ///< 正则表达式名称
    QString pattern;       ///< 正则表达式模式
    int code;              ///< 单词编码
    bool isMultiWord;      ///< 是否为多单词模式
    QStringList wordList;  ///< 匹配的完整单词列表
} RegexItem;

/**
 * @brief 正则表达式处理器类
 * 
 * 负责解析、验证和转换正则表达式文本
 */
class RegexProcessor
{
public:
    /**
     * @brief 构造函数
     */
    RegexProcessor();
    
    /**
     * @brief 析构函数
     */
    ~RegexProcessor();

    /**
     * @brief 解析正则表达式文本
     * 
     * 将正则表达式文本解析为RegexItem列表
     * 
     * @param text 正则表达式文本
     * @return bool 解析成功返回true，失败返回false
     */
    bool parse(const QString &text);

    /**
     * @brief 获取解析后的正则表达式列表
     * 
     * @return QList<RegexItem> 解析后的正则表达式列表
     */
    QList<RegexItem> getRegexItems() const;

    /**
     * @brief 获取错误信息
     * 
     * @return QString 错误信息
     */
    QString getErrorMessage() const;

private:
    /**
     * @brief 解析单行正则表达式
     * 
     * @param line 单行正则表达式文本
     * @param item 输出参数，存储解析结果
     * @return bool 解析成功返回true，失败返回false
     */
    bool parseLine(const QString &line, RegexItem &item);

    /**
     * @brief 验证命名规则
     * 
     * 验证正则表达式名称是否符合规则，并提取编码和多单词标志
     * 
     * @param name 正则表达式名称
     * @param code 输出参数，存储提取的编码
     * @param isMultiWord 输出参数，存储是否为多单词
     * @return bool 验证成功返回true，失败返回false
     */
    bool validateName(const QString &name, int &code, bool &isMultiWord);

    /**
     * @brief 处理转义字符
     * 
     * 处理正则表达式模式中的转义字符
     * 
     * @param pattern 原始正则表达式模式
     * @return QString 处理后的正则表达式模式
     */
    QString processEscapeCharacters(const QString &pattern);

    /**
     * @brief 检查正则表达式语法
     * 
     * 检查正则表达式语法是否正确
     * 
     * @param pattern 正则表达式模式
     * @return bool 语法正确返回true，否则返回false
     */
    bool checkRegexSyntax(const QString &pattern);

    /**
     * @brief 转换为标准正则表达式格式
     * 
     * 将自定义格式的正则表达式转换为标准格式
     * 
     * @param pattern 自定义格式的正则表达式
     * @return QString 标准格式的正则表达式
     */
    QString convertToStandardRegex(const QString &pattern);
    
    /**
     * @brief 提取多单词列表
     * 
     * 从正则表达式模式中提取多单词列表
     * 
     * @param pattern 正则表达式模式
     * @return QStringList 提取的多单词列表
     */
    QStringList extractMultiWords(const QString &pattern);
    
    /**
     * @brief 生成所有可能的大小写组合
     * 
     * 为给定字符串生成所有可能的大小写组合
     * 
     * @param pattern 输入字符串
     * @return QStringList 所有可能的大小写组合
     */
    static QStringList generateAllCaseCombinations(const QString &pattern);
    
    /**
     * @brief 从分组的正则表达式中生成所有可能的单词变体
     * 
     * 从分组的正则表达式中生成所有可能的单词变体
     * 
     * @param pattern 分组的正则表达式
     * @param depth 递归深度，默认为0
     * @return QStringList 生成的所有单词变体
     */
    QStringList generateWordsFromGroupedRegex(const QString &pattern, int depth = 0);

private:
    QList<RegexItem> m_regexItems;  ///< 正则表达式项列表
    QString m_errorMessage;  ///< 错误信息
    QMap<QString, QString> m_regexReferences;  ///< 保存正则表达式引用，如digit=[0-9]
};

#endif // REGEXPROCESSOR_H