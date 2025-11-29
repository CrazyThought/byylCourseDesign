#include "regexprocessor.h"
#include <QStringList>
#include <QRegularExpressionMatch>
#include <QtGlobal> // 为了Qt::SkipEmptyParts

RegexProcessor::RegexProcessor()
{
}

RegexProcessor::~RegexProcessor()
{
}

bool RegexProcessor::parse(const QString &text)
{
    m_regexItems.clear();
    m_errorMessage.clear();

    QStringList lines = text.split(QRegularExpression("[\\r\\n]"), Qt::SkipEmptyParts);
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();
        if (line.isEmpty() || line.startsWith("//")) {
            continue; // 跳过空行和注释行
        }

        RegexItem item;
        if (!parseLine(line, item)) {
            m_errorMessage = QString("第 %1 行解析错误: %2")
                             .arg(i + 1).arg(m_errorMessage);
            return false;
        }
        m_regexItems.append(item);
    }

    return true;
}

QList<RegexItem> RegexProcessor::getRegexItems() const
{
    return m_regexItems;
}

QString RegexProcessor::getErrorMessage() const
{
    return m_errorMessage;
}

bool RegexProcessor::parseLine(const QString &line, RegexItem &item)
{
    // 寻找等号位置
    int equalsPos = line.indexOf('=');
    if (equalsPos == -1) {
        m_errorMessage = "缺少等号";
        return false;
    }

    // 提取名称
    QString name = line.left(equalsPos).trimmed();
    if (name.isEmpty()) {
        m_errorMessage = "正则表达式名称不能为空";
        return false;
    }

    // 提取模式
    QString pattern = line.mid(equalsPos + 1).trimmed();
    if (pattern.isEmpty()) {
        m_errorMessage = "正则表达式模式不能为空";
        return false;
    }

    // 验证名称
    int code = 0;
    bool isMultiWord = false;
    if (!validateName(name, code, isMultiWord)) {
        return false;
    }

    // 处理转义字符
    QString processedPattern = processEscapeCharacters(pattern);

    // 检查正则表达式语法
    if (!checkRegexSyntax(processedPattern)) {
        return false;
    }

    // 转换为标准正则表达式格式
    QString standardPattern = convertToStandardRegex(processedPattern);

    // 设置结果
    item.name = name;
    item.pattern = standardPattern;
    item.code = code;
    item.isMultiWord = isMultiWord;

    return true;
}

bool RegexProcessor::validateName(const QString &name, int &code, bool &isMultiWord)
{
    // 检查名称是否以下划线开头
    if (!name.startsWith('_')) {
        m_errorMessage = "正则表达式名称必须以下划线开头";
        return false;
    }

    // 检查名称格式：_xxx123 或 _xxx123S
    QRegularExpression nameRegex("^_([a-zA-Z_][a-zA-Z0-9_]*)(\\d+)(S?)$");
    QRegularExpressionMatch match = nameRegex.match(name);
    if (!match.hasMatch()) {
        m_errorMessage = "正则表达式名称格式错误，应为 _名称编码 或 _名称编码S";
        return false;
    }

    // 提取编码
    QString codeStr = match.captured(2);
    code = codeStr.toInt();
    if (code <= 0) {
        m_errorMessage = "单词编码必须为正整数";
        return false;
    }

    // 检查是否为多单词
    isMultiWord = match.captured(3) == "S";

    return true;
}

QString RegexProcessor::processEscapeCharacters(const QString &pattern)
{
    QString result = pattern;
    
    // Process escape characters
    // Escape special characters: +, *, ?, |, (, ), [, ], {, }, ., ^, $
    QList<QChar> specialChars;
    specialChars << '+' << '*' << '?' << '|' << '(' << ')' << '[' << ']' << '{' << '}' << '.' << '^' << '$' << '\\';    
    for (int i = 0; i < result.length(); ++i) {
        QChar c = result.at(i);
        if (c == '\\' && i < result.length() - 1) {
            QChar nextC = result.at(i + 1);
            if (specialChars.contains(nextC)) {
                // Keep escape character
                i++; // Skip next character
            } else {
                // Remove unnecessary escape
                result.remove(i, 1);
                i--;
            }
        }
    }
    
    return result;
}

bool RegexProcessor::checkRegexSyntax(const QString &pattern)
{
    try {
        // 尝试创建正则表达式对象来检查语法
        QRegularExpression regex(pattern);
        if (!regex.isValid()) {
            m_errorMessage = QString("正则表达式语法错误: %1")
                             .arg(regex.errorString());
            return false;
        }
    } catch (...) {
        m_errorMessage = "正则表达式语法错误";
        return false;
    }
    
    return true;
}

QString RegexProcessor::convertToStandardRegex(const QString &pattern)
{
    QString result = pattern;
    
    // 转换自定义语法为标准正则表达式语法
    
    // 处理字符集：[a-z] 已经是标准格式
    
    // 处理连接操作：在需要的地方添加隐式连接
    // 例如：ab -> a(?>b)
    QString temp = "";
    for (int i = 0; i < result.length(); ++i) {
        temp += result.at(i);
        if (i < result.length() - 1) {
            QChar current = result.at(i);
            QChar next = result.at(i + 1);
            
            // 需要添加连接的情况：
            // 1. 当前字符是普通字符、字符集结束或闭包操作，下一个字符是普通字符、字符集开始或左括号
            bool needConcat = false;
            
            // 当前字符是普通字符或字符集结束
            bool currentIsEnd = (current != '(' && current != '|' && current != '\\');
            if (current == ']' || current == '*' || current == '+' || current == '?') {
                currentIsEnd = true;
            }
            
            // 下一个字符是普通字符或字符集开始
            bool nextIsStart = (next != ')' && next != '|' && next != '*' && next != '+' && next != '?');
            if (next == '[' || next == '(') {
                nextIsStart = true;
            }
            
            if (currentIsEnd && nextIsStart) {
                temp += "(?>";
                int j = i + 1;
                // 找到需要连接的部分的结束位置
                int parenCount = 0;
                int bracketCount = 0;
                while (j < result.length()) {
                    QChar c = result.at(j);
                    if (c == '(') parenCount++;
                    else if (c == ')') {
                        if (parenCount == 0) break;
                        parenCount--;
                    } else if (c == '[') bracketCount++;
                    else if (c == ']') bracketCount--;
                    else if ((c == '|' || c == '*' || c == '+' || c == '?') && parenCount == 0 && bracketCount == 0) {
                        break;
                    }
                    j++;
                }
                temp += result.mid(i + 1, j - i - 1);
                temp += ")";
                i = j - 1;
            }
        }
    }
    result = temp;
    
    return result;
}