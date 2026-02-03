#include <QCoreApplication>
#include <QList>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QTextStream>
#include "task1/lexergenerator.h"

// 从文件读取正则表达式定义
QList<RegexItem> readRegexItemsFromFile(const QString &filePath)
{
    QList<RegexItem> regexItems;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件：" << filePath;
        return regexItems;
    }
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("//")) {
            continue;
        }
        
        // 解析正则表达式定义
        // 格式：name=pattern 或 nameCode=pattern 或 nameCodeS=pattern
        int equalPos = line.indexOf('=');
        if (equalPos == -1) {
            continue;
        }
        
        QString namePart = line.left(equalPos);
        QString pattern = line.mid(equalPos + 1);
        
        // 解析名称和代码
        QString name;
        int code = 0;
        bool isMultiWord = false;
        
        // 检查是否包含代码
        if (namePart.contains(QRegExp("\\d+"))) {
            // 提取代码
            QRegExp codeRegex("\\d+");
            if (codeRegex.indexIn(namePart) != -1) {
                code = codeRegex.cap(0).toInt();
                name = namePart.left(codeRegex.indexIn(namePart));
            }
        } else {
            name = namePart;
        }
        
        // 检查是否为多单词项（以S结尾）
        if (namePart.endsWith('S')) {
            isMultiWord = true;
        }
        
        // 创建RegexItem
        RegexItem item;
        item.name = name;
        item.code = code;
        item.isMultiWord = isMultiWord;
        
        // 对于多单词项，提取单词列表
        if (isMultiWord) {
            // 这里简化处理，实际应该解析正则表达式来提取单词
            // 对于测试，我们手动添加一些常见的单词
            item.wordList << "if" << "then" << "else" << "end" << "read" << "write";
            item.wordList << "=" << ":=" << "+" << "-" << "*" << "/";
        }
        
        regexItems.append(item);
    }
    
    file.close();
    return regexItems;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    // 读取Tiny示例中的正则表达式定义
    QString regexFile = "test/Tiny/regix_sample.txt";
    QList<RegexItem> regexItems = readRegexItemsFromFile(regexFile);
    
    if (regexItems.isEmpty()) {
        qDebug() << "无法读取正则表达式定义";
        return 1;
    }
    
    qDebug() << "=== 读取的正则表达式项 ===";
    for (const RegexItem &item : regexItems) {
        qDebug() << "名称：" << item.name << "，代码：" << item.code << "，多单词：" << item.isMultiWord;
    }
    
    // 创建测试用的DFA
    DFA dfa;
    
    // 添加状态
    dfa.states << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8;
    dfa.startState = 0;
    dfa.acceptStates << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8;
    
    // 添加一些基本的状态转移
    DFATransition t1, t2, t3, t4, t5;
    t1.fromState = 0; t1.input = "a"; t1.toState = 1;
    t2.fromState = 0; t2.input = "0"; t2.toState = 2;
    t3.fromState = 0; t3.input = "{"; t3.toState = 3;
    t4.fromState = 0; t4.input = "+"; t4.toState = 4;
    t5.fromState = 0; t5.input = "i"; t5.toState = 5;
    
    dfa.transitions << t1 << t2 << t3 << t4 << t5;
    
    // 添加接受状态到正则表达式索引的映射
    dfa.acceptStateToRegexIndex[1] = 0; // 标识符
    dfa.acceptStateToRegexIndex[2] = 1; // 数字
    dfa.acceptStateToRegexIndex[3] = 2; // 注释
    dfa.acceptStateToRegexIndex[4] = 3; // 特殊字符
    dfa.acceptStateToRegexIndex[5] = 4; // 关键字
    
    // 创建词法分析器生成器
    LexerGenerator generator;
    
    // 生成状态转移法代码
    qDebug() << "=== 生成状态转移法代码 ===";
    QString stateCode = generator.generateLexer(regexItems, dfa, GenerationMethod::STATE_TRANSITION);
    
    // 检查生成的代码是否包含所有必要的接受状态代码
    qDebug() << "=== 检查生成的接受状态映射 ===";
    
    bool has100 = stateCode.contains("100");
    bool has101 = stateCode.contains("101");
    bool has102 = stateCode.contains("102");
    bool has103 = stateCode.contains("103");
    bool has200 = stateCode.contains("200");
    
    qDebug() << "包含 100 (identifier)：" << has100;
    qDebug() << "包含 101 (number)：" << has101;
    qDebug() << "包含 102 (comment)：" << has102;
    qDebug() << "包含 103 (special)：" << has103;
    qDebug() << "包含 200 (keyword)：" << has200;
    
    // 检查接受状态数组
    qDebug() << "=== 检查接受状态数组 ===";
    if (stateCode.contains("acceptTokens")) {
        int startPos = stateCode.indexOf("acceptTokens");
        int endPos = stateCode.indexOf("};", startPos);
        if (startPos != -1 && endPos != -1) {
            QString acceptTokensArray = stateCode.mid(startPos, endPos - startPos + 2);
            qDebug() << "接受状态数组：";
            qDebug().noquote() << acceptTokensArray;
        }
    }
    
    // 保存生成的代码到文件以便检查
    QFile outputFile("test_tiny_output.cpp");
    if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&outputFile);
        out << stateCode;
        outputFile.close();
        qDebug() << "生成的代码已保存到 test_tiny_output.cpp";
    }
    
    // 验证所有必要的代码都已包含
    bool allCodesPresent = has100 && has101 && has102 && has103 && has200;
    qDebug() << "=== 验证结果 ===";
    qDebug() << "所有必要的代码都已包含：" << allCodesPresent;
    
    if (allCodesPresent) {
        qDebug() << "修复成功！生成的接受状态映射包含所有必要的代码。";
    } else {
        qDebug() << "修复失败！生成的接受状态映射缺少某些代码。";
    }
    
    return 0;
}
