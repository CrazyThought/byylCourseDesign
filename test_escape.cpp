#include <iostream>
#include <string>

// 修复后的转义字符处理逻辑
std::string processEscapeCharacters(const std::string &input) {
    std::string processedValue;
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == '\\' && i + 1 < input.length()) {
            // 处理转义字符，只保留转义后的字符
            processedValue += input[++i];
        } else {
            processedValue += c;
        }
    }
    return processedValue;
}

int main() {
    std::cout << "测试转义字符处理逻辑\n";
    std::cout << "=====================\n";
    
    // 测试用例
    std::string testCases[] = {
        "\\(",
        "\\)",
        "\\+",
        "\\*",
        "\\?",
        "a\\+b",
        "\\(abc\\)"
    };
    
    for (const auto &testCase : testCases) {
        std::string result = processEscapeCharacters(testCase);
        std::cout << testCase << " -> " << result << "\n";
    }
    
    // 重点测试括号字符
    std::cout << "\n重点测试括号：\n";
    std::string bracketTest = "\\(";
    std::string bracketResult = processEscapeCharacters(bracketTest);
    std::cout << "\\( 处理后: '" << bracketResult << "'，长度: " << bracketResult.length() << "\n";
    
    // 测试修复前后的差异
    std::cout << "\n修复前后对比：\n";
    std::cout << "旧逻辑(移除所有\\): " << "\\(" << " -> " << "" << " (空字符串)\n";
    std::cout << "新逻辑(正确处理): " << "\\(" << " -> " << bracketResult << "\n";
    
    return 0;
}