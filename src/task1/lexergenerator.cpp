/*
 * @file lexergenerator.cpp
 * @id lexergenerator-cpp
 * @brief 实现词法分析器生成功能，支持直接匹配和状态转移两种生成方法
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task1/lexergenerator.h"
#include <QDebug>
#include <QMap>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#define ERROR_STATE -1

/**
 * @brief 构造函数
 * 
 * 初始化词法分析器生成器
 */
LexerGenerator::LexerGenerator()
{
}

/**
 * @brief 析构函数
 * 
 * 清理词法分析器生成器资源
 */
LexerGenerator::~LexerGenerator()
{
}

/**
 * @brief 生成词法分析器
 * 
 * 根据正则表达式项和最小化DFA，使用指定的生成方法生成词法分析器代码
 * 
 * @param regexItems 正则表达式项列表
 * @param minimizedDFA 最小化DFA
 * @param method 生成方法（直接匹配或状态转移）
 * @return QString 生成的词法分析器代码
 */
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

/**
 * @brief 获取错误信息
 * 
 * @return QString 错误信息
 */
QString LexerGenerator::getErrorMessage() const
{
    return m_errorMessage;
}

/**
 * @brief 生成直接匹配法的词法分析器
 * 
 * 生成基于直接匹配方法的词法分析器代码
 * 
 * @param regexItems 正则表达式项列表
 * @param minimizedDFA 最小化DFA（此处未使用）
 * @return QString 生成的词法分析器代码
 */
QString LexerGenerator::generateDirectMatchLexer(const QList<RegexItem> &regexItems, const DFA &minimizedDFA)
{
    QString code;

    // 生成头文件
    code += "#include <iostream>\n";
    code += "#include <fstream>\n";
    code += "#include <string>\n";
    code += "#include <vector>\n";
    code += "#include <cctype>\n";
    code += "using namespace std;\n\n";

    // 生成词法单元结构
    code += "struct Token {\n";
    code += "    int code;\n";
    code += "    string lexeme;\n";
    code += "};\n\n";

    // 生成词法分析函数
    code += "vector<Token> lexicalAnalysis(const string &source) {\n";
    code += "    vector<Token> tokens;\n";
    code += "    size_t pos = 0;\n";
    code += "    size_t sourceLen = source.length();\n\n";

    code += "    while (pos < sourceLen) {\n";
    code += "        // 跳过空白字符\n";
    code += "        if (isspace(static_cast<unsigned char>(source[pos]))) {\n";
    code += "            pos++;";
    code += "            continue;\n";
    code += "        }\n\n";

    code += "        bool matched = false;\n";
    code += "        size_t maxMatchLen = 0;\n";
    code += "        int tokenCode = -1;\n";
    code += "        string matchedLexeme;\n\n";

    // 生成每个正则表达式的匹配逻辑
    for (const RegexItem &item : regexItems) {
        code += QString("        // 匹配 %1\n").arg(item.name);
        code += "        {\n";
        code += "            string currentLexeme;\n";
        code += "            bool currentMatched = false;\n\n";

        // 多单词情况，直接匹配单词列表
        if (item.isMultiWord) {
            int index = 0;
            for (const QString &word : item.wordList) {
                QString escapedWord = word;
                escapedWord.replace("\"", "\\\"");
                escapedWord.replace("\\", "\\\\");

                code += QString("            if (source.compare(pos, %1, \"%2\") == 0) {\n").arg(word.length()).arg(escapedWord);
                code += QString("                currentLexeme = \"%1\";\n").arg(escapedWord);
                code += "                currentMatched = true;\n";
                code += "                if (currentLexeme.length() > maxMatchLen) {\n";
                code += "                    maxMatchLen = currentLexeme.length();\n";
                code += QString("                    tokenCode = %1;\n").arg(item.code + index);
                code += "                    matchedLexeme = currentLexeme;\n";
                code += "                    matched = true;\n";
                code += "                }\n";
                code += "            }\n";
                index++;
            }
        } else {
            // 单单词情况，生成动态匹配逻辑，不依赖硬编码
            code += "            size_t currentPos = pos;\n";
            code += "            size_t matchLen = 0;\n";
            code += "            bool isMatch = false;\n\n";
            
            // 基于DFA的动态匹配逻辑
            code += "            // 基于DFA状态转移的动态匹配\n";
            code += "            int state = " + QString::number(minimizedDFA.startState) + ";\n";
            code += "            int lastAcceptPos = pos;\n";
            code += "            int lastAcceptState = -1;\n\n";
            code += "            // 模拟DFA状态转移\n";
            code += "            for (size_t i = pos; i < sourceLen; ++i) {\n";
            code += "                char c = source[i];\n";
            code += "                bool foundTransition = false;\n\n";
            
            // 为当前状态生成所有可能的转移检查
            for (const auto &transition : minimizedDFA.transitions) {
                // 这里需要为每个转移生成对应的检查代码
                // 由于状态转移是基于输入字符的，我们需要生成对应的条件判断
                QString input = transition.input;
                
                // 保存原始输入
                QString originalInput = transition.input;
                
                // 处理特殊字符
                if (input == "\\n") {
                    input = "'\\n'";
                } else if (input == "\\t") {
                    input = "'\\t'";
                } else if (input == "\\r") {
                    input = "'\\r'";
                } else if (input == "\\0") {
                    input = "'\\0'";
                } else if (input.length() == 1) {
                    input = "'" + input + "'";
                } else {
                    input = "\"" + input + "\"";
                }
                
                code += QString("            // 检查从状态%1通过输入%2到状态%3的转移\n").arg(transition.fromState).arg(input).arg(transition.toState);
                code += QString("            if (state == %1 && (").arg(transition.fromState);
                
                // 根据输入类型生成不同的检查条件
                if (originalInput.length() == 1) {
                    // 单字符匹配
                    code += QString("c == %1").arg(input);
                } else {
                    // 多字符匹配，需要比较子字符串
                    code += QString("source.compare(i, %1, \"%2\") == 0").arg(originalInput.length()).arg(originalInput);
                }
                
                code += "            ) {\n";
                code += QString("                state = %1;\n").arg(transition.toState);
                code += "                lastAcceptPos = i + " + QString::number(originalInput.length()) + ";\n";
                code += "                foundTransition = true;\n";
                
                // 检查是否为接受状态
                if (minimizedDFA.acceptStates.contains(transition.toState)) {
                    code += "                lastAcceptState = " + QString::number(transition.toState) + ";\n";
                }
                
                code += "            }\n";
            }
            
            // 处理无转移情况
            code += "            if (!foundTransition) {\n";
            code += "                break;\n";
            code += "            }\n";
            
            code += "            // 检查当前状态是否为接受状态\n";
            code += "            if (lastAcceptState != -1) {\n";
            code += "                isMatch = true;\n";
            code += "            }\n";
            code += "            }\n\n";
            
            // 设置匹配结果
            code += "            if (isMatch && lastAcceptPos > pos) {\n";
            code += "                matchLen = lastAcceptPos - pos;\n";
            code += "                currentLexeme = source.substr(pos, matchLen);\n";
            code += "                currentMatched = true;\n";
            code += "                if (matchLen > maxMatchLen) {\n";
            code += "                    maxMatchLen = matchLen;\n";
            code += QString("                    tokenCode = %1;\n").arg(item.code);
            code += "                    matchedLexeme = currentLexeme;\n";
            code += "                    matched = true;\n";
            code += "                }\n";
            code += "            }\n";
        }

        code += "        }\n";
    }

    code += "\n";
    code += "        if (matched) {\n";
    code += "            tokens.push_back({tokenCode, matchedLexeme});\n";
    code += "            pos += maxMatchLen;\n";
    code += "        } else {\n";
    code += "            // 跳过无法识别的字符\n";
    code += "            pos++;\n";
    code += "        }\n";
    code += "    }\n\n";
    code += "    return tokens;\n";
    code += "}\n\n";

    // 生成主函数
    code += "int main(int argc, char *argv[]) {\n";
    code += "    string source;\n";
    code += "    ifstream file;\n\n";
    code += "    if (argc > 1) {\n";
    code += "        // 从文件读取源文件\n";
    code += "        file.open(argv[1]);\n";
    code += "        if (file.is_open()) {\n";
    code += "            string line;\n";
    code += "            while (getline(file, line)) {\n";
    code += "                source += line + '\n';\n";
    code += "            }\n";
    code += "            file.close();\n";
    code += "        } else {\n";
    code += "            cerr << \"Error: Could not open file '\" << argv[1] << \'.\" << endl;\n";
    code += "            return 1;\n";
    code += "        }\n";
    code += "    } else {\n";
    code += "        // 从标准输入读取源文件\n";
    code += "        string line;\n";
    code += "        while (getline(cin, line)) {\n";
    code += "            source += line + '\n';\n";
    code += "        }\n";
    code += "    }\n\n";
    code += "    vector<Token> tokens = lexicalAnalysis(source);\n\n";
    code += "    // 输出格式：单词\t编码\n";
    code += "    for (const auto &token : tokens) {\n";
    code += "        cout << token.lexeme << '\t' << token.code << endl;\n";
    code += "    }\n\n";
    code += "    return 0;\n";
    code += "}\n";

    return code;
}

/**
 * @brief 生成状态转移法的词法分析器
 * 
 * 生成基于状态转移表的词法分析器代码
 * 
 * @param regexItems 正则表达式项列表
 * @param minimizedDFA 最小化DFA
 * @return QString 生成的词法分析器代码
 */
QString LexerGenerator::generateStateTransitionLexer(const QList<RegexItem> &regexItems, const DFA &minimizedDFA)
{
    QString code;

    // 生成头文件
    code += "#include <iostream>\n";
    code += "#include <fstream>\n";
    code += "#include <string>\n";
    code += "#include <cstring>\n";
    code += "#include <cctype>\n";
    code += "using namespace std;\n\n";

    // 生成全局变量
    code += "ifstream in;\n";
    code += "string buf;\n";
    code += "int read_cnt = 0;\n\n";

    // 生成跳过空白字符函数
    code += "void skipBlank() {\n";
    code += "\tchar c;\n";
    code += "\twhile ((c = in.get()) != EOF) {\n";
    code += "\t\tif (isspace(static_cast<unsigned char>(c))) {\n";
    code += "\t\t\tread_cnt++;\n";
    code += "\t\t} else {\n";
    code += "\t\t\tin.unget();\n";
    code += "\t\t\tbreak;\n";
    code += "\t\t}\n";
    code += "\t}\n";
    code += "}\n\n";

    // 生成状态转移表
    code += generateStateTransitionTable(minimizedDFA);
    code += "\n";

    // 生成接受状态映射
    code += generateAcceptStatesMap(regexItems, minimizedDFA);
    code += "\n";

    // 生成词法分析函数
    code += "void analyzeToken() {\n";
    code += "\tint state = " + QString::number(minimizedDFA.startState) + ";\n";
    code += "\tint lastAcceptState = -1;\n";
    code += "\tint lastAcceptPos = read_cnt;\n";
    code += "\tchar c;\n";
    code += "\tbuf.clear();\n\n";
    code += "\twhile ((c = in.peek()) != EOF) {\n";
    code += "\t\tint nextState = getNextState(state, c);\n";
    code += "\t\tif (nextState != ERROR_STATE) {\n";
    code += "\t\t\tstate = nextState;\n";
    code += "\t\t\tbuf += c;\n";
    code += "\t\t\tin.get(c);\n";
    code += "\t\t\tread_cnt++;\t\t\t\n";
    code += "\t\t\tif (isAcceptState[state]) {\n";
    code += "\t\t\t\tlastAcceptState = state;\n";
    code += "\t\t\t\tlastAcceptPos = read_cnt;\n";
    code += "\t\t\t}\n";
    code += "\t\t}\n";
    code += "\t\telse {\n";
    code += "\t\t\tbreak;\n";
    code += "\t\t}\n";
    code += "\t}\n\n";
    code += "\tif (lastAcceptState != -1) {\n";
    code += "\t\t// 回退到最后一个接受状态\n";
    code += "\t\tint backoff = read_cnt - lastAcceptPos;\n";
    code += "\t\tin.seekg(-backoff, ios::cur);\n";
    code += "\t\tread_cnt = lastAcceptPos;\n";
    code += "\t\tbuf.resize(buf.size() - backoff);\t\t\n";
    code += "\t\t\n";
    code += "\t\t// 输出格式：单词\t编码\n";
    code += "\t\tint tokenCode = acceptTokens[lastAcceptState];\n";

    code += "\t\tcout << buf << '\t' << tokenCode << endl;\n";
    code += "\t}\n";
    code += "\telse {\n";
    code += "\t\t// 跳过无法识别的字符\n";
    code += "\t\tbuf.clear();\n";
    code += "\t\tin.get();\n";
    code += "\t\tread_cnt++;\n";
    code += "\t}\n";
    code += "}\n\n";

    // 生成主函数
    code += "int main(int argc, char *argv[]) {\n";

    code += "\tif (argc < 2) {\n";
    code += "\t\tcerr << \"Usage: \" << argv[0] << \" <source_file>\" << endl;\n";
    code += "\t\treturn 1;\n";
    code += "\t}\n\n";
    code += "\t// 打开输入文件\n";
    code += "\tin.open(argv[1], ios::in);\n";
    code += "\tif (!in.is_open()) {\n";
    code += "\t\tcerr << \"Error: Could not open file '\" << argv[1] << \"'.\" << endl;\n";
    code += "\t\treturn 1;\n";
    code += "\t}\n\n";
    code += "\t// 跳过初始空白字符\n";
    code += "\tskipBlank();\n\n";
    code += "\t// 主循环\n";
    code += "\twhile (in.peek() != EOF) {\n";
    code += "\t\tanalyzeToken();\n";
    code += "\t\tskipBlank();\n";
    code += "\t}\n\n";
    code += "\t// 关闭文件\n";
    code += "\tin.close();\n";
    code += "\treturn 0;\n";
    code += "}\n";

    return code;
}

/**
 * @brief 生成状态转移表
 * 
 * 生成DFA的状态转移表代码（使用嵌套switch-case结构）
 * 
 * @param minimizedDFA 最小化DFA
 * @return QString 生成的状态转移表代码
 */
QString LexerGenerator::generateStateTransitionTable(const DFA &minimizedDFA)
{
    QString code;

    code += "// DFA状态转移表（嵌套switch-case结构）\n";
    code += "const int NUM_STATES = " + QString::number(minimizedDFA.states.size()) + ";\n";
    code += "const int ERROR_STATE = -1;\n\n";

    // 字符类型辅助函数
    code += "// 字符类型辅助函数\n";
    code += "bool IsDigit(char c) {\n";
    code += "    return c >= '0' && c <= '9';\n";
    code += "}\n\n";
    code += "bool IsAlpha(char c) {\n";
    code += "    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');\n";
    code += "}\n\n";
    code += "bool IsAlnum(char c) {\n";
    code += "    return IsAlpha(c) || IsDigit(c);\n";
    code += "}\n\n";
    code += "bool IsLower(char c) {\n";
    code += "    return c >= 'a' && c <= 'z';\n";
    code += "}\n\n";
    code += "bool IsUpper(char c) {\n";
    code += "    return c >= 'A' && c <= 'Z';\n";
    code += "}\n\n";

    // 生成状态转移函数声明
    code += "// 状态转移函数\n";
    code += "int getNextState(int currentState, char input) {\n";
    code += "    switch(currentState) {\n";

    // 为每个状态生成case分支
    for (int state = 0; state < minimizedDFA.states.size(); state++) {
        code += QString("        case %1: {\n").arg(state);
        code += "            switch(input) {\n";

        // 收集当前状态的所有转移
        QMap<QString, int> transitionsFromState;
        for (const auto &transition : minimizedDFA.transitions) {
            if (transition.fromState == state) {
                transitionsFromState[transition.input] = transition.toState;
            }
        }

        // 处理特殊字符
        if (transitionsFromState.contains("\n")) {
            code += "                case '\n':\n";
            code += QString("                    return %1;\n").arg(transitionsFromState["\n"]);
            transitionsFromState.remove("\n");
        }
        if (transitionsFromState.contains("\t")) {
            code += "                case '\t':\n";
            code += QString("                    return %1;\n").arg(transitionsFromState["\t"]);
            transitionsFromState.remove("\t");
        }
        if (transitionsFromState.contains("\r")) {
            code += "                case '\r':\n";
            code += QString("                    return %1;\n").arg(transitionsFromState["\r"]);
            transitionsFromState.remove("\r");
        }
        if (transitionsFromState.contains("\0")) {
            code += "                case '\0':\n";
            code += QString("                    return %1;\n").arg(transitionsFromState["\0"]);
            transitionsFromState.remove("\0");
        }

        // 移除硬编码的数字和字母范围检查，直接处理所有实际的字符转移

        // 处理其他单个字符
        QMap<QString, int>::const_iterator it;
        for (it = transitionsFromState.constBegin(); it != transitionsFromState.constEnd(); ++it) {
            QString input = it.key();
            int toState = it.value();
            
            // 处理转义字符
            if (input == "\\\\") {
                input = "\\\\\\\\";
            } else if (input == "\\\'") {
                input = "\\\\'";
            } else if (input == "\"" || input == "\\" || input == "'" || input == "%" || input == "&") {
                // 处理其他需要转义的字符
                input = "\\" + input;
            }
            
            code += QString("                case '%1':\n").arg(input);
            code += QString("                    return %1;\n").arg(toState);
        }

        // 处理默认情况
        code += "                default:\n";
        code += "                    return ERROR_STATE;\n";
        code += "            }\n";
        code += "        }\n";
    }

    code += "        default:\n";
    code += "            return ERROR_STATE;\n";
    code += "    }\n";
    code += "}\n\n";

    return code;
}

/**
 * @brief 生成接受状态映射
 * 
 * 生成接受状态到token代码的映射代码
 * 
 * @param regexItems 正则表达式项列表
 * @param minimizedDFA 最小化DFA
 * @return QString 生成的接受状态映射代码
 */
QString LexerGenerator::generateAcceptStatesMap(const QList<RegexItem> &regexItems, const DFA &minimizedDFA)
{
    QString code;

    int numStates = minimizedDFA.states.size();
    QVector<bool> isAccept(numStates, false);
    QVector<int> tokens(numStates, -1);

    // 1. 标记所有接受状态
    for (const int &state : minimizedDFA.acceptStates) {
        if (state >= 0 && state < numStates) {
            isAccept[state] = true;
        }
    }

    // 2. 为每个接受状态分配正确的tokenCode
    // 初始化tokens数组为-1
    for (int i = 0; i < numStates; i++) {
        tokens[i] = -1;
    }

    // 填充接受状态和对应的token代码
    for (const auto &state : minimizedDFA.acceptStates) {
        if (state >= 0 && state < numStates) {
            // 只处理存在映射的状态
            if (minimizedDFA.acceptStateToRegexIndex.contains(state)) {
                // 从DFA的映射中获取正则表达式索引
                int regexIndex = minimizedDFA.acceptStateToRegexIndex[state];

                // 确保索引有效
                if (regexIndex >= 0 && regexIndex < regexItems.size()) {
                    // 获取对应的正则表达式项
                    const RegexItem &item = regexItems[regexIndex];
                    
                    // 为接受状态分配token代码
                    tokens[state] = item.code;
                    
                    // 对于多单词项，处理编码递增
                    if (item.isMultiWord) {
                        // 这里可以添加更复杂的多单词编码处理逻辑
                    }
                }
            }
            // 对于没有映射的接受态，保持-1
        }
    }

    // 对于没有映射的接受态，保持-1，由tokenCodeMap覆盖
    // 非多单词接受态应该已经通过映射获得了正确的tokenCode
    // 多单词接受态由tokenCodeMap处理，不需要额外分配

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
        code += QString::number(tokens[i]);
    }
    code += "};\n";

    return code;
}


/**
 * @brief 生成Token映射
 * 
 * 生成Token映射文件内容
 * 
 * @param regexItems 正则表达式项列表
 * @return QString 生成的Token映射内容
 */
QString LexerGenerator::generateTokenMap(const QList<RegexItem> &regexItems)
{
    QString mapContent;
    
    // 添加文件头注释
    mapContent += "// Token映射文件，生成自词法分析器生成器\n";
    mapContent += "// 格式：编码=token名称 或 编码=token名称|single\n";
    mapContent += "// |single 表示该token是单编码类型，需要词素\n\n";

    // 为每个正则表达式项生成映射
    for (const RegexItem &item : regexItems) {
        if (item.isMultiWord) {
            // 多单词情况（关键字、符号等）
            int currentCode = item.code;

            // 使用QMap来确保同一单词的不同大小写形式映射到同一个编码
            QMap<QString, int> lowercaseToCodeMap;

            for (const QString &word : item.wordList) {
                QString lowercaseWord = word.toLower();
                if (!lowercaseToCodeMap.contains(lowercaseWord)) {
                    lowercaseToCodeMap[lowercaseWord] = currentCode++;
                }
            }

            // 生成映射条目
            for (auto it = lowercaseToCodeMap.constBegin(); it != lowercaseToCodeMap.constEnd(); ++it) {
                // 正确处理转义字符
                QString tokenValue = it.key();
                // 转义需要特殊处理的字符
                tokenValue.replace("\\", "\\\\");
                tokenValue.replace("\"", "\\\"");
                tokenValue.replace("'", "\\'");
                mapContent += QString("%1=%2\n").arg(it.value()).arg(tokenValue);
            }
        } else {
            // 单单词情况（标识符、数字等）
            mapContent += QString("%1=%2|single\n").arg(item.code).arg(item.name);
        }
    }

    return mapContent;
}

/**
 * @brief 保存Token映射到文件
 * 
 * 将生成的Token映射保存到指定文件
 * 
 * @param regexItems 正则表达式项列表
 * @param outputPath 输出文件路径
 * @return bool 保存成功返回true，失败返回false
 */
bool LexerGenerator::saveTokenMap(const QList<RegexItem> &regexItems, const QString &outputPath)
{
    QString mapContent = generateTokenMap(regexItems);

    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_errorMessage = QString("无法打开文件 %1 进行写入").arg(outputPath);
        return false;
    }

    QTextStream out(&file);
    out << mapContent;
    file.close();

    return true;
}
