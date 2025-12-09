#ifndef LEXERGENERATOR_H
#define LEXERGENERATOR_H

#include <QString>
#include <QList>
#include <QMap>
#include <QSet>
#include "regexprocessor.h" // 包含正则表达式信息
#include "dfabuilder.h" // 包含DFA的定义

// 生成方法枚举
enum class GenerationMethod {
    DIRECT_MATCH,     // 直接匹配逻辑
    STATE_TRANSITION  // 状态转移逻辑
};

class LexerGenerator
{
public:
    LexerGenerator();
    ~LexerGenerator();
    
    // 生成词法分析器代码
    QString generateLexer(const QList<RegexItem> &regexItems, const DFA &minimizedDFA, GenerationMethod method);
    
    // 生成token映射内容
    QString generateTokenMap(const QList<RegexItem> &regexItems);
    
    // 保存token映射到文件
    bool saveTokenMap(const QList<RegexItem> &regexItems, const QString &outputPath);
    
    // 获取错误信息
    QString getErrorMessage() const;
    
private:
    // 生成直接匹配逻辑的词法分析器
    QString generateDirectMatchLexer(const QList<RegexItem> &regexItems, const DFA &minimizedDFA);
    
    // 生成状态转移逻辑的词法分析器
    QString generateStateTransitionLexer(const QList<RegexItem> &regexItems, const DFA &minimizedDFA);
    
    // 生成DFA状态转移表
    QString generateStateTransitionTable(const DFA &minimizedDFA);
    
    // 生成接受状态映射
    QString generateAcceptStatesMap(const QList<RegexItem> &regexItems, const DFA &minimizedDFA);
    
    // 生成单词编码映射
    QString generateTokenCodeMap(const QList<RegexItem> &regexItems);
    
    // 获取字符对应的输入符号
    QString getInputSymbol(const QString &input);
    
    // 错误信息
    QString m_errorMessage;
    
    // 正则表达式列表
    QList<RegexItem> m_regexItems;
};

#endif // LEXERGENERATOR_H
