#include "lexergenerator.h"
#include <QDebug>

LexerGenerator::LexerGenerator()
{
}

LexerGenerator::~LexerGenerator()
{
}

QString LexerGenerator::generateLexer(const QList<RegexItem> &regexItems, const DFA &minimizedDFA, GenerationMethod method)
{
    m_errorMessage.clear();
    m_regexItems = regexItems;
    
    // 检查输入是否有效
    if (regexItems.isEmpty()) {
        m_errorMessage = "正则表达式列表为空";
        return "";
    }
    
    if (minimizedDFA.states.isEmpty()) {
        m_errorMessage = "最小化DFA为空";
        return "";
    }
    
    // 根据选择的方法生成词法分析器
    switch (method) {
    case GenerationMethod::DIRECT_MATCH:
        return generateDirectMatchLexer(regexItems, minimizedDFA);
    case GenerationMethod::STATE_TRANSITION:
        return generateStateTransitionLexer(regexItems, minimizedDFA);
    default:
        m_errorMessage = "无效的生成方法";
        return "";
    }
}

QString LexerGenerator::getErrorMessage() const
{
    return m_errorMessage;
}

QString LexerGenerator::generateDirectMatchLexer(const QList<RegexItem> &regexItems, const DFA &/*minimizedDFA*/)
{
    QString code;
    
    // 生成头文件
    code += "#include <iostream>\n";
    code += "#include <fstream>\n";
    code += "#include <string>\n";
    code += "#include <regex>\n";
    code += "#include <vector>\n";
    code += "\n";
    
    // 生成词法单元结构
    code += "struct Token {\n";
    code += "    int code;\n";
    code += "    std::string lexeme;\n";
    code += "    int line;\n";
    code += "};\n";
    code += "\n";
    
    // 生成单词编码映射
    code += generateTokenCodeMap(regexItems);
    code += "\n";
    
    // 生成词法分析函数
    code += "std::vector<Token> lexicalAnalysis(const std::string &source) {\n";
    code += "    std::vector<Token> tokens;\n";
    code += "    int line = 1;\n";
    code += "    size_t pos = 0;\n";
    code += "\n";
    code += "    while (pos < source.length()) {\n";
    code += "        // 跳过空白字符\n";
    code += "        if (std::isspace(source[pos])) {\n";
    code += "            if (source[pos] == '\\n') line++;\n";
    code += "            pos++;\n";
    code += "            continue;\n";
    code += "        }\n";
    code += "\n";
    code += "        bool matched = false;\n";
    code += "        size_t maxMatchLen = 0;\n";
    code += "        int tokenCode = -1;\n";
    code += "        std::string matchedLexeme;\n";
    code += "\n";
    
    // 生成每个正则表达式的匹配逻辑
    for (const RegexItem &item : regexItems) {
        if (item.name.startsWith('_')) { // 只生成以下划线开头的正则表达式
            code += "        // 匹配 " + item.name.mid(1) + "\n";
            code += "        {\n";
            code += "            std::regex pattern(\"" + item.pattern + "\");\n";
            code += "            std::smatch match;\n";
            code += "            if (std::regex_search(source.begin() + pos, source.end(), match, pattern) && match.position() == 0) {\n";
            code += "                if (match.length() > maxMatchLen) {\n";
            code += "                    maxMatchLen = match.length();\n";
            code += "                    tokenCode = " + QString::number(item.code) + ";\n";
            code += "                    matchedLexeme = match.str();\n";
            code += "                    matched = true;\n";
            code += "                }\n";
            code += "            }\n";
            code += "        }\n";
        }
    }
    
    code += "\n";
    code += "        if (matched) {\n";
    code += "            tokens.push_back({tokenCode, matchedLexeme, line});\n";
    code += "            pos += maxMatchLen;\n";
    code += "        } else {\n";
    code += "            std::cerr << \"Error at line \" << line << \": Unexpected character '\" << source[pos] << \"' << std::endl;\n";
    code += "            pos++;\n";
    code += "        }\n";
    code += "    }\n";
    code += "\n";
    code += "    return tokens;\n";
    code += "}\n";
    code += "\n";
    
    // 生成主函数
    code += "int main(int argc, char *argv[]) {\n";
    code += "    if (argc != 2) {\n";
    code += "        std::cerr << \"Usage: \" << argv[0] << \" <source_file>\" << std::endl;\n";
    code += "        return 1;\n";
    code += "    }\n";
    code += "\n";
    code += "    std::ifstream input(argv[1]);\n";
    code += "    if (!input.is_open()) {\n";
    code += "        std::cerr << \"Failed to open file: \" << argv[1] << \"\n\";\n";    code += "        return 1;\n";
    code += "    }\n";
    code += "\n";
    code += "    std::string source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());\n";
    code += "    input.close();\n";
    code += "\n";
    code += "    std::vector<Token> tokens = lexicalAnalysis(source);\n";
    code += "\n";
    code += "    // 输出到控制台\n";
    code += "    for (const auto &token : tokens) {\n";
    code += "        std::cout << token.line << \"\t\" << token.lexeme << \"\t\" << token.code << \"\n\";\n";
    code += "    }\n";
    code += "\n";
    
    // 输出到文件
    code += "    std::string outputFile = std::string(argv[1]) + \"\\.lex\";\n";
    code += "    std::ofstream output(outputFile);\n";
    code += "    if (output.is_open()) {\n";
    code += "        for (const auto &token : tokens) {\n";
    code += "            output << token.line << \"\t\" << token.lexeme << \"\t\" << token.code << \"\n\";\n";
    code += "        }\n";
    code += "        output.close();\n";
    code += "    }\n";
    code += "\n";
    code += "    return 0;\n";
    code += "}\n";
    
    return code;
}

QString LexerGenerator::generateTokenCodeMap(const QList<RegexItem> &regexItems)
{
    QString code;
    
    // 生成单词编码宏定义
    code += "// Token code definitions\n";
    for (const RegexItem &item : regexItems) {
        if (item.name.startsWith('_')) {
            code += "#define TOKEN_" + item.name.mid(1).toUpper() + " " + QString::number(item.code) + "\n";
        }
    }
    code += "\n";
    
    // 生成单词名称映射
    code += "const std::map<int, std::string> tokenNames = {\n";
    for (const RegexItem &item : regexItems) {
        if (item.name.startsWith('_')) {
            code += "    {TOKEN_" + item.name.mid(1).toUpper() + ", \"" + item.name.mid(1) + "\"},\n";
        }
    }
    code += "};\n";
    
    return code;
}

QString LexerGenerator::generateStateTransitionTable(const DFA &minimizedDFA)
{
    QString code;
    
    // 生成状态转移表
    code += "// DFA State Transition Table\n";
    code += "const std::map<int, std::map<int, int>> stateTransitionTable = {\n";
    
    // 遍历所有DFA状态
    for (const DFAState &state : minimizedDFA.states) {
        code += QString("    {%1, {\n").arg(state);
        
        // 遍历所有转换，找出属于当前状态的转换
        bool firstTransition = true;
        for (const auto &transition : minimizedDFA.transitions) {
            if (transition.fromState == state) {
                if (!firstTransition) {
                    code += ",\n";
                }
                firstTransition = false;
                
                QString inputSymbol;
                if (transition.input == "a-z" || transition.input == "A-Z") {
                    inputSymbol = "0";
                } else if (transition.input == "0-9") {
                    inputSymbol = "1";
                } else if (transition.input == " ") {
                    inputSymbol = "2";
                } else {
                    inputSymbol = QString::number(transition.input.at(0).toLatin1());
                }
                
                code += QString("        {%1, %2},\n").arg(inputSymbol).arg(transition.toState);
            }
        }
        
        code += "    }},\n";
    }
    
    code += "};\n";
    
    return code;
}

QString LexerGenerator::generateAcceptStatesMap(const QList<RegexItem> &regexItems, const DFA &minimizedDFA)
{
    QString code;
    
    // 生成接受状态映射
    code += "// Accept States Map\n";
    code += "const std::map<int, int> acceptStates = {\n";
    
    // 遍历所有DFA接受状态
    for (const DFAState &state : minimizedDFA.acceptStates) {
        // 找到对应的正则表达式（这里简化处理，实际需要根据状态映射获取）
        code += QString("    {%1, 0},\n").arg(state);
    }
    
    code += "};\n";
    
    return code;
}

QString LexerGenerator::generateStateTransitionLexer(const QList<RegexItem> &regexItems, const DFA &minimizedDFA)
{
    QString code;
    
    // 生成头文件
    code += "#include <iostream>\n";
    code += "#include <fstream>\n";
    code += "#include <string>\n";
    code += "#include <vector>\n";
    code += "#include <map>\n";
    code += "#include <cctype>\n";
    code += "#include <algorithm>\n";
    code += "\n";
    
    // 生成词法单元结构
    code += "struct Token {\n";
    code += "    int code;\n";
    code += "    std::string lexeme;\n";
    code += "    int line;\n";
    code += "};\n";
    code += "\n";
    
    // 生成单词编码映射
    code += generateTokenCodeMap(regexItems);
    code += "\n";
    
    // 生成状态转移表
    code += generateStateTransitionTable(minimizedDFA);
    code += "\n";
    
    // 生成接受状态映射
    code += generateAcceptStatesMap(regexItems, minimizedDFA);
    code += "\n";
    
    // 生成获取输入符号的辅助函数
    code += "int getInputSymbol(char c) {\n";
    code += "    if (std::isalpha(c)) return 0;\n";
    code += "    if (std::isdigit(c)) return 1;\n";
    code += "    if (std::isspace(c)) return 2;\n";
    code += "    // 其他符号返回ASCII值\n";
    code += "    return static_cast<int>(c);\n";
    code += "}\n";
    code += "\n";
    
    // 生成词法分析函数
    code += "std::vector<Token> lexicalAnalysis(const std::string &source) {\n";
    code += "    std::vector<Token> tokens;\n";
    code += "    int line = 1;\n";
    code += "    size_t pos = 0;\n";
    code += "\n";
    code += "    while (pos < source.length()) {\n";
    code += "        // 跳过空白字符\n";
    code += "        if (std::isspace(source[pos])) {\n";
    code += "            if (source[pos] == '\\n') line++;\n";
    code += "            pos++;\n";
    code += "            continue;\n";
    code += "        }\n";
    code += "\n";
    code += "        int currentState = 0;\n";
    code += "        size_t lastAcceptPos = pos;\n";
    code += "        int lastAcceptState = -1;\n";
    code += "\n";
    code += "        // 模拟DFA状态转移\n";
    code += "        for (size_t i = pos; i < source.length(); i++) {\n";
    code += "            char c = source[i];\n";
    code += "            int inputSymbol = getInputSymbol(c);\n";
    code += "            \n";
    code += "            if (stateTransitionTable[currentState].find(inputSymbol) == stateTransitionTable[currentState].end()) {\n";
    code += "                break;\n";
    code += "            }\n";
    code += "            \n";
    code += "            currentState = stateTransitionTable[currentState][inputSymbol];\n";
    code += "            \n";
    code += "            if (acceptStates.find(currentState) != acceptStates.end()) {\n";
    code += "                lastAcceptPos = i + 1;\n";
    code += "                lastAcceptState = currentState;\n";
    code += "            }\n";
    code += "        }\n";
    code += "\n";
    code += "        if (lastAcceptState != -1) {\n";
    code += "            std::string lexeme = source.substr(pos, lastAcceptPos - pos);\n";
    code += "            int tokenCode = acceptStates[lastAcceptState];\n";
    code += "            tokens.push_back({tokenCode, lexeme, line});\n";
    code += "            pos = lastAcceptPos;\n";
    code += "        } else {\n";
    code += "            std::cerr << \"Error at line \" << line << \": Unexpected character '\" << source[pos] << \"' << std::endl;\n";
    code += "            pos++;\n";
    code += "        }\n";
    code += "    }\n";
    code += "\n";
    code += "    return tokens;\n";
    code += "}\n";
    code += "\n";
    
    // 生成主函数
    code += "int main(int argc, char *argv[]) {\n";
    code += "    if (argc != 2) {\n";
  code += "        std::cerr << \"Usage: \" << argv[0] << \" <source_file>\" << std::endl;\n";
    code += "        return 1;\n";
    code += "    }\n";
    code += "\n";
    code += "    std::ifstream input(argv[1]);\n";
    code += "    if (!input.is_open()) {\n";
    code += "        std::cerr << \"Failed to open file: \" << argv[1] << \"\n\";\n";
    code += "        return 1;\n";
    code += "    }\n";
    code += "\n";
    code += "    std::string source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());\n";
    code += "    input.close();\n";
    code += "\n";
    code += "    std::vector<Token> tokens = lexicalAnalysis(source);\n";
    code += "\n";
    code += "    // 输出到控制台\n";
    code += "    for (const auto &token : tokens) {\n";
    code += "        std::cout << token.line << \"\t\" << token.lexeme << \"\t\" << token.code << \"\n\";\n";
    code += "    }\n";
    code += "\n";
    code += "    // 输出到文件\n";
    code += "    std::string outputFile = std::string(argv[1]) + \"\\.lex\";\n";
    code += "    std::ofstream output(outputFile);\n";
    code += "    if (output.is_open()) {\n";
    code += "        for (const auto &token : tokens) {\n";
    code += "            output << token.line << \"\t\" << token.lexeme << \"\t\" << token.code << \"\n\";\n";
    code += "        }\n";
    code += "        output.close();\n";
    code += "    }\n";
    code += "\n";
    code += "    return 0;\n";
    code += "}\n";
    
    return code;
}