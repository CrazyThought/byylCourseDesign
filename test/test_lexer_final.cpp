#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <windows.h>

using namespace std;

// 运行词法分析器并获取结果
vector<pair<string, int>> runLexer(const string &lexerPath, const string &testFile) {
    vector<pair<string, int>> results;
    
    // 构建命令
    string command = lexerPath + " " + testFile + " > output.txt";
    
    // 运行命令
    int result = system(command.c_str());
    
    if (result != 0) {
        cerr << "词法分析器运行失败" << endl;
        return results;
    }
    
    // 读取输出文件
    ifstream outputFile("output.txt");
    if (!outputFile.is_open()) {
        cerr << "无法打开输出文件" << endl;
        return results;
    }
    
    string line;
    while (getline(outputFile, line)) {
        // 解析每行的单词和编码
        size_t tabPos = line.find('\t');
        if (tabPos != string::npos) {
            string lexeme = line.substr(0, tabPos);
            int tokenCode = stoi(line.substr(tabPos + 1));
            results.emplace_back(lexeme, tokenCode);
        }
    }
    
    outputFile.close();
    
    // 清理临时文件
    remove("output.txt");
    
    return results;
}

// 比较结果
bool compareResults(const vector<pair<string, int>> &actual, const vector<pair<string, int>> &expected) {
    if (actual.size() != expected.size()) {
        cout << "结果数量不匹配: 实际 " << actual.size() << "，预期 " << expected.size() << endl;
        return false;
    }
    
    for (size_t i = 0; i < actual.size(); i++) {
        if (actual[i].first != expected[i].first || actual[i].second != expected[i].second) {
            cout << "结果不匹配在位置 " << i << ":" << endl;
            cout << "实际: " << actual[i].first << " (" << actual[i].second << ")" << endl;
            cout << "预期: " << expected[i].first << " (" << expected[i].second << ")" << endl;
            return false;
        }
    }
    
    return true;
}

int main() {
    cout << "=== 词法分析器最终测试 ===" << endl;
    
    // 编译词法分析器
    cout << "编译词法分析器..." << endl;
    int compileResult = system("g++ -o lexer.exe test/lexer.cpp");
    if (compileResult != 0) {
        cerr << "编译失败" << endl;
        return 1;
    }
    cout << "编译成功" << endl;
    
    // 测试文件路径
    string lexerPath = ".\\lexer.exe";
    string testFile = "test\\Tiny\\tiny_sample.tny";
    
    // 运行词法分析器
    cout << "运行词法分析器..." << endl;
    vector<pair<string, int>> actualResults = runLexer(lexerPath, testFile);
    
    if (actualResults.empty()) {
        cerr << "没有获取到分析结果" << endl;
        return 1;
    }
    
    // 预期结果
    vector<pair<string, int>> expectedResults = {
        {"{thisisacomment}", 102},
        {"read", 200},
        {"x", 100},
        {";" , 103},
        {"if", 200},
        {"0", 101},
        {"<", 103},
        {"x", 100},
        {"then", 200},
        {"fact", 100},
        {":=", 103},
        {"1", 101},
        {";" , 103},
        {"repeat", 200},
        {"fact", 100},
        {":=", 103},
        {"fact", 100},
        {"*", 103},
        {"x", 100},
        {";" , 103},
        {"x", 100},
        {":=", 103},
        {"x", 100},
        {"-", 103},
        {"1", 101},
        {"until", 200},
        {"x", 100},
        {"=", 103},
        {"0", 101},
        {";" , 103},
        {"write", 200},
        {"fact", 100},
        {"end", 200}
    };
    
    // 比较结果
    cout << "比较结果..." << endl;
    bool success = compareResults(actualResults, expectedResults);
    
    if (success) {
        cout << "测试通过！词法分析器能够正确识别所有token类型。" << endl;
    } else {
        cout << "测试失败！词法分析器无法正确识别所有token类型。" << endl;
        return 1;
    }
    
    // 清理
    remove("lexer.exe");
    
    cout << "=== 测试完成 ===" << endl;
    return 0;
}
