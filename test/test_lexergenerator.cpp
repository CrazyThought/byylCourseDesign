#include <QTest>
#include <QList>
#include "task1/lexergenerator.h"
#include "task1/dfabuilder.h"
#include "task1/regexprocessor.h"
#include <QFile>
#include <QTextStream>

class TestLexerGenerator : public QObject
{
    Q_OBJECT

private slots:
    void testGenerateLexer();
    void testGenerateDirectMatchLexer();
    void testGenerateStateTransitionLexer();
    void testGeneratedCodeCompiles();
};

void TestLexerGenerator::testGenerateLexer()
{
    // 创建测试用的正则表达式项
    QList<RegexItem> regexItems;
    RegexItem identifierItem;
    identifierItem.name = "_identifier";
    identifierItem.code = 1;
    identifierItem.isMultiWord = false;
    regexItems.append(identifierItem);

    RegexItem numberItem;
    numberItem.name = "_number";
    numberItem.code = 2;
    numberItem.isMultiWord = false;
    regexItems.append(numberItem);

    // 创建测试用的DFA
    DFA dfa;
    dfa.states << 0 << 1 << 2;
    dfa.startState = 0;
    dfa.acceptStates << 1 << 2;
    
    // 添加简单的状态转移
    DFATransition transition1;
    transition1.fromState = 0;
    transition1.input = "a";
    transition1.toState = 1;
    dfa.transitions.append(transition1);
    
    DFATransition transition2;
    transition2.fromState = 0;
    transition2.input = "0";
    transition2.toState = 2;
    dfa.transitions.append(transition2);
    
    // 创建lexer生成器
    LexerGenerator generator;
    
    // 测试直接匹配法
    QString directMatchCode = generator.generateLexer(regexItems, dfa, GenerationMethod::DIRECT_MATCH);
    QVERIFY(!directMatchCode.isEmpty());
    QVERIFY(directMatchCode.contains("lexicalAnalysis"));
    
    // 测试状态转移法
    QString stateTransitionCode = generator.generateLexer(regexItems, dfa, GenerationMethod::STATE_TRANSITION);
    QVERIFY(!stateTransitionCode.isEmpty());
    QVERIFY(stateTransitionCode.contains("getNextState"));
    QVERIFY(stateTransitionCode.contains("IsDigit"));
    QVERIFY(stateTransitionCode.contains("IsAlpha"));
}

void TestLexerGenerator::testGenerateDirectMatchLexer()
{
    // 创建测试用的正则表达式项
    QList<RegexItem> regexItems;
    RegexItem keywordItem;
    keywordItem.name = "keyword";
    keywordItem.code = 100;
    keywordItem.isMultiWord = true;
    keywordItem.wordList << "if" << "else" << "while";
    regexItems.append(keywordItem);
    
    RegexItem operatorItem;
    operatorItem.name = "operator";
    operatorItem.code = 200;
    operatorItem.isMultiWord = true;
    operatorItem.wordList << "+" << "-" << "*" << "/";
    regexItems.append(operatorItem);
    
    // 创建lexer生成器
    LexerGenerator generator;
    DFA dummyDfa;
    
    // 生成直接匹配法代码
    QString code = generator.generateDirectMatchLexer(regexItems, dummyDfa);
    
    // 验证代码结构
    QVERIFY(code.contains("struct Token"));
    QVERIFY(code.contains("vector<Token> lexicalAnalysis"));
    QVERIFY(code.contains("if (source.compare(pos, 2, \"if\"))"));
    QVERIFY(code.contains("if (source.compare(pos, 4, \"else\"))"));
    QVERIFY(code.contains("if (source.compare(pos, 5, \"while\"))"));
    
    // 验证没有映射表相关代码
    QVERIFY(!code.contains("tokenCodeMap"));
    QVERIFY(!code.contains("initializeTokenCodeMap"));
}

void TestLexerGenerator::testGenerateStateTransitionLexer()
{
    // 创建测试用的正则表达式项
    QList<RegexItem> regexItems;
    RegexItem identifierItem;
    identifierItem.name = "_identifier";
    identifierItem.code = 1;
    identifierItem.isMultiWord = false;
    regexItems.append(identifierItem);
    
    // 创建测试用的DFA
    DFA dfa;
    dfa.states << 0 << 1 << 2;
    dfa.startState = 0;
    dfa.acceptStates << 1 << 2;
    
    // 添加数字字符转移
    for (char c = '0'; c <= '9'; c++) {
        DFATransition transition;
        transition.fromState = 0;
        transition.input = QString(c);
        transition.toState = 1;
        dfa.transitions.append(transition);
    }
    
    // 添加字母字符转移
    for (char c = 'a'; c <= 'z'; c++) {
        DFATransition transition;
        transition.fromState = 0;
        transition.input = QString(c);
        transition.toState = 2;
        dfa.transitions.append(transition);
    }
    
    // 创建lexer生成器
    LexerGenerator generator;
    
    // 生成状态转移法代码
    QString code = generator.generateStateTransitionLexer(regexItems, dfa);
    
    // 验证代码结构
    QVERIFY(code.contains("IsDigit"));
    QVERIFY(code.contains("IsAlpha"));
    QVERIFY(code.contains("getNextState"));
    QVERIFY(code.contains("case '0': case '1': case '2': case '3': case '4'"));
    QVERIFY(code.contains("case 'a': case 'b': case 'c': case 'd': case 'e'"));
    
    // 验证没有映射表相关代码
    QVERIFY(!code.contains("tokenCodeMap"));
    QVERIFY(!code.contains("initializeTokenCodeMap"));
}

void TestLexerGenerator::testGeneratedCodeCompiles()
{
    // 创建简单的正则表达式项
    QList<RegexItem> regexItems;
    RegexItem simpleItem;
    simpleItem.name = "simple";
    simpleItem.code = 1;
    simpleItem.isMultiWord = false;
    regexItems.append(simpleItem);
    
    // 创建简单的DFA
    DFA dfa;
    dfa.states << 0 << 1;
    dfa.startState = 0;
    dfa.acceptStates << 1;
    
    DFATransition transition;
    transition.fromState = 0;
    transition.input = "a";
    transition.toState = 1;
    dfa.transitions.append(transition);
    
    // 生成状态转移法代码
    LexerGenerator generator;
    QString code = generator.generateStateTransitionLexer(regexItems, dfa);
    
    // 保存生成的代码到临时文件
    QFile tempFile("test_lexer.cpp");
    if (tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&tempFile);
        out << code;
        tempFile.close();
        
        // 尝试编译生成的代码
        int result = system("g++ -o test_lexer.exe test_lexer.cpp");
        QVERIFY(result == 0);
        
        // 清理临时文件
        QFile::remove("test_lexer.cpp");
        QFile::remove("test_lexer.exe");
    } else {
        QFAIL("Failed to create temporary file");
    }
}

QTEST_APPLESS_MAIN(TestLexerGenerator)
#include "test_lexergenerator.moc"
