#include <QCoreApplication>
#include <QList>
#include <QDebug>
#include <QString>
#include "task1/lexergenerator.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    // 创建测试用的正则表达式项
    QList<RegexItem> regexItems;
    
    // 添加Tiny示例中的正则表达式项
    
    // _identifier100
    RegexItem identifierItem;
    identifierItem.name = "_identifier";
    identifierItem.code = 100;
    identifierItem.isMultiWord = false;
    regexItems.append(identifierItem);
    
    // _number101
    RegexItem numberItem;
    numberItem.name = "_number";
    numberItem.code = 101;
    numberItem.isMultiWord = false;
    regexItems.append(numberItem);
    
    // _comment102
    RegexItem commentItem;
    commentItem.name = "_comment";
    commentItem.code = 102;
    commentItem.isMultiWord = false;
    regexItems.append(commentItem);
    
    // _special103S
    RegexItem specialItem;
    specialItem.name = "_special";
    specialItem.code = 103;
    specialItem.isMultiWord = true;
    specialItem.wordList << "=" << ":=" << "+" << "-" << "*" << "/";
    regexItems.append(specialItem);
    
    // _Keyword200S
    RegexItem keywordItem;
    keywordItem.name = "_Keyword";
    keywordItem.code = 200;
    keywordItem.isMultiWord = true;
    keywordItem.wordList << "if" << "then" << "else" << "end" << "read" << "write";
    regexItems.append(keywordItem);
    
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
        int endPos = stateCode.indexOf("};" , startPos);
        if (startPos != -1 && endPos != -1) {
            QString acceptTokensArray = stateCode.mid(startPos, endPos - startPos + 2);
            qDebug() << "接受状态数组：";
            qDebug().noquote() << acceptTokensArray;
        }
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
