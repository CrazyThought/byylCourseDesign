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
    
    // 基本情况：空字符串
    if (pattern.empty()) {
        result.push_back("");
        return result;
    }
    
    // 检查是否为选择表达式
    // 只在顶级括号外的|处分割
    int bracketCount = 0;
    bool isChoiceExpr = false;
    for (char c : pattern) {
        if (c == '(') {
            bracketCount++;
        } else if (c == ')') {
            bracketCount--;
        } else if (c == '|' && bracketCount == 0) {
            isChoiceExpr = true;
            break;
        }
    }
    
    // 如果是选择表达式，分割成多个选择项，为每个选择项生成单词列表，然后合并
    if (isChoiceExpr) {
        vector<string> choices;
        int lastChoiceStart = 0;
        bracketCount = 0;
        
        for (size_t i = 0; i < pattern.length(); ++i) {
            char c = pattern[i];
            if (c == '(') {
                bracketCount++;
            } else if (c == ')') {
                bracketCount--;
            } else if (c == '|' && bracketCount == 0) {
                choices.push_back(pattern.substr(lastChoiceStart, i - lastChoiceStart));
                lastChoiceStart = i + 1;
            }
        }
        
        // 添加最后一个选择项
        choices.push_back(pattern.substr(lastChoiceStart));
        
        // 移除空选择项
        choices.erase(remove_if(choices.begin(), choices.end(), [](const string &s) { return s.empty(); }), choices.end());
        
        // 为每个选择项生成单词列表，并合并
        for (const string &choice : choices) {
            vector<string> choiceWords = generateWordsFromGroupedRegex(choice);
            result.insert(result.end(), choiceWords.begin(), choiceWords.end());
        }
        
        // 去重并排序
        sort(result.begin(), result.end());
        result.erase(unique(result.begin(), result.end()), result.end());
        
        return result;
    }
    
    // 检查是否为简单字符串
    if (pattern.find('(') == string::npos && pattern.find(')') == string::npos) {
        // 简单字符串，直接生成大小写组合
        return generateAllCaseCombinations(pattern);
    }
    
    // 否则，是分组表达式，需要生成笛卡尔积
    // 1. 首先解析所有顶级分组和非分组部分
    // 示例：(i|I)(f|F) -> ["(i|I)", "(f|F)"]
    vector<string> topLevelParts;
    int lastGroupStart = 0;
    bracketCount = 0;
    
    for (size_t i = 0; i < pattern.length(); ++i) {
        char c = pattern[i];
        
        if (c == '(') {
            bracketCount++;
            if (bracketCount == 1 && i > lastGroupStart) {
                // 分组前有非分组部分
                topLevelParts.push_back(pattern.substr(lastGroupStart, i - lastGroupStart));
                lastGroupStart = i;
            }
        } else if (c == ')') {
            bracketCount--;
            if (bracketCount == 0) {
                // 分组结束
                topLevelParts.push_back(pattern.substr(lastGroupStart, i - lastGroupStart + 1));
                lastGroupStart = i + 1;
            }
        }
    }
    
    // 添加最后一个部分
    if (lastGroupStart < pattern.length()) {
        topLevelParts.push_back(pattern.substr(lastGroupStart));
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
        } else {
            // 处理普通字符串：生成大小写组合
            vector<string> simpleOptions = generateAllCaseCombinations(part);
            allOptions.push_back(simpleOptions);
        }
    }
    
    // 3. 生成所有可能的组合（笛卡尔积）
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
