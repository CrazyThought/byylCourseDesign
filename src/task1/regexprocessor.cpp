#include "task1/regexprocessor.h"
#include "task1/regexengine.h"
#include <QStringList>
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

    QStringList lines = text.split("\n");
    int actualLineNumber = 0;
    
    // 第一次遍历：收集所有引用定义
    for (int i = 0; i < lines.size(); ++i) {
        actualLineNumber++;
        QString line = lines[i].trimmed();
        // 移除可能存在的回车符
        line = line.replace("\r", "");
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
        // 移除可能存在的回车符
        line = line.replace("\r", "");
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

        // 只有以下划线开头的名称才是正则表达式项
        if (name.startsWith('_')) {
            RegexItem item;
            if (!parseLine(line, item)) {
                m_errorMessage = QString("第 %1 行解析错误: %2")
                                 .arg(actualLineNumber).arg(m_errorMessage);
                return false;
            }
            m_regexItems.append(item);
        }
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

    // 提取多单词列表（如果是多单词）
    QStringList wordList;
    if (isMultiWord) {
        wordList = extractMultiWords(standardPattern);
    }

    // 设置结果
    item.name = name;
    item.pattern = standardPattern;
    item.code = code;
    item.isMultiWord = isMultiWord;
    item.wordList = wordList;

    return true;
}

bool RegexProcessor::validateName(const QString &name, int &code, bool &isMultiWord)
{
    // 只有以下划线开头的正则表达式才需要验证命名规则
    if (name.startsWith('_')) {
        // 检查名称格式：_xxx123 或 _xxx123S
        
        // 检查长度，至少需要：_ + 1个字母 + 1个数字 + 0或1个S
        if (name.length() < 3) {
            m_errorMessage = "正则表达式名称格式错误，应为 _名称编码 或 _名称编码S";
            return false;
        }
        
        int pos = 1; // 跳过开头的下划线
        
        // 提取名称部分（字母和下划线）
        while (pos < name.length()) {
            QChar c = name.at(pos);
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
                pos++;
            } else {
                break;
            }
        }
        
        // 确保名称部分至少有一个字符
        if (pos == 1) {
            m_errorMessage = "正则表达式名称格式错误，应为 _名称编码 或 _名称编码S";
            return false;
        }
        
        // 提取编码部分（数字）
        int codeStartPos = pos;
        while (pos < name.length()) {
            QChar c = name.at(pos);
            if (c >= '0' && c <= '9') {
                pos++;
            } else {
                break;
            }
        }
        
        // 确保编码部分至少有一个数字
        if (pos == codeStartPos) {
            m_errorMessage = "正则表达式名称格式错误，应为 _名称编码 或 _名称编码S";
            return false;
        }
        
        // 提取编码
        QString codeStr = name.mid(codeStartPos, pos - codeStartPos);
        code = codeStr.toInt();
        if (code < 0) {
            m_errorMessage = "单词编码必须为正整数";
            return false;
        }
        
        // 检查是否为多单词（可选的S结尾）
        isMultiWord = false;
        if (pos < name.length()) {
            if (name.at(pos) == 'S' && pos == name.length() - 1) {
                isMultiWord = true;
            } else {
                m_errorMessage = "正则表达式名称格式错误，应为 _名称编码 或 _名称编码S";
                return false;
            }
        }
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
        // 使用自定义RegexEngine检查正则表达式语法
        RegexEngine engine;
        if (!engine.compile(pattern)) {
            m_errorMessage = QString("正则表达式语法错误: %1")
                             .arg(engine.getErrorMessage());
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
        
        // 使用直接字符串替换，确保所有引用都能被正确替换
        result.replace(name, refPattern, Qt::CaseSensitive);
    }
    
    return result;
}

QStringList RegexProcessor::extractMultiWords(const QString &pattern)
{
    QStringList result;
    
    // 处理字符集情况，如 [+\-*/]
    if (pattern.startsWith('[') && pattern.endsWith(']')) {
        QString charset = pattern.mid(1, pattern.length() - 2);
        for (int i = 0; i < charset.length(); ++i) {
            QChar c = charset.at(i);
            // 跳过转义字符和范围标记
            if (c == '\\' || c == '-') {
                continue;
            }
            result.append(QString(c));
        }
    }
    // 处理选择表达式，如 +|\-|*|/
    else if (pattern.contains('|')) {
        QStringList options = pattern.split('|');
        for (const QString &option : options) {
            result.append(option.trimmed());
        }
    }
    // 处理其他情况
    else {
        // 简单处理：如果不是字符集或选择表达式，可能是单个单词
        result.append(pattern);
    }
    
    return result;
}
