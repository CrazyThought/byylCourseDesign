#include <QCoreApplication>
#include <QStringList>
#include <QDebug>

// 模拟修复后的括号计数逻辑
QStringList splitChoiceExpression(const QString &pattern) {
    QStringList topLevelParts;
    int bracketCount = 0;
    int lastSplitIndex = 0;
    
    for (int i = 0; i < pattern.length(); ++i) {
        QChar c = pattern.at(i);
        
        // 修复：添加转义字符检查
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
            QString part = pattern.mid(lastSplitIndex, i - lastSplitIndex);
            topLevelParts.append(part);
            lastSplitIndex = i + 1;
        }
    }
    
    // 添加最后一个部分
    if (lastSplitIndex < pattern.length()) {
        QString part = pattern.mid(lastSplitIndex);
        topLevelParts.append(part);
    }
    
    return topLevelParts;
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    
    qDebug() << "Qt测试：修复后的正则表达式处理逻辑";
    
    // 测试用例：模拟_special203S的正则表达式
    QString testPattern = "\\+|-|\\*|/|\\^|%|.|<|<=|>|>=|==|!=|=|;|,|\\+\\+|--|\\(|\\)";
    
    qDebug() << "测试模式:" << testPattern;
    
    QStringList result = splitChoiceExpression(testPattern);
    
    qDebug() << "\n分割结果 (" << result.size() << "个部分):";
    for (int i = 0; i < result.size(); ++i) {
        qDebug() << i + 1 << ": '" << result[i] << "'";
    }
    
    // 检查括号是否存在
    bool hasOpenBracket = result.contains("\\(");
    bool hasCloseBracket = result.contains("\\)");
    
    qDebug() << "\n检查结果：";
    qDebug() << "包含 \\(:" << (hasOpenBracket ? "是" : "否");
    qDebug() << "包含 \\):" << (hasCloseBracket ? "是" : "否");
    
    if (hasOpenBracket && hasCloseBracket) {
        qDebug() << "\n✅ 测试通过！括号字符能被正确处理。";
    } else {
        qDebug() << "\n❌ 测试失败！括号字符丢失。";
    }
    
    // 测试完整流程
    qDebug() << "\n=== 完整处理流程测试 ===";
    
    // 模拟完整处理：processEscapeCharacters → 分割 → tokenmap生成
    QString originalPattern = "\\+|-|\\*|/|\\^|%|.|<|<=|>|>=|==|!=|=|;|,|\\+\\+|--|\\(|\\)|\\{|\\}|\\[|\\]|_";
    
    qDebug() << "原始模式:" << originalPattern;
    
    // 1. 分割
    QStringList words = splitChoiceExpression(originalPattern);
    
    // 2. 处理转义字符（模拟tokenmap生成中的处理）
    qDebug() << "\n生成tokenmap条目:";
    int code = 203; // 模拟_special203S的起始编码
    bool foundBrackets = false;
    
    for (const QString &word : words) {
        // 处理转义字符，只保留转义后的字符
        QString processedWord;
        for (int i = 0; i < word.length(); i++) {
            QChar c = word.at(i);
            if (c == '\\' && i + 1 < word.length()) {
                processedWord += word.at(++i);
            } else {
                processedWord += c;
            }
        }
        
        // 生成tokenmap条目
        qDebug() << code << "=" << processedWord;
        
        // 检查括号
        if (processedWord == "(" || processedWord == ")") {
            foundBrackets = true;
        }
        
        code++;
    }
    
    if (foundBrackets) {
        qDebug() << "\n✅ 最终结果：tokenmap包含括号字符！";
    } else {
        qDebug() << "\n❌ 最终结果：tokenmap中括号字符丢失！";
    }
    
    return a.exec();
}