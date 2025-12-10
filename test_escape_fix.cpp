#include <iostream>
#include <string>
#include <vector>

// 模拟修复后的转义字符处理逻辑
std::string processEscapeCharacters(const std::string &input) {
    std::string result;
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == '\\' && i + 1 < input.length()) {
            // 处理转义字符，只保留转义后的字符
            result += input[++i];
        } else {
            result += c;
        }
    }
    return result;
}

int main() {
    std::vector<std::string> testCases = {
        "\\(",  // 应该变成 "("
        "\\)",  // 应该变成 ")"
        "\\+",  // 应该变成 "+"
        "\\*",  // 应该变成 "*"
        "test\\(case\\)",  // 应该变成 "test(case)"
        "normal text"  // 应该保持不变
    };
    
    std::cout << "测试转义字符处理逻辑:\n";
    for (const auto &testCase : testCases) {
        std::string result = processEscapeCharacters(testCase);
        std::cout << "输入: \"" << testCase << "\"\n";
        std::cout << "输出: \"" << result << "\"\n";
        std::cout << "---------------------\n";
    }
    
    return 0;
}