#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>

using namespace std;

// 模拟 RegexItem 结构
typedef struct {
    string name;
    string pattern;
    int code;
    bool isMultiWord;
    vector<string> wordList;
} RegexItem;

// 简单的大小写组合生成函数
vector<string> generateAllCaseCombinations(const string &pattern) {
    vector<string> result;
    
    // 处理基本情况：空字符串
    if (pattern.empty()) {
        result.push_back("");
        return result;
    }
    
    // 检查是否包含复杂表达式符号
    if (pattern.find('(') != string::npos || pattern.find(')') != string::npos || pattern.find('|') != string::npos) {
        // 包含复杂表达式符号，直接返回原字符串
        result.push_back(pattern);
        return result;
    }
    
    // 处理普通字符串
    if (pattern.length() == 1) {
        // 单个字符，返回原始字符和大写字符（如果是字母）
        result.push_back(pattern);
        if (isalpha(pattern[0])) {
            string upper = pattern;
            upper[0] = toupper(pattern[0]);
            result.push_back(upper);
        }
        return result;
    }
    
    // 递归处理：将字符串拆分为第一个字符和剩余部分
    string firstChar = pattern.substr(0, 1);
    string rest = pattern.substr(1);
    
    vector<string> firstCombinations = generateAllCaseCombinations(firstChar);
    vector<string> restCombinations = generateAllCaseCombinations(rest);
    
    // 生成所有组合
    for (const string &first : firstCombinations) {
        for (const string &r : restCombinations) {
            result.push_back(first + r);
        }
    }
    
    // 去重，确保每个组合只出现一次
    sort(result.begin(), result.end());
    result.erase(unique(result.begin(), result.end()), result.end());
    
    return result;
}

// 改进的分组正则表达式解析函数
vector<string> generateWordsFromGroupedRegex(const string &pattern) {
    vector<string> result;
    
    // 检查是否包含任何分组或选择符
    if (pattern.find('(') == string::npos && pattern.find(')') == string::npos && pattern.find('|') == string::npos) {
        // 简单字符串，直接生成大小写组合
        return generateAllCaseCombinations(pattern);
    }
    
    // 1. 首先解析所有顶级分组和非分组部分
    vector<string> topLevelParts;
    int bracketCount = 0;
    int lastPartStart = 0;
    
    for (int i = 0; i < pattern.length(); ++i) {
        char c = pattern[i];
        
        if (c == '(') {
            bracketCount++;
            if (bracketCount == 1 && i > lastPartStart) {
                // 分组前有非分组部分
                topLevelParts.push_back(pattern.substr(lastPartStart, i - lastPartStart));
                lastPartStart = i;
            }
        } else if (c == ')') {
            bracketCount--;
            if (bracketCount == 0) {
                // 分组结束
                topLevelParts.push_back(pattern.substr(lastPartStart, i - lastPartStart + 1));
                lastPartStart = i + 1;
            }
        } else if (c == '|' && bracketCount == 0) {
            // 顶级选择符，分割选择项
            topLevelParts.push_back(pattern.substr(lastPartStart, i - lastPartStart));
            lastPartStart = i + 1;
        }
    }
    
    // 添加最后一个部分
    if (lastPartStart < pattern.length()) {
        topLevelParts.push_back(pattern.substr(lastPartStart));
    }
    
    // 移除空字符串
    topLevelParts.erase(remove_if(topLevelParts.begin(), topLevelParts.end(), [](const string &s) { return s.empty(); }), topLevelParts.end());
    
    // 2. 处理每个顶级部分，生成选项列表
    vector<vector<string>> allOptions;
    
    for (const string &part : topLevelParts) {
        if (part.front() == '(' && part.back() == ')') {
            // 处理分组：(i|I) -> ["i", "I"]
            string inner = part.substr(1, part.length() - 2);
            
            // 递归处理分组内容
            vector<string> groupOptions = generateWordsFromGroupedRegex(inner);
            allOptions.push_back(groupOptions);
        } else if (part.find('|') != string::npos) {
            // 处理选择表达式：if|else -> ["if", "else"]
            vector<string> choices;
            string currentChoice;
            for (char c : part) {
                if (c == '|') {
                    if (!currentChoice.empty()) {
                        choices.push_back(currentChoice);
                        currentChoice.clear();
                    }
                } else {
                    currentChoice += c;
                }
            }
            if (!currentChoice.empty()) {
                choices.push_back(currentChoice);
            }
            
            vector<string> choiceOptions;
            for (const string &choice : choices) {
                vector<string> subOptions = generateWordsFromGroupedRegex(choice);
                choiceOptions.insert(choiceOptions.end(), subOptions.begin(), subOptions.end());
            }
            
            allOptions.push_back(choiceOptions);
        } else {
            // 处理普通字符串：生成大小写组合
            vector<string> simpleOptions = generateAllCaseCombinations(part);
            allOptions.push_back(simpleOptions);
        }
    }
    
    // 3. 生成所有可能的组合
    if (allOptions.empty()) {
        return result;
    }
    
    // 初始化结果列表
    result = allOptions[0];
    
    // 依次与其他部分的选项进行笛卡尔积
    for (size_t i = 1; i < allOptions.size(); ++i) {
        vector<string> newResult;
        const vector<string> &currentOptions = allOptions[i];
        
        for (const string &existing : result) {
            for (const string &option : currentOptions) {
                newResult.push_back(existing + option);
            }
        }
        
        result = newResult;
    }
    
    // 4. 去重并排序
    sort(result.begin(), result.end());
    result.erase(unique(result.begin(), result.end()), result.end());
    
    return result;
}

int main() {
    cout << "测试单词生成逻辑..." << endl;
    
    // 测试1：简单分组表达式 (i|I)(f|F)
    cout << endl << "测试1：(i|I)(f|F)" << endl;
    vector<string> ifWords = generateWordsFromGroupedRegex("(i|I)(f|F)");
    cout << "生成的单词列表：" << endl;
    for (const string &word : ifWords) {
        cout << "  - " << word << endl;
    }
    cout << "预期数量：4，实际数量：" << ifWords.size() << endl;
    
    // 测试2：复杂分组表达式 (t|T)(h|H)(e|E)(n|N)
    cout << endl << "测试2：(t|T)(h|H)(e|E)(n|N)" << endl;
    vector<string> thenWords = generateWordsFromGroupedRegex("(t|T)(h|H)(e|E)(n|N)");
    cout << "生成的单词列表（前10个）：" << endl;
    for (size_t i = 0; i < min(10, (int)thenWords.size()); ++i) {
        cout << "  - " << thenWords[i] << endl;
    }
    if (thenWords.size() > 10) {
        cout << "  ... 还有 " << thenWords.size() - 10 << " 个单词" << endl;
    }
    cout << "预期数量：16，实际数量：" << thenWords.size() << endl;
    
    // 测试3：选择表达式 if|else|while
    cout << endl << "测试3：if|else|while" << endl;
    vector<string> keywords = generateWordsFromGroupedRegex("if|else|while");
    cout << "生成的单词列表：" << endl;
    for (const string &word : keywords) {
        cout << "  - " << word << endl;
    }
    cout << "预期数量：3，实际数量：" << keywords.size() << endl;
    
    // 测试4：组合表达式 (i|I)(f|F)|(e|E)(l|L)(s|S)(e|E)
    cout << endl << "测试4：(i|I)(f|F)|(e|E)(l|L)(s|S)(e|E)" << endl;
    vector<string> combined = generateWordsFromGroupedRegex("(i|I)(f|F)|(e|E)(l|L)(s|S)(e|E)");
    cout << "生成的单词列表：" << endl;
    for (const string &word : combined) {
        cout << "  - " << word << endl;
    }
    cout << "预期数量：4 + 16 = 20，实际数量：" << combined.size() << endl;
    
    return 0;
}
