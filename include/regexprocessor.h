#ifndef REGEXPROCESSOR_H
#define REGEXPROCESSOR_H

#include <QString>
#include <QList>
#include <QMap>
#include <QRegularExpression>

// 正则表达式项结构
typedef struct {
    QString name;          // 正则表达式名称
    QString pattern;       // 正则表达式模式
    int code;              // 单词编码
    bool isMultiWord;      // 是否为多单词
} RegexItem;

class RegexProcessor
{
public:
    RegexProcessor();
    ~RegexProcessor();

    // 解析正则表达式文本
    bool parse(const QString &text);

    // 获取解析后的正则表达式列表
    QList<RegexItem> getRegexItems() const;

    // 获取错误信息
    QString getErrorMessage() const;

private:
    // 解析单行正则表达式
    bool parseLine(const QString &line, RegexItem &item);

    // 验证命名规则
    bool validateName(const QString &name, int &code, bool &isMultiWord);

    // 处理转义字符
    QString processEscapeCharacters(const QString &pattern);

    // 检查正则表达式语法
    bool checkRegexSyntax(const QString &pattern);

    // 转换为标准正则表达式格式
    QString convertToStandardRegex(const QString &pattern);

private:
    QList<RegexItem> m_regexItems;
    QString m_errorMessage;
};

#endif // REGEXPROCESSOR_H