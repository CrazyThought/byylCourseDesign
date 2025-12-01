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
    m_regexReferences.clear();

    QStringList lines = text.split(QRegularExpression("[\\r\\n]"));
    int actualLineNumber = 0;
    
    // 第一次遍历：收集所有引用定义
    for (int i = 0; i < lines.size(); ++i) {
        actualLineNumber++;
        QString line = lines[i].trimmed();
        if (line.isEmpty() || line.startsWith("//")) {
            continue; // 跳过空行和注释行
        }

        // 寻找等号位置
        int equalsPos = line.indexOf('=');
        if (equalsPos == -1) {
            continue; // 跳过无效行
        }

        // 提取名称
        QString name = line.left(equalsPos).trimmed();
        if (name.isEmpty()) {
            continue; // 跳过无效行
        }

        // 提取模式
        QString pattern = line.mid(equalsPos + 1).trimmed();
        if (pattern.isEmpty()) {
            continue; // 跳过无效行
        }

        // 如果不是以下划线开头，说明是引用定义
        if (!name.startsWith('_')) {
            // 处理转义字符
            QString processedPattern = processEscapeCharacters(pattern);
            m_regexReferences[name] = processedPattern;
        }
    }
    
    // 第二次遍历：解析正则表达式项
    actualLineNumber = 0;
    for (int i = 0; i < lines.size(); ++i) {
        actualLineNumber++;
        QString line = lines[i].trimmed();
        if (line.isEmpty() || line.startsWith("//")) {
            continue; // 跳过空行和注释行
        }

        RegexItem item;
        if (!parseLine(line, item)) {
            m_errorMessage = QString("第 %1 行解析错误: %2")
                             .arg(actualLineNumber).arg(m_errorMessage);
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
    // 只有以下划线开头的正则表达式才需要验证命名规则
    if (name.startsWith('_')) {
        // 检查名称格式：_xxx123 或 _xxx123S
        // 简化正则表达式，确保能正确匹配_num100这样的名称
        QRegularExpression nameRegex("^_([a-zA-Z0-9_]+)(\\d+)(S?)$");
        QRegularExpressionMatch match = nameRegex.match(name);
        if (!match.hasMatch()) {
            m_errorMessage = "正则表达式名称格式错误，应为 _名称编码 或 _名称编码S";
            return false;
        }

        // 提取编码
        QString codeStr = match.captured(2);
        code = codeStr.toInt();
        if (code < 0) {
            m_errorMessage = "单词编码必须为正整数";
            return false;
        }

        // 检查是否为多单词
        isMultiWord = match.captured(3) == "S";
    } else {
        // 对于非下划线开头的正则表达式，设置默认值
        code = 0;
        isMultiWord = false;
    }

    return true;
}

QString RegexProcessor::processEscapeCharacters(const QString &pattern)
{
    QString result = pattern;
    
    // Process escape characters
    // Escape special characters: +, *, ?, |, (, ), [, ], {, }, ., ^, $
    QList<QChar> specialChars;
    specialChars << '+' << '*' << '?' << '|' << '(' << ')' << '[' << ']' << '{' << '}' << '.' << '^' << '$' << '\\';    
    
    // Iterate through the string and process escape characters
    for (int i = 0; i < result.length(); ++i) {
        QChar c = result.at(i);
        if (c == '\\' && i < result.length() - 1) {
            QChar nextC = result.at(i + 1);
            if (specialChars.contains(nextC)) {
                // Keep escape character for special characters
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
    
    // 替换引用，如 digit 替换为 [0-9]，letter 替换为 [A-Za-z]
    // 遍历所有引用
    for (auto it = m_regexReferences.constBegin(); it != m_regexReferences.constEnd(); ++it) {
        const QString &name = it.key();
        const QString &refPattern = it.value();
        
        // 构建正则表达式：匹配引用名称，确保不是其他单词的一部分
        // 使用 \b 作为单词边界
        QRegularExpression refRegex(QString("\\b%1\\b").arg(name));
        
        // 替换所有匹配的引用
        result.replace(refRegex, refPattern);
    }
    
    return result;
}
