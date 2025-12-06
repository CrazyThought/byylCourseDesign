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
        // 检查是否为分组的正则表达式，如 (i|I)(f|F)
        if (standardPattern.contains('(') && standardPattern.contains(')')) {
            // 使用新函数从分组的正则表达式中生成所有可能的单词变体
            wordList = generateWordsFromGroupedRegex(standardPattern);
        } else {
            // 原有逻辑：处理简单的选择表达式，如 if|else|while
            QStringList rawWordList = extractMultiWords(standardPattern);
            // 生成所有大小写组合
            QStringList combinedWordList;
            for (const QString &rawWord : rawWordList) {
                QStringList combinations = generateAllCaseCombinations(rawWord);
                combinedWordList.append(combinations);
            }
            wordList = combinedWordList;
        }
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
    // 处理选择表达式，如 +|\-|*|/ 或 (i|I)(f|F)
    else if (pattern.contains('|')) {
        // 使用括号匹配算法，只在顶级|处分割
        QList<QString> topLevelParts;
        int bracketCount = 0;
        int lastSplitIndex = 0;
        
        for (int i = 0; i < pattern.length(); ++i) {
            QChar c = pattern.at(i);
            if (c == '(') {
                bracketCount++;
            } else if (c == ')') {
                bracketCount--;
            } else if (c == '|' && bracketCount == 0) {
                // 顶级|，分割
                topLevelParts.append(pattern.mid(lastSplitIndex, i - lastSplitIndex).trimmed());
                lastSplitIndex = i + 1;
            }
        }
        // 添加最后一个部分
        topLevelParts.append(pattern.mid(lastSplitIndex).trimmed());
        
        // 处理每个顶级部分
        for (const QString &part : topLevelParts) {
            // 移除首尾括号
            QString processedPart = part;
            if (processedPart.startsWith('(') && processedPart.endsWith(')')) {
                processedPart = processedPart.mid(1, processedPart.length() - 2);
            }
            
            // 递归处理嵌套的选择表达式
            QStringList subParts = extractMultiWords(processedPart);
            if (subParts.isEmpty()) {
                result.append(processedPart);
            } else {
                result.append(subParts);
            }
        }
    }
    // 处理其他情况
    else {
        // 简单处理：如果不是字符集或选择表达式，可能是单个单词
        result.append(pattern);
    }
    
    return result;
}

// 生成所有可能的大小写组合
QStringList RegexProcessor::generateAllCaseCombinations(const QString &pattern)
{
    QStringList result;
    
    // 处理基本情况：空字符串
    if (pattern.isEmpty()) {
        result.append("");
        return result;
    }
    
    // 检查是否为选择表达式
    if (pattern.contains('|')) {
        // 只处理顶级选择表达式
        int bracketCount = 0;
        int lastSplitIndex = 0;
        QList<QString> topLevelParts;
        
        for (int i = 0; i < pattern.length(); ++i) {
            QChar c = pattern.at(i);
            if (c == '(') {
                bracketCount++;
            } else if (c == ')') {
                bracketCount--;
            } else if (c == '|' && bracketCount == 0) {
                topLevelParts.append(pattern.mid(lastSplitIndex, i - lastSplitIndex).trimmed());
                lastSplitIndex = i + 1;
            }
        }
        topLevelParts.append(pattern.mid(lastSplitIndex).trimmed());
        
        // 对于每个选择项，生成所有组合
        for (const QString &part : topLevelParts) {
            QStringList subCombinations = generateAllCaseCombinations(part);
            result.append(subCombinations);
        }
        
        return result;
    }
    
    // 检查是否为括号表达式
    if (pattern.startsWith('(') && pattern.endsWith(')')) {
        return generateAllCaseCombinations(pattern.mid(1, pattern.length() - 2));
    }
    
    // 处理普通字符串
    if (pattern.length() == 1) {
        // 单个字符，返回原始字符和大写字符（如果是字母）
        result.append(pattern);
        if (pattern.at(0).isLetter()) {
            result.append(pattern.toUpper());
        }
        return result;
    }
    
    // 递归处理：将字符串拆分为第一个字符和剩余部分
    QString firstChar = pattern.left(1);
    QString rest = pattern.mid(1);
    
    QStringList firstCombinations = generateAllCaseCombinations(firstChar);
    QStringList restCombinations = generateAllCaseCombinations(rest);
    
    // 生成所有组合
    for (const QString &first : firstCombinations) {
        for (const QString &r : restCombinations) {
            result.append(first + r);
        }
    }
    
    return result;
}

// 从分组的正则表达式中生成所有可能的单词变体
QStringList RegexProcessor::generateWordsFromGroupedRegex(const QString &pattern, int depth)
{
    QStringList result;
    
    // 添加递归深度限制，防止无限递归
    if (depth > 100) {
        // 递归深度超过限制，返回空列表
        return result;
    }
    
    // 基本情况：空字符串
    if (pattern.isEmpty()) {
        result.append("");
        return result;
    }
    
    // 针对关键词的分组表达式，如 (i|I)(f|F)，生成所有可能的组合
    // 使用更精确的方法来处理多个连续分组
    
    // 检查是否为类似 (i|I)(f|F) 这样的分组表达式
    if (pattern.startsWith('(') && pattern.endsWith(')')) {
        // 移除外层括号
        QString innerPattern = pattern.mid(1, pattern.length() - 2);
        
        // 使用更精确的方法来识别和处理多个连续分组
        // 例如：(i|I)(f|F) 应该被识别为两个独立的分组
        
        // 分割连续的分组
        QList<QString> groups;
        int bracketCount = 0;
        int lastGroupStart = 0;
        
        for (int i = 0; i < innerPattern.length(); ++i) {
            QChar c = innerPattern.at(i);
            if (c == '(') {
                if (bracketCount == 0) {
                    // 新的分组开始
                    lastGroupStart = i;
                }
                bracketCount++;
            } else if (c == ')') {
                bracketCount--;
                if (bracketCount == 0) {
                    // 当前分组结束
                    QString group = innerPattern.mid(lastGroupStart, i - lastGroupStart + 1);
                    groups.append(group);
                }
            }
        }
        
        // 如果成功识别到多个分组
        if (groups.size() >= 1) {
            // 为每个分组生成可能的选项
            QList<QStringList> groupOptions;
            
            for (const QString &group : groups) {
                // 移除分组的外层括号
                if (group.startsWith('(') && group.endsWith(')')) {
                    QString innerGroup = group.mid(1, group.length() - 2);
                    
                    // 分割选择项
                    QStringList choices;
                    int innerBracketCount = 0;
                    int lastChoiceStart = 0;
                    
                    for (int i = 0; i < innerGroup.length(); ++i) {
                        QChar c = innerGroup.at(i);
                        if (c == '(') {
                            innerBracketCount++;
                        } else if (c == ')') {
                            innerBracketCount--;
                        } else if (c == '|' && innerBracketCount == 0) {
                            // 顶级选择符
                            QString choice = innerGroup.mid(lastChoiceStart, i - lastChoiceStart);
                            choices.append(choice);
                            lastChoiceStart = i + 1;
                        }
                    }
                    
                    // 添加最后一个选择项
                    QString lastChoice = innerGroup.mid(lastChoiceStart);
                    choices.append(lastChoice);
                    
                    groupOptions.append(choices);
                } else {
                    // 不是分组的情况，直接添加
                    groupOptions.append(QStringList() << group);
                }
            }
            
            // 生成所有可能的组合
            if (!groupOptions.isEmpty()) {
                // 初始化结果列表
                result = groupOptions.first();
                
                // 依次与其他分组的选项进行笛卡尔积
                for (int i = 1; i < groupOptions.size(); ++i) {
                    QStringList newResult;
                    const QStringList &currentOptions = groupOptions[i];
                    
                    for (const QString &existing : result) {
                        for (const QString &option : currentOptions) {
                            newResult.append(existing + option);
                        }
                    }
                    
                    result = newResult;
                }
                
                // 添加变体数量限制，防止内存耗尽
                if (result.size() > 1024) {
                    result = result.mid(0, 1024);
                }
                
                return result;
            }
        }
    }
    
    // 原有逻辑：处理简单的选择表达式，如 if|else|while
    QStringList rawWordList = extractMultiWords(pattern);
    
    // 生成所有大小写组合
    QStringList combinedWordList;
    for (const QString &rawWord : rawWordList) {
        QStringList combinations = generateAllCaseCombinations(rawWord);
        combinedWordList.append(combinations);
    }
    
    // 添加变体数量限制，防止内存耗尽
    if (combinedWordList.size() > 1024) {
        combinedWordList = combinedWordList.mid(0, 1024);
    }
    
    return combinedWordList;
}
