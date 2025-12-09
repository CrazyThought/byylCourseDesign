#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
using namespace std;

// 正则表达式项结构体
struct RegexItem {
    string name;
    string pattern;
    int code;
    bool isMultiWord;
    vector<string> wordList;
};

// DFATransition结构体
typedef struct {
    int fromState;
    string input;
    int toState;
} DFATransition;

// DFA结构体
typedef struct {
    vector<int> states;
    set<string> alphabet;
    vector<DFATransition> transitions;
    int startState;
    set<int> acceptStates;
    map<vector<int>, int> stateMap;
    map<int, int> acceptStateToRegexIndex;
} DFA;

// 生成接受状态映射
string generateAcceptStatesMap(const vector<RegexItem> &regexItems, const DFA &minimizedDFA)
{
    string code;
    
    code += "// 接受状态映射\n";
    
    // 构建接受状态映射
    int numStates = minimizedDFA.states.size();
    vector<bool> isAccept(numStates, false);
    vector<int> tokens(numStates, -1);
    
    // 1. 遍历所有接受状态，将它们标记为接受状态
    for (int state : minimizedDFA.acceptStates) {
        if (state >= 0 && state < numStates) {
            isAccept[state] = true;
        }
    }
    
    // 2. 为每个接受状态分配正确的tokenCode
    // 收集所有正则表达式项，并按照code值排序
    vector<RegexItem> sortedRegexItems = regexItems;
    // 按照code值从小到大排序
    sort(sortedRegexItems.begin(), sortedRegexItems.end(), [](const RegexItem &a, const RegexItem &b) {
        return a.code < b.code;
    });
    
    // 提取排序后的code值
    vector<int> sortedTokenCodes;
    for (const auto &item : sortedRegexItems) {
        sortedTokenCodes.push_back(item.code);
    }
    
    // 将接受状态转换为列表，并按照数值大小排序
    vector<int> sortedAcceptStates(minimizedDFA.acceptStates.begin(), minimizedDFA.acceptStates.end());
    sort(sortedAcceptStates.begin(), sortedAcceptStates.end());
    
    // 将排序后的tokenCode分配给排序后的接受状态
    for (int i = 0; i < sortedAcceptStates.size(); i++) {
        int state = sortedAcceptStates[i];
        if (i < sortedTokenCodes.size() && state >= 0 && state < numStates) {
            tokens[state] = sortedTokenCodes[i];
        }
    }
    
    // 生成接受状态数组
    code += "bool isAcceptState[NUM_STATES] = {";
    for (int i = 0; i < isAccept.size(); i++) {
        if (i > 0) {
            code += ", ";
        }
        code += isAccept[i] ? "true" : "false";
    }
    code += "};\n";
    
    // 生成token代码数组
    code += "int acceptTokens[NUM_STATES] = {";
    for (int i = 0; i < tokens.size(); i++) {
        if (i > 0) {
            code += ", ";
        }
        code += to_string(tokens[i]);
    }
    code += "};\n";
    
    return code;
}

int main() {
    // 创建测试用的正则表达式项，顺序是乱的
    vector<RegexItem> regexItems;
    
    // 注释（102）
    RegexItem commentItem;
    commentItem.name = "_comment";
    commentItem.pattern = "\\{([^\\}]|\\})*\\}";
    commentItem.code = 102;
    commentItem.isMultiWord = false;
    commentItem.wordList.push_back("");
    regexItems.push_back(commentItem);
    
    // 标识符（100）
    RegexItem identifierItem;
    identifierItem.name = "_identifier";
    identifierItem.pattern = "letter(letter|digit)*";
    identifierItem.code = 100;
    identifierItem.isMultiWord = false;
    identifierItem.wordList.push_back("");
    regexItems.push_back(identifierItem);
    
    // 数字（101）
    RegexItem numberItem;
    numberItem.name = "_number";
    numberItem.pattern = "digit+(.digit+)?(e[+-]?digit+)?";
    numberItem.code = 101;
    numberItem.isMultiWord = false;
    numberItem.wordList.push_back("");
    regexItems.push_back(numberItem);
    
    // 其他符号（103）
    RegexItem symbolItem;
    symbolItem.name = "_symbol";
    symbolItem.pattern = "[^\\w\\s\\{\\}]";
    symbolItem.code = 103;
    symbolItem.isMultiWord = false;
    symbolItem.wordList.push_back("");
    regexItems.push_back(symbolItem);
    
    // 关键字（200）
    RegexItem keywordItem;
    keywordItem.name = "_keyword";
    keywordItem.pattern = "(if|then|else|while|do|repeat|until|read|write|begin|end)";
    keywordItem.code = 200;
    keywordItem.isMultiWord = true;
    keywordItem.wordList.push_back("if");
    keywordItem.wordList.push_back("then");
    regexItems.push_back(keywordItem);
    
    // 创建测试用的最小化DFA
    DFA minimizedDFA;
    
    // 设置DFA状态
    minimizedDFA.states = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    
    // 设置DFA起始状态
    minimizedDFA.startState = 5;
    
    // 设置DFA接受状态
    minimizedDFA.acceptStates = {0, 1, 2, 3, 4};
    
    // 设置接受状态到正则表达式索引的映射
    // 状态0对应索引0（注释，code=102）
    minimizedDFA.acceptStateToRegexIndex[0] = 0;
    // 状态1对应索引1（标识符，code=100）
    minimizedDFA.acceptStateToRegexIndex[1] = 1;
    // 状态2对应索引2（数字，code=101）
    minimizedDFA.acceptStateToRegexIndex[2] = 2;
    // 状态3对应索引3（其他符号，code=103）
    minimizedDFA.acceptStateToRegexIndex[3] = 3;
    // 状态4对应索引4（关键字，code=200）
    minimizedDFA.acceptStateToRegexIndex[4] = 4;
    
    // 生成接受状态映射
    string acceptStatesMap = generateAcceptStatesMap(regexItems, minimizedDFA);
    
    // 输出结果
    cout << acceptStatesMap << endl;
    
    return 0;
}
