#include <iostream>
#include "include/regexprocessor.h"
#include "include/regexengine.h"

using namespace std;

// 测试解析正则表达式文本
void testParse() {
    cout << "=== 测试解析正则表达式文本 ===" << endl;
    
    RegexProcessor processor;
    QString testText = "// 测试注释行\n" 
                      "digit=[0-9]\n" 
                      "letter=[a-zA-Z]\n" 
                      "_num100=\d+\n" 
                      "_id101=[a-zA-Z_]\w*\n" 
                      "_special200S=\+|\-|\*|/|=\n";
    
    bool success = processor.parse(testText);
    if (success) {
        cout << "✓ 解析成功" << endl;
        QList<RegexItem> items = processor.getRegexItems();
        cout << "解析到 " << items.size() << " 个正则表达式项" << endl;
        
        for (const RegexItem &item : items) {
            cout << "- 名称: " << item.name.toStdString() << ", 模式: " << item.pattern.toStdString() 
                 << ", 编码: " << item.code << ", 多单词: " << (item.isMultiWord ? "是" : "否") << endl;
            if (item.isMultiWord) {
                cout << "  单词列表: ";
                for (const QString &word : item.wordList) {
                    cout << word.toStdString() << " ";
                }
                cout << endl;
            }
        }
    } else {
        cout << "✗ 解析失败: " << processor.getErrorMessage().toStdString() << endl;
    }
    cout << endl;
}

// 测试验证名称格式
void testValidateName() {
    cout << "=== 测试验证名称格式 ===" << endl;
    
    RegexProcessor processor;
    QStringList testNames = {
        "_num100",
        "_id101S",
        "_invalidName",  // 缺少编码
        "_name123Sx",    // 无效结尾
        "name100",       // 缺少下划线开头
        "_123num",       // 数字在名称前
        "_name-123"      // 包含无效字符
    };
    
    for (const QString &name : testNames) {
        int code;
        bool isMultiWord;
        bool success = processor.validateName(name, code, isMultiWord);
        cout << "名称: " << name.toStdString() << ": ";
        if (success) {
            cout << "✓ 有效, 编码: " << code << ", 多单词: " << (isMultiWord ? "是" : "否") << endl;
        } else {
            cout << "✗ 无效: " << processor.getErrorMessage().toStdString() << endl;
        }
    }
    cout << endl;
}

// 测试检查正则表达式语法
void testCheckRegexSyntax() {
    cout << "=== 测试检查正则表达式语法 ===" << endl;
    
    RegexProcessor processor;
    QStringList testPatterns = {
        "\d+",
        "[a-zA-Z_]\w*",
        "\+|\-|\*|/",
        "[0-9a-fA-F]+\b",
        "(a|b)*c",
        "[a-z-",          // 缺少右括号
        "a**b",           // 无效的量词
        "a{3,2}b"         // 最小值大于最大值
    };
    
    for (const QString &pattern : testPatterns) {
        bool success = processor.checkRegexSyntax(pattern);
        cout << "模式: " << pattern.toStdString() << ": ";
        if (success) {
            cout << "✓ 语法正确" << endl;
        } else {
            cout << "✗ 语法错误: " << processor.getErrorMessage().toStdString() << endl;
        }
    }
    cout << endl;
}

// 测试自定义RegexEngine
void testRegexEngine() {
    cout << "=== 测试自定义RegexEngine ===" << endl;
    
    RegexEngine engine;
    
    // 测试基本匹配
    QString pattern = "\d+";
    if (engine.compile(pattern)) {
        cout << "✓ 编译成功: " << pattern.toStdString() << endl;
        
        QStringList testStrings = {"123", "abc", "12abc", "12345"};
        for (const QString &str : testStrings) {
            int matchLength = 0;
            bool success = engine.match(str, matchLength);
            cout << "  匹配 \"" << str.toStdString() << "\": ";
            if (success) {
                cout << "✓ 成功, 匹配长度: " << matchLength << endl;
            } else {
                cout << "✗ 失败" << endl;
            }
        }
    } else {
        cout << "✗ 编译失败: " << engine.getErrorMessage().toStdString() << endl;
    }
    
    // 测试字符集
    pattern = "[a-zA-Z_]\w*";
    if (engine.compile(pattern)) {
        cout << "✓ 编译成功: " << pattern.toStdString() << endl;
        
        QStringList testStrings = {"abc", "_abc123", "123abc", "abc_def"};
        for (const QString &str : testStrings) {
            int matchLength = 0;
            bool success = engine.match(str, matchLength);
            cout << "  匹配 \"" << str.toStdString() << "\": ";
            if (success) {
                cout << "✓ 成功, 匹配长度: " << matchLength << endl;
            } else {
                cout << "✗ 失败" << endl;
            }
        }
    } else {
        cout << "✗ 编译失败: " << engine.getErrorMessage().toStdString() << endl;
    }
    cout << endl;
}

int main() {
    cout << "正则表达式处理器测试" << endl;
    cout << "===================" << endl;
    
    testParse();
    testValidateName();
    testCheckRegexSyntax();
    testRegexEngine();
    
    cout << "所有测试完成!" << endl;
    return 0;
}
