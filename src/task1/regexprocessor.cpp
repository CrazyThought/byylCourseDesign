/*
 * @file regexprocessor.cpp
 * @id regexprocessor-cpp
 * @brief 实现正则表达式处理功能，包括正则表达式解析、验证和转换
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task1/regexprocessor.h"
#include "task1/regexengine.h"
#include <QStringList>
#include <QtGlobal> // 为了Qt::SkipEmptyParts

/**
 * @brief 构造函数
 * 
 * 初始化正则表达式处理器
 */
RegexProcessor::RegexProcessor()
{
}

/**
 * @brief 析构函数
 * 
 * 清理正则表达式处理器资源
 */
RegexProcessor::~RegexProcessor()
{
}

/**
 * @brief 解析正则表达式文本
 * 
 * 从文本中解析正则表达式项和引用定义
 * 
 * @param text 包含正则表达式定义的文本
 * @return bool 解析成功返回true，失败返回false
 */
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

/**
 * @brief 获取解析得到的正则表达式项列表
 * 
 * @return QList<RegexItem> 正则表达式项列表
 */
QList<RegexItem> RegexProcessor::getRegexItems() const
{
    return m_regexItems;
}

/**
 * @brief 获取错误信息
 * 
 * @return QString 错误信息
 */
QString RegexProcessor::getErrorMessage() const
{
    return m_errorMessage;
}

/**
 * @brief 解析单行正则表达式定义
 * 
 * @param line 包含正则表达式定义的单行文本
 * @param item 输出参数，存储解析结果
 * @return bool 解析成功返回true，失败返回false
 */
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
        // 对于多单词情况，统一使用 generateWordsFromGroupedRegex 函数处理
        // 该函数能处理各种复杂情况，包括分组表达式和选择表达式
        wordList = generateWordsFromGroupedRegex(standardPattern);
        
        // 去重，确保每个单词只出现一次
        wordList.removeDuplicates();
        
        // 排序，便于后续处理
        wordList.sort();
    }

    // 设置结果
    item.name = name;
    item.pattern = standardPattern;
    item.code = code;
    item.isMultiWord = isMultiWord;
    item.wordList = wordList;

    return true;
}

/**
 * @brief 验证正则表达式名称
 * 
 * 验证正则表达式名称的格式，提取编码和是否为多单词
 * 
 * @param name 正则表达式名称
 * @param code 输出参数，存储提取的编码
 * @param isMultiWord 输出参数，指示是否为多单词
 * @return bool 验证成功返回true，失败返回false
 */
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

/**
 * @brief 处理转义字符
 * 
 * 处理正则表达式中的转义字符
 * 
 * @param pattern 包含转义字符的正则表达式模式
 * @return QString 处理后的正则表达式模式
 */
QString RegexProcessor::processEscapeCharacters(const QString &pattern)
{
    QString result = pattern;
    
    // 处理转义字符
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

/**
 * @brief 检查正则表达式语法
 * 
 * 使用自定义RegexEngine检查正则表达式语法
 * 
 * @param pattern 要检查的正则表达式模式
 * @return bool 语法正确返回true，错误返回false
 */
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

/**
 * @brief 转换为标准正则表达式格式
 * 
 * 将包含引用的正则表达式转换为标准格式
 * 
 * @param pattern 包含引用的正则表达式
 * @return QString 转换后的标准正则表达式
 */
QString RegexProcessor::convertToStandardRegex(const QString &pattern)
{
    QString result = pattern;
    
    // 替换引用，如 digit 替换为 [0-9]，letter 替换为 [A-Za-z]
    // 遍历所有引用
    for (auto it = m_regexReferences.constBegin(); it != m_regexReferences.constEnd(); ++it) {
        const QString &name = it.key();
        const QString &refPattern = it.value();
        
        // 改进：使用更安全的方式替换引用，避免误替换
        // 只替换完整的引用名称，不替换表达式中的部分字符串
        // 引用名称通常是由字母、数字和下划线组成的标识符
        
        // 不使用正则表达式，改用简单的字符串替换
        // 遍历字符串，查找完整的引用名称并替换
        int pos = 0;
        while (pos != -1 && pos < result.length()) {
            pos = result.indexOf(name, pos);
            if (pos != -1) {
                // 检查是否为完整的引用名称
                bool isCompleteWord = true;
                
                // 检查前面的字符是否为字母、数字或下划线
                if (pos > 0) {
                    QChar prevChar = result.at(pos - 1);
                    if (prevChar.isLetterOrNumber() || prevChar == '_') {
                        isCompleteWord = false;
                    }
                }
                
                // 检查后面的字符是否为字母、数字或下划线
                if (isCompleteWord && (pos + name.length()) < result.length()) {
                    QChar nextChar = result.at(pos + name.length());
                    if (nextChar.isLetterOrNumber() || nextChar == '_') {
                        isCompleteWord = false;
                    }
                }
                
                // 如果是完整的引用名称，则替换
                if (isCompleteWord) {
                    result.replace(pos, name.length(), refPattern);
                    pos += refPattern.length();
                } else {
                    // 否则继续查找下一个
                    pos += name.length();
                }
            }
        }
    }
    
    return result;
}

/**
 * @brief 提取多单词
 * 
 * 从正则表达式模式中提取所有可能的多单词
 * 
 * @param pattern 正则表达式模式
 * @return QStringList 提取的多单词列表
 */
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
            
            // 检查是否为转义字符
            if (c == '\\' && i + 1 < pattern.length()) {
                i++; // 跳过转义后的字符
                continue;
            }
            
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

/**
 * @brief 生成所有可能的大小写组合
 * 
 * 注意：此函数只处理简单字符串，不处理包含 (、) 或 | 的复杂表达式
 * 
 * @param pattern 要生成大小写组合的字符串
 * @return QStringList 所有可能的大小写组合
 */
QStringList RegexProcessor::generateAllCaseCombinations(const QString &pattern)
{
    QStringList result;
    
    // 处理基本情况：空字符串
    if (pattern.isEmpty()) {
        result.append("");
        return result;
    }
    
    // 检查是否包含复杂表达式符号
    if (pattern.contains('(') || pattern.contains(')') || pattern.contains('|')) {
        // 包含复杂表达式符号，直接返回原字符串
        result.append(pattern);
        return result;
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
    
    // 去重，确保每个组合只出现一次
    result.removeDuplicates();
    
    return result;
}

/**
 * @brief 从分组的正则表达式中生成所有可能的单词变体
 * 
 * @param pattern 分组的正则表达式
 * @param depth 递归深度，防止无限递归
 * @return QStringList 所有可能的单词变体
 */
QStringList RegexProcessor::generateWordsFromGroupedRegex(const QString &pattern, int depth)
{
    QStringList result;
    
    // 添加递归深度限制，防止无限递归
    if (depth > 100) {
        return result;
    }
    
    // 基本情况：空字符串
    if (pattern.isEmpty()) {
        result.append("");
        return result;
    }
    
    // 检查是否为选择表达式
    // 只在顶级括号外的|处分割
    int bracketCount = 0;
    bool isChoiceExpr = false;
    for (int i = 0; i < pattern.length(); ++i) {
        QChar c = pattern.at(i);
        
        // 检查是否为转义字符
        if (c == '\\' && i + 1 < pattern.length()) {
            i++; // 跳过转义后的字符
            continue;
        }
        
        if (c == '(') {
            bracketCount++;
        } else if (c == ')') {
            bracketCount--;
        } else if (c == '|' && bracketCount == 0) {
            isChoiceExpr = true;
            break;
        }
    }
    
    // 如果是选择表达式，分割成多个选择项，为每个选择项生成单词列表，然后合并
    if (isChoiceExpr) {
        QList<QString> choices;
        int lastChoiceStart = 0;
        bracketCount = 0;
        
        for (int i = 0; i < pattern.length(); ++i) {
            QChar c = pattern.at(i);
            
            // 检查是否为转义字符
            if (c == '\\' && i + 1 < pattern.length()) {
                i++; // 跳过转义后的字符
                continue;
            }
            
            if (c == '(') {
                bracketCount++;
            } else if (c == ')') {
                bracketCount--;
            } else if (c == '|' && bracketCount == 0) {
                choices.append(pattern.mid(lastChoiceStart, i - lastChoiceStart).trimmed());
                lastChoiceStart = i + 1;
            }
        }
        
        // 添加最后一个选择项
        choices.append(pattern.mid(lastChoiceStart).trimmed());
        
        // 移除空选择项
        choices.removeAll("");
        
        // 为每个选择项生成单词列表，并合并
        for (const QString &choice : choices) {
            QStringList choiceWords = generateWordsFromGroupedRegex(choice, depth + 1);
            result.append(choiceWords);
        }
        
        // 去重并排序
        result.removeDuplicates();
        result.sort();
        
        return result;
    }
    
    // 检查是否为简单字符串
    if (!pattern.contains('(') && !pattern.contains(')')) {
        // 简单字符串，直接生成大小写组合
        return generateAllCaseCombinations(pattern);
    }
    
    // 否则，是分组表达式，需要生成笛卡尔积
    // 1. 首先解析所有顶级分组和非分组部分
    // 示例：(i|I)(f|F) -> ["(i|I)", "(f|F)"]
    QList<QString> topLevelParts;
    int lastGroupStart = 0;
    bracketCount = 0;
    
    for (int i = 0; i < pattern.length(); ++i) {
        QChar c = pattern.at(i);
        
        // 检查是否为转义字符
        if (c == '\\' && i + 1 < pattern.length()) {
            i++; // 跳过转义后的字符
            continue;
        }
        
        if (c == '(') {
            bracketCount++;
            if (bracketCount == 1 && i > lastGroupStart) {
                // 分组前有非分组部分
                topLevelParts.append(pattern.mid(lastGroupStart, i - lastGroupStart));
                lastGroupStart = i;
            }
        } else if (c == ')') {
            bracketCount--;
            if (bracketCount == 0) {
                // 分组结束
                topLevelParts.append(pattern.mid(lastGroupStart, i - lastGroupStart + 1));
                lastGroupStart = i + 1;
            }
        }
    }
    
    // 添加最后一个部分
    if (lastGroupStart < pattern.length()) {
        topLevelParts.append(pattern.mid(lastGroupStart));
    }
    
    // 移除空字符串
    topLevelParts.removeAll("");
    
    // 2. 处理每个顶级部分，生成选项列表
    QList<QStringList> allOptions;
    
    for (const QString &part : topLevelParts) {
        if (part.startsWith('(') && part.endsWith(')')) {
            // 处理分组：(i|I) -> ["i", "I"]
            QString inner = part.mid(1, part.length() - 2);
            
            // 递归处理分组内容
            QStringList groupOptions = generateWordsFromGroupedRegex(inner, depth + 1);
            allOptions.append(groupOptions);
        } else {
            // 处理普通字符串：生成大小写组合
            QStringList simpleOptions = generateAllCaseCombinations(part);
            allOptions.append(simpleOptions);
        }
    }
    
    // 3. 生成所有可能的组合（笛卡尔积）
    if (allOptions.isEmpty()) {
        return result;
    }
    
    // 初始化结果列表
    result = allOptions.first();
    
    // 依次与其他部分的选项进行笛卡尔积
    for (int i = 1; i < allOptions.size(); ++i) {
        QStringList newResult;
        const QStringList &currentOptions = allOptions[i];
        
        for (const QString &existing : result) {
            for (const QString &option : currentOptions) {
                newResult.append(existing + option);
            }
        }
        
        result = newResult;
    }
    
    // 4. 去重并限制变体数量
    result.removeDuplicates();
    if (result.size() > 1024) {
        result = result.mid(0, 1024);
    }
    
    return result;
}
