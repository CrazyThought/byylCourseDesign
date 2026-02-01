/*
 * @file lexergenerator.h
 * @id lexergenerator-h
 * @brief 提供词法分析器生成功能，根据正则表达式和DFA生成词法分析器代码
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#ifndef LEXERGENERATOR_H
#define LEXERGENERATOR_H

#include <QString>
#include <QList>
#include <QMap>
#include <QSet>
#include "regexprocessor.h" // 包含正则表达式信息
#include "dfabuilder.h" // 包含DFA的定义

/**
 * @brief 生成方法枚举
 * 
 * 定义词法分析器的生成方法
 */
enum class GenerationMethod {
    DIRECT_MATCH,     ///< 直接匹配逻辑，适用于简单正则表达式
    STATE_TRANSITION  ///< 状态转移逻辑，基于DFA状态机实现
};

/**
 * @brief 词法分析器生成器类
 * 
 * 根据正则表达式和最小化DFA生成词法分析器代码
 */
class LexerGenerator
{
public:
    /**
     * @brief 构造函数
     */
    LexerGenerator();
    
    /**
     * @brief 析构函数
     */
    ~LexerGenerator();
    
    /**
     * @brief 生成词法分析器代码
     * 
     * 根据指定的生成方法，生成词法分析器代码
     * 
     * @param regexItems 正则表达式项列表
     * @param minimizedDFA 最小化的DFA
     * @param method 生成方法
     * @return QString 生成的词法分析器代码
     */
    QString generateLexer(const QList<RegexItem> &regexItems, const DFA &minimizedDFA, GenerationMethod method);
    
    /**
     * @brief 生成token映射内容
     * 
     * 生成token类型到正则表达式的映射字符串
     * 
     * @param regexItems 正则表达式项列表
     * @return QString 生成的token映射内容
     */
    QString generateTokenMap(const QList<RegexItem> &regexItems);
    
    /**
     * @brief 保存token映射到文件
     * 
     * 将token映射内容保存到指定文件
     * 
     * @param regexItems 正则表达式项列表
     * @param outputPath 输出文件路径
     * @return bool 保存成功返回true，失败返回false
     */
    bool saveTokenMap(const QList<RegexItem> &regexItems, const QString &outputPath);
    
    /**
     * @brief 获取错误信息
     * 
     * 当生成过程中出现错误时，获取详细的错误信息
     * 
     * @return QString 错误信息
     */
    QString getErrorMessage() const;
    
private:
    /**
     * @brief 生成直接匹配逻辑的词法分析器
     * 
     * 生成基于直接匹配策略的词法分析器代码
     * 
     * @param regexItems 正则表达式项列表
     * @param minimizedDFA 最小化的DFA
     * @return QString 生成的词法分析器代码
     */
    QString generateDirectMatchLexer(const QList<RegexItem> &regexItems, const DFA &minimizedDFA);
    
    /**
     * @brief 生成状态转移逻辑的词法分析器
     * 
     * 生成基于状态转移表的词法分析器代码
     * 
     * @param regexItems 正则表达式项列表
     * @param minimizedDFA 最小化的DFA
     * @return QString 生成的词法分析器代码
     */
    QString generateStateTransitionLexer(const QList<RegexItem> &regexItems, const DFA &minimizedDFA);
    
    /**
     * @brief 生成DFA状态转移表
     * 
     * 根据最小化DFA生成状态转移表代码
     * 
     * @param minimizedDFA 最小化的DFA
     * @return QString 生成的状态转移表代码
     */
    QString generateStateTransitionTable(const DFA &minimizedDFA);
    
    /**
     * @brief 生成接受状态映射
     * 
     * 生成接受状态到token类型的映射代码
     * 
     * @param regexItems 正则表达式项列表
     * @param minimizedDFA 最小化的DFA
     * @return QString 生成的接受状态映射代码
     */
    QString generateAcceptStatesMap(const QList<RegexItem> &regexItems, const DFA &minimizedDFA);
    
    QString m_errorMessage;  ///< 错误信息
    QList<RegexItem> m_regexItems;  ///< 正则表达式列表
};

#endif // LEXERGENERATOR_H
