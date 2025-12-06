#include "include/regexengine.h"
#include <QDebug>
#include <QRegularExpression>

// 测试结果结构
typedef struct {
    QString pattern;
    QString input;
    bool expectedMatch;
    int expectedLength;
} TestCase;

// 测试函数
void runTest(const TestCase &testCase)
{
    // 使用自定义正则表达式引擎
    RegexEngine engine;
    bool customMatch = false;
    int customLength = 0;
    
    if (engine.compile(testCase.pattern)) {
        customMatch = engine.match(testCase.input, customLength);
    } else {
        qDebug() << "自定义引擎编译失败:" << engine.getErrorMessage();
        return;
    }
    
    // 使用QRegularExpression
    QRegularExpression qregex(testCase.pattern);
    QRegularExpressionMatch qmatch = qregex.match(testCase.input);
    bool qtMatch = qmatch.hasMatch();
    int qtLength = qmatch.hasMatch() ? qmatch.capturedLength() : 0;
    
    // 比较结果
    bool matchResult = (customMatch == qtMatch);
    bool lengthResult = (customLength == qtLength);
    
    qDebug() << "\n测试: 模式='" << testCase.pattern << "', 输入='" << testCase.input << "'";
    qDebug() << "预期匹配: " << (testCase.expectedMatch ? "是" : "否") << ", 预期长度: " << testCase.expectedLength;
    qDebug() << "自定义引擎: 匹配=" << (customMatch ? "是" : "否") << ", 长度=" << customLength;
    qDebug() << "QRegularExpression: 匹配=" << (qtMatch ? "是" : "否") << ", 长度=" << qtLength;
    qDebug() << "匹配结果: " << (matchResult ? "通过" : "失败") << ", 长度结果: " << (lengthResult ? "通过" : "失败");
    qDebug() << "总结果: " << ((matchResult && lengthResult) ? "通过" : "失败");
}

int main()
{
    qDebug() << "=== 正则表达式引擎测试 ===";
    
    // 测试用例列表
    QList<TestCase> testCases = {
        // 基本字符匹配
        {"a", "a", true, 1},
        {"a", "b", false, 0},
        {"ab", "ab", true, 2},
        {"ab", "ac", false, 0},
        
        // 特殊字符
        {".", "a", true, 1},
        {".", "\n", false, 0},
        {".", "", false, 0},
        {"a.b", "acb", true, 3},
        {"a.b", "ab", false, 0},
        
        // 量词
        {"a*", "", true, 0},
        {"a*", "a", true, 1},
        {"a*", "aa", true, 2},
        {"a+", "", false, 0},
        {"a+", "a", true, 1},
        {"a+", "aa", true, 2},
        {"a?", "", true, 0},
        {"a?", "a", true, 1},
        {"a?", "aa", true, 1},
        {"a{2}", "aa", true, 2},
        {"a{2}", "a", false, 0},
        {"a{2,}", "a", false, 0},
        {"a{2,}", "aa", true, 2},
        {"a{2,}", "aaa", true, 3},
        {"a{1,3}", "a", true, 1},
        {"a{1,3}", "aa", true, 2},
        {"a{1,3}", "aaa", true, 3},
        {"a{1,3}", "aaaa", true, 3},
        
        // 字符集
        {"[abc]", "a", true, 1},
        {"[abc]", "d", false, 0},
        {"[^abc]", "d", true, 1},
        {"[^abc]", "a", false, 0},
        {"[a-z]", "a", true, 1},
        {"[a-z]", "z", true, 1},
        {"[a-z]", "0", false, 0},
        {"[a-zA-Z0-9]", "a", true, 1},
        {"[a-zA-Z0-9]", "Z", true, 1},
        {"[a-zA-Z0-9]", "5", true, 1},
        {"[a-zA-Z0-9]", "@", false, 0},
        
        // 锚点
        {"^a", "a", true, 1},
        {"^a", "ba", false, 0},
        {"a$", "a", true, 1},
        {"a$", "ab", false, 0},
        {"^a$", "a", true, 1},
        {"^a$", "aa", false, 0},
        
        // 选择
        {"a|b", "a", true, 1},
        {"a|b", "b", true, 1},
        {"a|b", "c", false, 0},
        {"a|b|c", "b", true, 1},
        
        // 分组
        {"(ab)*", "", true, 0},
        {"(ab)*", "ab", true, 2},
        {"(ab)*", "abab", true, 4},
        {"(a|b)*", "abba", true, 4},
        
        // 转义字符
        {"\\.", ".", true, 1},
        {"\\*", "*", true, 1},
        {"\\+", "+", true, 1},
        {"\\?", "?", true, 1},
        {"\\(", "(", true, 1},
        {"\\)", ")", true, 1},
        {"\\[", "[", true, 1},
        {"\\]", "]", true, 1},
        {"\\{", "{", true, 1},
        {"\\}", "}", true, 1},
        {"\\|", "|", true, 1},
        {"\\n", "\n", true, 1},
        {"\\t", "\t", true, 1},
        {"\\r", "\r", true, 1},
        
        // 复杂模式
        {"^[a-zA-Z][a-zA-Z0-9_]*$", "valid_name", true, 10},
        {"^[a-zA-Z][a-zA-Z0-9_]*$", "1invalid", false, 0},
        {"^\\d{3}-\\d{3}-\\d{4}$", "123-456-7890", true, 12},
        {"^\\d{3}-\\d{3}-\\d{4}$", "1234567890", false, 0},
        {"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$", "test@example.com", true, 16},
        {"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$", "invalid-email", false, 0}
    };
    
    // 运行所有测试
    for (const TestCase &testCase : testCases) {
        runTest(testCase);
    }
    
    qDebug() << "\n=== 测试完成 ===";
    return 0;
}
