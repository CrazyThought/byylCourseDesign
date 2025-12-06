#include "lexergenerator.h"
#include <QDebug>
#include <QMap>

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
    code += "#include <cctype>\n";
    code += "#include <map>\n";
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
    code += "    // 初始化多单词映射\n";
    code += "    initMultiWordMap();\n";
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
    code += "        int matchedRegexIndex = -1;\n";
    code += "\n";

    // 生成每个正则表达式的匹配逻辑
    for (const RegexItem &item : regexItems) {
        if (item.name.startsWith('_')) {
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
    code += "            // 检查是否是多单词情况\n";
    code += "            if (matchedRegexIndex >= 0) {\n";
    code += "                const auto &regexItem = regexItems[matchedRegexIndex];\n";
    code += "                if (regexItem.isMultiWord) {\n";
    code += "                    // 多单词情况：根据匹配到的具体单词返回对应的编码\n";
    code += "                    // 检查是否在多单词映射中\n";
    code += "                    auto it = multiWordMap.find(matchedLexeme);\n";
    code += "                    if (it != multiWordMap.end()) {\n";
    code += "                        tokenCode = it->second;\n";
    code += "                    } else {\n";
    code += "                        // 如果不在映射中，动态生成编码\n";
    code += "                        static std::map<int, int> nextCodeMap;\n";
    code += "                        if (nextCodeMap.find(regexItem.code) == nextCodeMap.end()) {\n";
    code += "                            nextCodeMap[regexItem.code] = regexItem.code + 1;\n";
    code += "                        }\n";
    code += "                        tokenCode = nextCodeMap[regexItem.code]++;\n";
    code += "                        multiWordMap[matchedLexeme] = tokenCode;\n";
    code += "                    }\n";
    code += "                }\n";
    code += "            }\n";
    code += "            tokens.push_back({tokenCode, matchedLexeme, line});\n";
    code += "            pos += maxMatchLen;\n";
    code += "        } else {\n";
    code += "            std::cerr << \"Error at line \" << line << \": Invalid character '\" << source[pos] << \"'\\n\";\n";
    code += "            pos++;\n";
    code += "        }\n";
    code += "    }\n";
    code += "\n";
    code += "    return tokens;\n";
    code += "}\n";
    code += "\n";

    // 主函数示例
    code += "int main() {\n";
    code += "    std::string source;\n";
    code += "    std::getline(std::cin, source, '\\0');\n";
    code += "    \n";
    code += "    std::vector<Token> tokens = lexicalAnalysis(source);\n";
    code += "    \n";
    code += "    for (const auto &token : tokens) {\n";
    code += "        std::cout << \"Token(code: \" << token.code << \", lexeme: \\\"\" << token.lexeme << \\\"\", line: \" << token.line << \")\\n\";\n";
    code += "    }\n";
    code += "    \n";
    code += "    return 0;\n";
    code += "}\n";

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
    code += "#include <cctype>\n";
    code += "#include <cstring>\n";
    code += "#include <map>\n";
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
    
    // 生成DFA状态转移表
    code += generateStateTransitionTable(minimizedDFA);
    code += "\n";
    
    // 生成接受状态映射
    code += generateAcceptStatesMap(regexItems, minimizedDFA);
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
    code += "        size_t startPos = pos;\n";
    code += "        size_t maxMatchPos = pos;\n";
    code += "        int tokenCode = -1;\n";
    code += "\n";
    code += "        // 尝试最长匹配\n";
    code += "        while (pos < source.length()) {\n";
    code += "            unsigned char c = static_cast<unsigned char>(source[pos]);\n";
    code += "            int nextState = transitions[currentState][c];\n";
    code += "\n";
    code += "            if (nextState == ERROR_STATE) break;\n";
    code += "\n";
    code += "            currentState = nextState;\n";
    code += "            pos++;\n";
    code += "\n";
    code += "            // 检查是否是接受状态\n";
    code += "            if (isAcceptState[currentState]) {\n";
    code += "                maxMatchPos = pos;\n";
    code += "                tokenCode = acceptTokens[currentState];\n";
    code += "            }\n";
    code += "        }\n";
    code += "\n";
    code += "        if (tokenCode != -1) {\n";
    code += "            std::string lexeme = source.substr(startPos, maxMatchPos - startPos);\n";
    code += "            // 检查是否是多单词情况\n";
    code += "            // 多单词情况：根据匹配到的具体单词返回对应的编码\n";
    code += "            // 检查是否在多单词映射中\n";
    code += "            auto it = multiWordMap.find(lexeme);\n";
    code += "            if (it != multiWordMap.end()) {\n";
    code += "                tokenCode = it->second;\n";
    code += "            } else {\n";
    code += "                // 动态处理多单词情况\n";
    code += "                // 这里需要根据实际情况调整，为每个多单词生成连续编码\n";
    code += "                // 我们假设tokenCode是多单词的基础编码\n";
    code += "                static std::map<int, std::map<std::string, int>> multiWordCodeMap;\n";
    code += "                if (multiWordCodeMap.find(tokenCode) == multiWordCodeMap.end()) {\n";
    code += "                    // 初始化多单词编码映射\n";
    code += "                    multiWordCodeMap[tokenCode] = std::map<std::string, int>();\n";
    code += "                }\n";
    code += "                if (multiWordCodeMap[tokenCode].find(lexeme) == multiWordCodeMap[tokenCode].end()) {\n";
    code += "                    // 为新单词分配编码\n";
    code += "                    int newCode = tokenCode + multiWordCodeMap[tokenCode].size() + 1;\n";
    code += "                    multiWordCodeMap[tokenCode][lexeme] = newCode;\n";
    code += "                    multiWordMap[lexeme] = newCode;\n";
    code += "                    tokenCode = newCode;\n";
    code += "                } else {\n";
    code += "                    tokenCode = multiWordCodeMap[tokenCode][lexeme];\n";
    code += "                    multiWordMap[lexeme] = tokenCode;\n";
    code += "                }\n";
    code += "            }\n";
    code += "            tokens.push_back({tokenCode, lexeme, line});\n";
    code += "            pos = maxMatchPos;\n";
    code += "        } else {\n";
    code += "            std::cerr << \"Error at line \" << line << \": Invalid character '\" << source[startPos] << \"'\\n\";\n";
    code += "            pos = startPos + 1;\n";
    code += "        }\n";
    code += "    }\n";
    code += "\n";
    code += "    return tokens;\n";
    code += "}\n";
    code += "\n";
    
    // 主函数示例
    code += "int main() {\n";
    code += "    std::string source;\n";
    code += "    std::getline(std::cin, source, '\\0');\n";
    code += "    \n";
    code += "    std::vector<Token> tokens = lexicalAnalysis(source);\n";
    code += "    \n";
    code += "    for (const auto &token : tokens) {\n";
    code += "        std::cout << \"Token(code: \" << token.code << \", lexeme: \\\"\" << token.lexeme << \\\"\", line: \" << token.line << \")\\n\";\n";
    code += "    }\n";
    code += "    \n";
    code += "    return 0;\n";
    code += "}\n";
    
    return code;
}

QString LexerGenerator::generateStateTransitionTable(const DFA &minimizedDFA)
{
    QString code;
    
    code += "// DFA状态转移表\n";
    code += "const int NUM_STATES = " + QString::number(minimizedDFA.states.size()) + ";\n";
    code += "const int ERROR_STATE = -1;\n";
    code += "\n";
    
    // 创建一个二维数组表示状态转移表
    code += "// 状态转移表: transitions[当前状态][输入字符] = 下一个状态\n";
    code += "int transitions[NUM_STATES][256] = {\n";
    
    // 初始化所有转移为错误状态
    for (int i = 0; i < minimizedDFA.states.size(); i++) {
        code += "    {";
        for (int j = 0; j < 256; j++) {
            if (j > 0) code += ", ";
            code += "ERROR_STATE";
        }
        code += "}";
        if (i < minimizedDFA.states.size() - 1) code += ",\n";
    }
    
    code += "\n};\n";
    code += "\n";
    
    // 设置实际的转移
    code += "// 初始化转移表\n";
    for (const auto &transition : minimizedDFA.transitions) {
        QString inputSymbol = getInputSymbol(transition.input);
        
        if (transition.input.size() == 1) {
            // 单个字符，使用ASCII值进行索引
            char c = transition.input.at(0).toLatin1();
            int asciiValue = static_cast<int>(static_cast<unsigned char>(c));
            code += QString("transitions[%1][%2] = %3;\n")
                    .arg(transition.fromState)
                    .arg(asciiValue)
                    .arg(transition.toState);
        } else {
            // 特殊字符处理
            if (transition.input == "\\n") {
                // 换行符 ASCII值10
                code += QString("transitions[%1][10] = %2;\n")
                        .arg(transition.fromState)
                        .arg(transition.toState);
            } else if (transition.input == "\\t") {
                // 制表符 ASCII值9
                code += QString("transitions[%1][9] = %2;\n")
                        .arg(transition.fromState)
                        .arg(transition.toState);
            } else if (transition.input == "\\r") {
                // 回车符 ASCII值13
                code += QString("transitions[%1][13] = %2;\n")
                        .arg(transition.fromState)
                        .arg(transition.toState);
            } else if (transition.input == "\\0") {
                // 空字符 ASCII值0
                code += QString("transitions[%1][0] = %2;\n")
                        .arg(transition.fromState)
                        .arg(transition.toState);
            } else {
                // 其他特殊字符，尝试直接解析为ASCII值
                QByteArray utf8 = transition.input.toUtf8();
                if (utf8.size() == 1) {
                    int asciiValue = static_cast<int>(static_cast<unsigned char>(utf8.at(0)));
                    code += QString("transitions[%1][%2] = %3;\n")
                            .arg(transition.fromState)
                            .arg(asciiValue)
                            .arg(transition.toState);
                }
            }
        }
    }
    
    return code;
}

QString LexerGenerator::generateAcceptStatesMap(const QList<RegexItem> &regexItems, const DFA &minimizedDFA)
{
    QString code;
    
    code += "// 接受状态映射\n";
    code += "bool isAcceptState[NUM_STATES] = {false};\n";
    code += "int acceptTokens[NUM_STATES] = {-1};\n";
    code += "\n";
    
    // 设置所有接受状态
    int stateIndex = 0;
    for (const auto &state : minimizedDFA.acceptStates) {
        code += QString("isAcceptState[%1] = true;\n").arg(state);
        
        // 为每个接受状态分配正确的token code
        int regexIndex = 0;
        
        // 优先使用DFA中的acceptStateToRegexIndex映射
        if (minimizedDFA.acceptStateToRegexIndex.contains(state)) {
            regexIndex = minimizedDFA.acceptStateToRegexIndex[state];
        } else {
            // 如果映射不存在，使用stateIndex作为备选方案
            regexIndex = stateIndex % regexItems.size();
            stateIndex++;
        }
        
        if (regexIndex < regexItems.size()) {
            code += QString("acceptTokens[%1] = %2;\n")
                    .arg(state)
                    .arg(regexItems[regexIndex].code);
        }
    }
    
    return code;
}

QString LexerGenerator::generateTokenCodeMap(const QList<RegexItem> &regexItems)
{
    QString code;
    
    code += "// 单词编码映射\n";
    code += "enum TokenCode {\n";
    
    for (const auto &regexItem : regexItems) {
        if (regexItem.name.startsWith('_')) {
            QString tokenName = regexItem.name.mid(1).toUpper();
            
            // 检查是否为多单词
            if (regexItem.isMultiWord) {
                // 对于多单词，从指定编码开始生成连续编码
                code += "    " + tokenName + " = " + QString::number(regexItem.code) + ",\n";
                
                // 为每个单词生成编码
                int currentCode = regexItem.code;
                for (const QString &word : regexItem.wordList) {
                    currentCode++;
                    // 将单词转换为合法的枚举名称（替换特殊字符）
                    QString wordTokenName = tokenName + "_" + word.toUpper().replace("+", "PLUS").replace("-", "MINUS").replace("*", "STAR").replace("/", "SLASH").replace(".", "DOT").replace("?", "QUESTION").replace("^", "CARET").replace("$", "DOLLAR").replace("(", "LPAREN").replace(")", "RPAREN").replace("[", "LBRACKET").replace("]", "RBRACKET").replace("{", "LBRACE").replace("}", "RBRACE").replace("|", "PIPE").replace("\\", "BACKSLASH");
                    code += "    " + wordTokenName + " = " + QString::number(currentCode) + ",\n";
                }
            } else {
                // 对于单单词，直接使用指定编码
                code += "    " + tokenName + " = " + QString::number(regexItem.code) + ",\n";
            }
        }
    }
    
    // 移除最后一个逗号
    if (code.endsWith(",\n")) {
        code.chop(2);
        code += "\n";
    }
    
    code += "};\n";
    
    // 为多单词添加特殊处理代码
    code += "\n";
    code += "// 多单词映射：从字符串到编码的映射\n";
    code += "std::map<std::string, int> multiWordMap;\n";
    code += "void initMultiWordMap() {\n";
    
    // 初始化多单词映射
    for (const auto &regexItem : regexItems) {
        if (regexItem.name.startsWith('_') && regexItem.isMultiWord) {
            int currentCode = regexItem.code;
            for (const QString &word : regexItem.wordList) {
                currentCode++;
                code += "    multiWordMap[\"" + word + "\"] = " + QString::number(currentCode) + ";\n";
            }
        }
    }
    
    code += "}\n";
    
    return code;
}

QString LexerGenerator::getInputSymbol(const QString &input)
{
    if (input == "\\n") return "\\n";
    if (input == "\\t") return "\\t";
    if (input == "\\r") return "\\r";
    if (input == "\\0") return "\\0";
    return input;
}