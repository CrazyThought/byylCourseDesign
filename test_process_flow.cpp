#include <iostream>
#include <string>
#include <vector>

// 模拟 processEscapeCharacters 函数
std::string processEscapeCharacters(const std::string &pattern) {
    std::string result = pattern;
    std::vector<char> specialChars = {'+', '*', '?', '|', '(', ')', '[', ']', '{', '}', '.', '^', '$', '\\'};
    
    for (size_t i = 0; i < result.length(); ++i) {
        char c = result[i];
        if (c == '\\' && i + 1 < result.length()) {
            char nextC = result[i + 1];
            bool isSpecial = false;
            for (char sc : specialChars) {
                if (nextC == sc) {
                    isSpecial = true;
                    break;
                }
            }
            if (isSpecial) {
                i++; // 跳过下一个字符，保留转义字符
            } else {
                // 移除不必要的转义
                result.erase(i, 1);
                i--;
            }
        }
    }
    
    return result;
}

// 模拟 extractMultiWords 函数的分割逻辑
std::vector<std::string> extractWords(const std::string &pattern) {
    std::vector<std::string> topLevelParts;
    int bracketCount = 0;
    int lastSplitIndex = 0;
    
    for (size_t i = 0; i < pattern.length(); ++i) {
        char c = pattern[i];
        
        // 关键问题：这里没有考虑转义字符！
        if (c == '(') {
            bracketCount++;
            std::cout << "遇到 '('，bracketCount = " << bracketCount << std::endl;
        } else if (c == ')') {
            bracketCount--;
            std::cout << "遇到 ')'，bracketCount = " << bracketCount << std::endl;
        } else if (c == '|' && bracketCount == 0) {
            // 顶级 |，分割
            std::string part = pattern.substr(lastSplitIndex, i - lastSplitIndex);
            topLevelParts.push_back(part);
            std::cout << "分割出: '" << part << "'" << std::endl;
            lastSplitIndex = i + 1;
        }
    }
    
    // 添加最后一个部分
    if (lastSplitIndex < pattern.length()) {
        std::string part = pattern.substr(lastSplitIndex);
        topLevelParts.push_back(part);
        std::cout << "最后部分: '" << part << "'" << std::endl;
    }
    
    return topLevelParts;
}

int main() {
    std::cout << "分析正则表达式处理流程\n";
    std::cout << "=====================\n";
    
    // 输入的正则表达式行
    std::string inputLine = "\+|-|\*|/|\^|%|.|<|<=|>|>=|==|!=|=|;|,|\+\+|--|\(|\)|\{|\}|\[|\]|_";
    std::cout << "原始输入: " << inputLine << std::endl;
    
    // 1. processEscapeCharacters 处理
    std::string processed = processEscapeCharacters(inputLine);
    std::cout << "\n1. processEscapeCharacters 处理后: " << processed << std::endl;
    
    // 2. extractWords 分割（模拟问题）
    std::cout << "\n2. extractWords 分割过程: " << std::endl;
    std::vector<std::string> words = extractWords(processed);
    
    // 3. 显示结果
    std::cout << "\n3. 最终分割结果: " << std::endl;
    for (size_t i = 0; i < words.size(); ++i) {
        std::cout << i << ": '" << words[i] << "'" << std::endl;
    }
    
    // 4. 检查是否包含括号
    bool hasOpenBracket = false;
    bool hasCloseBracket = false;
    for (const auto &word : words) {
        if (word == "\(") {
            hasOpenBracket = true;
        }
        if (word == "\)") {
            hasCloseBracket = true;
        }
    }
    
    std::cout << "\n4. 结果检查: " << std::endl;
    std::cout << "包含 '\\(': " << (hasOpenBracket ? "是" : "否") << std::endl;
    std::cout << "包含 '\\)': " << (hasCloseBracket ? "是" : "否") << std::endl;
    
    return 0;
}