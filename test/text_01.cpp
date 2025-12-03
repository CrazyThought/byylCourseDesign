#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <algorithm>

struct Token {
    int code;
    std::string lexeme;
    int line;
};

// Token code definitions
#define TOKEN_NUM100 0
#define TOKEN_ID101 1
#define TOKEN_SPECAIL200S 0

const std::map<int, std::string> tokenNames = {
    {TOKEN_NUM100, "num100"},
    {TOKEN_ID101, "ID101"},
    {TOKEN_SPECAIL200S, "specail200S"},
};

// DFA State Transition Table
const std::map<int, std::map<int, int>> stateTransitionTable = {
    {0, {
        {71, 0},
        {69, 0},
        {89, 0},
        {97, 0},
        {68, 0},
        {119, 0},
        {104, 0},
        {113, 0},
        {108, 0},
        {49, 0},
        {50, 0},
        {57, 0},
        {117, 0},
        {86, 0},
        {55, 0},
        {87, 0},
        {111, 0},
        {112, 0},
        {72, 0},
        {118, 0},
        {85, 0},
        {73, 0},
        {76, 0},
        {90, 0},
        {51, 0},
        {83, 0},
        {82, 0},
        {65, 0},
        {98, 0},
        {88, 0},
        {70, 0},
        {103, 0},
        {78, 0},
        {81, 0},
        {48, 0},
        {109, 0},
        {80, 0},
        {53, 0},
        {54, 0},
        {110, 0},
        {79, 0},
        {84, 0},
        {100, 0},
        {102, 0},
        {121, 0},
        {120, 0},
        {114, 0},
        {66, 0},
        {77, 0},
        {99, 0},
        {116, 0},
        {122, 0},
        {106, 0},
        {67, 0},
        {75, 0},
        {115, 0},
        {56, 0},
        {101, 0},
        {105, 0},
        {52, 0},
        {107, 0},
        {74, 0},
    }},
    {1, {
        {71, 0},
        {69, 0},
        {89, 0},
        {97, 0},
        {68, 0},
        {119, 0},
        {104, 0},
        {113, 0},
        {108, 0},
        {117, 0},
        {86, 0},
        {87, 0},
        {111, 0},
        {112, 0},
        {72, 0},
        {118, 0},
        {85, 0},
        {73, 0},
        {76, 0},
        {90, 0},
        {83, 0},
        {82, 0},
        {65, 0},
        {98, 0},
        {88, 0},
        {70, 0},
        {103, 0},
        {78, 0},
        {81, 0},
        {109, 0},
        {80, 0},
        {110, 0},
        {79, 0},
        {84, 0},
        {100, 0},
        {102, 0},
        {121, 0},
        {120, 0},
        {114, 0},
        {66, 0},
        {77, 0},
        {99, 0},
        {116, 0},
        {122, 0},
        {106, 0},
        {67, 0},
        {75, 0},
        {115, 0},
        {101, 0},
        {105, 0},
        {107, 0},
        {74, 0},
    }},
};

// Accept States Map
const std::map<int, int> acceptStates = {
    {0, 0},
};

int getInputSymbol(char c) {
    if (std::isalpha(c)) return 0;
    if (std::isdigit(c)) return 1;
    if (std::isspace(c)) return 2;
    // 其他符号返回ASCII值
    return static_cast<int>(c);
}

std::vector<Token> lexicalAnalysis(const std::string &source) {
    std::vector<Token> tokens;
    int line = 1;
    size_t pos = 0;

    while (pos < source.length()) {
        // 跳过空白字符
        if (std::isspace(source[pos])) {
            if (source[pos] == '\n') line++;
            pos++;
            continue;
        }

        int currentState = 0;
        size_t lastAcceptPos = pos;
        int lastAcceptState = -1;

        // 模拟DFA状态转移
        for (size_t i = pos; i < source.length(); i++) {
            char c = source[i];
            int inputSymbol = getInputSymbol(c);
            
            if (stateTransitionTable[currentState].find(inputSymbol) == stateTransitionTable[currentState].end()) {
                break;
            }
            
            currentState = stateTransitionTable[currentState][inputSymbol];
            
            if (acceptStates.find(currentState) != acceptStates.end()) {
                lastAcceptPos = i + 1;
                lastAcceptState = currentState;
            }
        }

        if (lastAcceptState != -1) {
            std::string lexeme = source.substr(pos, lastAcceptPos - pos);
            int tokenCode = acceptStates[lastAcceptState];
            tokens.push_back({tokenCode, lexeme, line});
            pos = lastAcceptPos;
        } else {
            std::cerr << "Error at line " << line << ": Unexpected character '" << source[pos] << "" << std::endl;
            pos++;
        }
    }

    return tokens;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
        return 1;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Failed to open file: " << argv[1] << "";
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    input.close();

    std::vector<Token> tokens = lexicalAnalysis(source);

    // 输出到控制台
    for (const auto &token : tokens) {
        std::cout << token.line << "	" << token.lexeme << "	" << token.code << "";
    }

    // 输出到文件
    std::string outputFile = std::string(argv[1]) + "\.lex";
    std::ofstream output(outputFile);
    if (output.is_open()) {
        for (const auto &token : tokens) {
            output << token.line << "	" << token.lexeme << "	" << token.code << "";
        }
        output.close();
    }

    return 0;
}
