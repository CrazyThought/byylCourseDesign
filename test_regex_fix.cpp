#include <iostream>
#include <string>
#include <vector>

// 模拟修复后的括号计数逻辑
std::vector<std::string> splitChoiceExpression(const std::string &pattern) {
    std::vector<std::string> topLevelParts;
    int bracketCount = 0;
    int lastSplitIndex = 0;
    
    for (size_t i = 0; i < pattern.length(); ++i) {
        char c = pattern[i];
        
        // 修复：添加转义字符检查
        if (c == '\\' && i + 1 < pattern.length()) {
            i++; // 跳过转义后的字符
            continue;
        }
        
        if (c == '(') {
            bracketCount++;
            std::cout << "遇到 '('，bracketCount = " << bracketCount << std::endl;
        } else if (c == ')') {
            bracketCount--;
            std::cout << "遇到 ')'，bracketCount = " << bracketCount << std::endl;
        } else if (c == '|' && bracketCount == 0) {
            // 顶级|，分割
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
    std::cout << "测试修复后的正则表达式处理逻辑\n";
    std::cout << "==================================\n";
    
    // 测试用例：模拟_special203S的正则表达式
    std::string testPattern = "\\+|-|\\*|/|\\^|%|.|<|<=|>|>=|==|!=|=|;|,|\\+\\+|--|\\(|\\)";
    
    std::cout << "测试模式: " << testPattern << std::endl;
    std::cout << "\n分割过程：\n";
    
    std::vector<std::string> result = splitChoiceExpression(testPattern);
    
    std::cout << "\n分割结果 (" << result.size() << " 个部分):" << std::endl;
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << i + 1 << ": '" << result[i] << "'" << std::endl;
    }
    
    // 检查括号是否存在
    bool hasOpenBracket = false;
    bool hasCloseBracket = false;
    for (const auto &part : result) {
        if (part == "\\(") {
            hasOpenBracket = true;
        }
        if (part == "\\)") {
            hasCloseBracket = true;
        }
    }
    
    std::cout << "\n检查结果：\n";
    std::cout << "包含 \\(: " << (hasOpenBracket ? "是" : "否") << std::endl;
    std::cout << "包含 \\): " << (hasCloseBracket ? "是" : "否") << std::endl;
    
    if (hasOpenBracket && hasCloseBracket) {
        std::cout << "\n✅ 测试通过！括号字符能被正确处理。" << std::endl;
    } else {
        std::cout << "\n❌ 测试失败！括号字符丢失。" << std::endl;
    }
    
    return 0;
}