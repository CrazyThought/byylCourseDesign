#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "include/task1/lexergenerator.h"
#include "include/task1/nfabuilder.h"
#include "include/task1/dfabuilder.h"

using namespace std;

int main() {
    // 创建词法分析器生成器
    LexerGenerator generator;
    
    // 定义正则表达式项
    QList<RegexItem> regexItems;
    
    // 添加标识符
    regexItems << RegexItem("identifier", "[a-zA-Z][a-zA-Z0-9]*", 100, false);
    
    // 添加数字
    regexItems << RegexItem("number", "[0-9]+(\.[0-9]+)?", 101, false);
    
    // 添加注释
    regexItems << RegexItem("comment", "\\{[^\\}]*\\}", 102, false);
    
    // 添加关键字（多单词）
    QStringList keywords;
    keywords << "if" << "then" << "else" << "end" << "read" << "write" << "repeat" << "until";
    regexItems << RegexItem("keyword", "", 200, true, keywords);
    
    // 添加操作符（多单词）
    QStringList operators;
    operators << "+" << "-" << "*" << "/" << "=" << "<" << "<=" << ">" << ">=" << "!=" << ":=";
    regexItems << RegexItem("operator", "", 103, true, operators);
    
    // 创建NFA和DFA构建器
    NFABuilder nfaBuilder;
    DFABuilder dfaBuilder;
    
    // 构建NFA
    nfaBuilder.buildFromRegexItems(regexItems);
    
    // 转换为DFA
    dfaBuilder = nfaBuilder.toDFA();
    
    // 最小化DFA
    dfaBuilder.minimize();
    
    // 生成词法分析器
    QString lexerCode = generator.generateLexer(regexItems, dfaBuilder, LexerGenerator::GenerationMethod::STATE_TRANSITION);
    
    // 保存生成的词法分析器
    ofstream outFile("test/lexer.cpp");
    if (outFile.is_open()) {
        outFile << lexerCode.toStdString();
        outFile.close();
        cout << "Lexer generated successfully: test/lexer.cpp" << endl;
    } else {
        cout << "Error: Could not open output file" << endl;
        return 1;
    }
    
    // 编译生成的词法分析器
    system("g++ -o test/lexer.exe test/lexer.cpp");
    
    // 运行词法分析器测试
    system("test/lexer.exe test/test_multi_word.txt");
    
    return 0;
}
