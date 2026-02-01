#include <iostream>
#include <string>
#include "task1/lexergenerator.h"
#include "task1/dfabuilder.h"
#include "task1/regexprocessor.h"
#include <fstream>

int main() {
    // 创建详细的测试用例
    QList<RegexItem> regexItems;
    
    // 关键字测试
    RegexItem keywordItem;
    keywordItem.name = "keyword";
    keywordItem.code = 100;
    keywordItem.isMultiWord = true;
    keywordItem.wordList << "if" << "else" << "while" << "for" << "return";
    regexItems.append(keywordItem);
    
    // 标识符测试
    RegexItem identifierItem;
    identifierItem.name = "_identifier";
    identifierItem.code = 1;
    identifierItem.isMultiWord = false;
    regexItems.append(identifierItem);
    
    // 数字测试
    RegexItem numberItem;
    numberItem.name = "_number";
    numberItem.code = 2;
    numberItem.isMultiWord = false;
    regexItems.append(numberItem);
    
    // 运算符测试
    RegexItem operatorItem;
    operatorItem.name = "operator";
    operatorItem.code = 300;
    operatorItem.isMultiWord = true;
    operatorItem.wordList << "+" << "-" << "*" << "/" << "=" << "==" << "!=" << "<" << ">" << "<=" << ">=";
    regexItems.append(operatorItem);
    
    // 分隔符测试
    RegexItem separatorItem;
    separatorItem.name = "separator";
    separatorItem.code = 400;
    separatorItem.isMultiWord = true;
    separatorItem.wordList << "(" << ")" << "{" << "}" << "[" << "]" << ";" << "," << ".";
    regexItems.append(separatorItem);
    
    // 创建DFA（简化版，实际应用中应该从正则表达式构建）
    DFA dfa;
    dfa.states << 0 << 1 << 2 << 3;
    dfa.startState = 0;
    dfa.acceptStates << 1 << 2 << 3;
    
    // 状态转移
    // 简单的状态转移，用于测试生成逻辑
    DFATransition t1;
    t1.fromState = 0;
    t1.input = "i";
    t1.toState = 1;
    dfa.transitions.append(t1);
    
    DFATransition t2;
    t2.fromState = 0;
    t2.input = "w";
    t2.toState = 1;
    dfa.transitions.append(t2);
    
    DFATransition t3;
    t3.fromState = 0;
    t3.input = "f";
    t3.toState = 1;
    dfa.transitions.append(t3);
    
    DFATransition t4;
    t4.fromState = 0;
    t4.input = "r";
    t4.toState = 1;
    dfa.transitions.append(t4);
    
    // 数字转移
    for (char c = '0'; c <= '9'; c++) {
        DFATransition t;
        t.fromState = 0;
        t.input = QString(c);
        t.toState = 2;
        dfa.transitions.append(t);
    }
    
    // 字母转移
    for (char c = 'a'; c <= 'z'; c++) {
        if (c != 'i' && c != 'w' && c != 'f' && c != 'r') {
            DFATransition t;
            t.fromState = 0;
            t.input = QString(c);
            t.toState = 3;
            dfa.transitions.append(t);
        }
    }
    
    // 运算符转移
    QStringList operators = {"+", "-", "*", "/", "=", "(", ")", "{", "}", "[", "]", ";", ",", "."};
    for (const QString& op : operators) {
        DFATransition t;
        t.fromState = 0;
        t.input = op;
        t.toState = 1;
        dfa.transitions.append(t);
    }
    
    // 创建lexer生成器
    LexerGenerator generator;
    
    // 生成直接匹配法代码
    QString directCode = generator.generateLexer(regexItems, dfa, GenerationMethod::DIRECT_MATCH);
    
    // 生成状态转移法代码
    QString stateCode = generator.generateLexer(regexItems, dfa, GenerationMethod::STATE_TRANSITION);
    
    // 验证生成的代码结构
    bool directValid = directCode.contains("struct Token") && 
                      directCode.contains("lexicalAnalysis") &&
                      !directCode.contains("tokenCodeMap") &&
                      !directCode.contains("initializeTokenCodeMap");
    
    bool stateValid = stateCode.contains("getNextState") &&
                     stateCode.contains("IsDigit") &&
                     stateCode.contains("IsAlpha") &&
                     !stateCode.contains("tokenCodeMap") &&
                     !stateCode.contains("initializeTokenCodeMap");
    
    std::cout << "=== 测试结果 ===" << std::endl;
    std::cout << "直接匹配法代码生成：" << (directValid ? "通过" : "失败") << std::endl;
    std::cout << "状态转移法代码生成：" << (stateValid ? "通过" : "失败") << std::endl;
    
    // 保存生成的代码用于人工检查
    std::ofstream directFile("direct_match_lexer.cpp");
    if (directFile.is_open()) {
        directFile << directCode.toStdString();
        directFile.close();
        std::cout << "直接匹配法代码已保存到：direct_match_lexer.cpp" << std::endl;
    }
    
    std::ofstream stateFile("state_transition_lexer.cpp");
    if (stateFile.is_open()) {
        stateFile << stateCode.toStdString();
        stateFile.close();
        std::cout << "状态转移法代码已保存到：state_transition_lexer.cpp" << std::endl;
    }
    
    // 测试生成的代码编译
    std::cout << std::endl << "=== 编译测试 ===" << std::endl;
    
    // 测试直接匹配法代码编译
    std::ofstream testFile("test_direct.cpp");
    if (testFile.is_open()) {
        testFile << directCode.toStdString();
        testFile.close();
        
        std::cout << "编译直接匹配法代码..." << std::endl;
        int directResult = system("g++ -o test_direct.exe test_direct.cpp");
        std::cout << "直接匹配法代码编译：" << (directResult == 0 ? "通过" : "失败") << std::endl;
        
        if (directResult == 0) {
            std::remove("test_direct.exe");
        }
        std::remove("test_direct.cpp");
    }
    
    // 测试状态转移法代码编译
    std::ofstream testStateFile("test_state.cpp");
    if (testStateFile.is_open()) {
        testStateFile << stateCode.toStdString();
        testStateFile.close();
        
        std::cout << "编译状态转移法代码..." << std::endl;
        int stateResult = system("g++ -o test_state.exe test_state.cpp");
        std::cout << "状态转移法代码编译：" << (stateResult == 0 ? "通过" : "失败") << std::endl;
        
        if (stateResult == 0) {
            std::remove("test_state.exe");
        }
        std::remove("test_state.cpp");
    }
    
    std::cout << std::endl << "=== 测试完成 ===" << std::endl;
    
    return 0;
}
