#include <QTest>
#include <QList>
#include <QFile>
#include <QTextStream>
#include "task1/lexergenerator.h"
#include "task1/dfabuilder.h"
#include "task1/regexprocessor.h"
#include "task1/lexertester.h"

class TestLexerFull : public QObject
{
    Q_OBJECT

private slots:
    void testExample1();
    void testExample2();
    void testMultiWordEncoding();
    void testIdentifierMatching();
    void testNumberMatching();
    void testCommentMatching();
    void testSpecialCharacterMatching();
    void testKeywordMatching();
};

// 辅助函数：从文件中读取内容
QString readFileContent(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    
    QTextStream in(&file);
    return in.readAll();
}

// 辅助函数：解析example.txt中的测试样例
bool parseExampleTest(const QString &exampleContent, 
                     QString &regexText, 
                     QString &sourceCode, 
                     QString &expectedOutput)
{
    // 简单解析，根据行内容分割
    QStringList lines = exampleContent.split("\n");
    
    int state = 0; // 0: 寻找正则表达式开始, 1: 读取正则表达式, 2: 寻找待分析源程序开始, 3: 读取源程序, 4: 寻找预期输出开始, 5: 读取预期输出
    
    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        
        switch (state) {
        case 0:
            if (trimmedLine == "正则表达式：") {
                state = 1;
            }
            break;
        case 1:
            if (trimmedLine == "待分析源程序：") {
                state = 2;
            } else {
                regexText += line + "\n";
            }
            break;
        case 2:
            if (!trimmedLine.isEmpty()) {
                sourceCode += line + "\n";
                state = 3;
            }
            break;
        case 3:
            if (trimmedLine == "预期输出：") {
                state = 4;
            } else {
                sourceCode += line + "\n";
            }
            break;
        case 4:
            if (!trimmedLine.isEmpty()) {
                expectedOutput = trimmedLine;
                state = 5;
            }
            break;
        case 5:
            // 预期输出已经读取完成
            break;
        }
    }
    
    return !regexText.isEmpty() && !sourceCode.isEmpty() && !expectedOutput.isEmpty();
}

void TestLexerFull::testExample1()
{
    // 读取example.txt中的第一个测试样例
    QString exampleContent = readFileContent("d:/Programmes/Qt/byylCourseDesign/byylCD/test/example.txt");
    QVERIFY(!exampleContent.isEmpty());
    
    // 解析测试样例
    QString regexText, sourceCode, expectedOutput;
    bool parsed = parseExampleTest(exampleContent, regexText, sourceCode, expectedOutput);
    QVERIFY(parsed);
    
    // TODO: 这里需要实现完整的测试逻辑，包括：
    // 1. 使用RegexProcessor处理正则表达式
    // 2. 使用DFABuilder构建DFA
    // 3. 使用LexerGenerator生成词法分析器
    // 4. 使用LexerTester测试生成的词法分析器
    // 5. 验证结果是否与预期输出匹配
    
    QSKIP("测试框架需要完整实现");
}

void TestLexerFull::testExample2()
{
    // 读取example.txt中的第二个测试样例
    // 注意：这个函数需要正确解析example.txt中的第二个测试样例
    
    QSKIP("测试框架需要完整实现");
}

void TestLexerFull::testMultiWordEncoding()
{
    // 测试多单词编码处理
    QList<RegexItem> regexItems;
    
    // 创建一个多单词项
    RegexItem specialItem;
    specialItem.name = "_special";
    specialItem.code = 103;
    specialItem.isMultiWord = true;
    specialItem.wordList << "+" << "-" << "*" << "/" << "<" << "<=" << ">" << ">=" << "=" << ":=" << ";";
    regexItems.append(specialItem);
    
    // 创建简单的DFA
    DFA dfa;
    dfa.states << 0 << 1 << 2 << 3;
    dfa.startState = 0;
    dfa.acceptStates << 1 << 2 << 3;
    
    // 添加状态转移
    // 对于简单的+和-符号
    DFATransition t1, t2, t3, t4;
    t1.fromState = 0; t1.input = "+"; t1.toState = 1;
    t2.fromState = 0; t2.input = "-"; t2.toState = 1;
    t3.fromState = 0; t3.input = "<"; t3.toState = 2;
    t4.fromState = 2; t4.input = "="; t4.toState = 3;
    
    dfa.transitions << t1 << t2 << t3 << t4;
    
    // 生成词法分析器
    LexerGenerator generator;
    QString code = generator.generateStateTransitionLexer(regexItems, dfa);
    
    // 验证生成的代码结构
    QVERIFY(!code.isEmpty());
    QVERIFY(code.contains("_special"));
    QVERIFY(code.contains("103"));
}

void TestLexerFull::testIdentifierMatching()
{
    // 测试标识符匹配
    QList<RegexItem> regexItems;
    
    RegexItem identifierItem;
    identifierItem.name = "_identifier";
    identifierItem.code = 100;
    identifierItem.isMultiWord = false;
    regexItems.append(identifierItem);
    
    // 创建DFA用于标识符匹配
    DFA dfa;
    dfa.states << 0 << 1 << 2;
    dfa.startState = 0;
    dfa.acceptStates << 1 << 2;
    
    // 字母转移到状态1（接受态）
    for (char c = 'a'; c <= 'z'; c++) {
        DFATransition t;
        t.fromState = 0; t.input = QString(c); t.toState = 1;
        dfa.transitions << t;
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        DFATransition t;
        t.fromState = 0; t.input = QString(c); t.toState = 1;
        dfa.transitions << t;
    }
    
    // 字母或数字从状态1转移到状态2（接受态）
    for (char c = 'a'; c <= 'z'; c++) {
        DFATransition t;
        t.fromState = 1; t.input = QString(c); t.toState = 2;
        dfa.transitions << t;
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        DFATransition t;
        t.fromState = 1; t.input = QString(c); t.toState = 2;
        dfa.transitions << t;
    }
    for (char c = '0'; c <= '9'; c++) {
        DFATransition t;
        t.fromState = 1; t.input = QString(c); t.toState = 2;
        dfa.transitions << t;
    }
    
    // 字母或数字从状态2转移到状态2（接受态）
    for (char c = 'a'; c <= 'z'; c++) {
        DFATransition t;
        t.fromState = 2; t.input = QString(c); t.toState = 2;
        dfa.transitions << t;
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        DFATransition t;
        t.fromState = 2; t.input = QString(c); t.toState = 2;
        dfa.transitions << t;
    }
    for (char c = '0'; c <= '9'; c++) {
        DFATransition t;
        t.fromState = 2; t.input = QString(c); t.toState = 2;
        dfa.transitions << t;
    }
    
    // 生成词法分析器
    LexerGenerator generator;
    QString code = generator.generateStateTransitionLexer(regexItems, dfa);
    
    // 验证生成的代码
    QVERIFY(!code.isEmpty());
    QVERIFY(code.contains("_identifier"));
    QVERIFY(code.contains("100"));
    QVERIFY(code.contains("IsAlpha"));
    QVERIFY(code.contains("IsDigit"));
}

void TestLexerFull::testNumberMatching()
{
    // 测试数字匹配
    QList<RegexItem> regexItems;
    
    RegexItem numberItem;
    numberItem.name = "_number";
    numberItem.code = 101;
    numberItem.isMultiWord = false;
    regexItems.append(numberItem);
    
    // 创建DFA用于数字匹配
    DFA dfa;
    dfa.states << 0 << 1 << 2;
    dfa.startState = 0;
    dfa.acceptStates << 1 << 2;
    
    // 数字从状态0转移到状态1（接受态）
    for (char c = '0'; c <= '9'; c++) {
        DFATransition t;
        t.fromState = 0; t.input = QString(c); t.toState = 1;
        dfa.transitions << t;
    }
    
    // 数字从状态1转移到状态2（接受态）
    for (char c = '0'; c <= '9'; c++) {
        DFATransition t;
        t.fromState = 1; t.input = QString(c); t.toState = 2;
        dfa.transitions << t;
    }
    
    // 数字从状态2转移到状态2（接受态）
    for (char c = '0'; c <= '9'; c++) {
        DFATransition t;
        t.fromState = 2; t.input = QString(c); t.toState = 2;
        dfa.transitions << t;
    }
    
    // 生成词法分析器
    LexerGenerator generator;
    QString code = generator.generateStateTransitionLexer(regexItems, dfa);
    
    // 验证生成的代码
    QVERIFY(!code.isEmpty());
    QVERIFY(code.contains("_number"));
    QVERIFY(code.contains("101"));
    QVERIFY(code.contains("IsDigit"));
}

void TestLexerFull::testCommentMatching()
{
    // 测试注释匹配
    QList<RegexItem> regexItems;
    
    RegexItem commentItem;
    commentItem.name = "_comment";
    commentItem.code = 102;
    commentItem.isMultiWord = false;
    regexItems.append(commentItem);
    
    // 创建简单的DFA用于注释匹配 {comment}
    DFA dfa;
    dfa.states << 0 << 1 << 2;
    dfa.startState = 0;
    dfa.acceptStates << 2;
    
    // 添加状态转移
    DFATransition t1, t2, t3;
    t1.fromState = 0; t1.input = "{"; t1.toState = 1;
    t2.fromState = 1; t2.input = "a"; t2.toState = 1; // 简化处理，实际应该处理所有字符
    t3.fromState = 1; t3.input = "}"; t3.toState = 2;
    
    dfa.transitions << t1 << t2 << t3;
    
    // 生成词法分析器
    LexerGenerator generator;
    QString code = generator.generateStateTransitionLexer(regexItems, dfa);
    
    // 验证生成的代码
    QVERIFY(!code.isEmpty());
    QVERIFY(code.contains("_comment"));
    QVERIFY(code.contains("102"));
}

void TestLexerFull::testSpecialCharacterMatching()
{
    // 测试特殊字符匹配
    QList<RegexItem> regexItems;
    
    // 创建特殊字符项
    RegexItem specialItem;
    specialItem.name = "_special";
    specialItem.code = 103;
    specialItem.isMultiWord = true;
    specialItem.wordList << "=" << ":=" << ";";
    regexItems.append(specialItem);
    
    // 创建简单的DFA
    DFA dfa;
    dfa.states << 0 << 1 << 2;
    dfa.startState = 0;
    dfa.acceptStates << 1 << 2;
    
    // 添加状态转移
    DFATransition t1, t2, t3;
    t1.fromState = 0; t1.input = "="; t1.toState = 1;
    t2.fromState = 0; t2.input = ":"; t2.toState = 2;
    t3.fromState = 2; t3.input = "="; t3.toState = 1;
    
    dfa.transitions << t1 << t2 << t3;
    
    // 生成词法分析器
    LexerGenerator generator;
    QString code = generator.generateStateTransitionLexer(regexItems, dfa);
    
    // 验证生成的代码
    QVERIFY(!code.isEmpty());
    QVERIFY(code.contains(":="));
    QVERIFY(code.contains("103"));
}

void TestLexerFull::testKeywordMatching()
{
    // 测试关键字匹配
    QList<RegexItem> regexItems;
    
    // 创建关键字项
    RegexItem keywordItem;
    keywordItem.name = "_Keyword";
    keywordItem.code = 200;
    keywordItem.isMultiWord = true;
    keywordItem.wordList << "if" << "then" << "else" << "end" << "read" << "write";
    regexItems.append(keywordItem);
    
    // 创建简单的DFA
    DFA dfa;
    dfa.states << 0 << 1 << 2 << 3;
    dfa.startState = 0;
    dfa.acceptStates << 1 << 2 << 3;
    
    // 添加状态转移
    // 简化测试，只处理if和end
    DFATransition t1, t2, t3, t4;
    t1.fromState = 0; t1.input = "i"; t1.toState = 1;
    t2.fromState = 1; t2.input = "f"; t2.toState = 2;
    t3.fromState = 0; t3.input = "e"; t3.toState = 3;
    t4.fromState = 3; t4.input = "n"; t4.toState = 2;
    
    dfa.transitions << t1 << t2 << t3 << t4;
    
    // 生成词法分析器
    LexerGenerator generator;
    QString code = generator.generateStateTransitionLexer(regexItems, dfa);
    
    // 验证生成的代码
    QVERIFY(!code.isEmpty());
    QVERIFY(code.contains("_Keyword"));
    QVERIFY(code.contains("200"));
}

QTEST_APPLESS_MAIN(TestLexerFull)
#include "test_lexer_full.moc"
