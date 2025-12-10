#include "task1/lexergenerator.h"
#include <QDebug>
#include <QMap>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#define ERROR_STATE -1

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
    code += "#include <vector>\n";
    code += "#include <cctype>\n";
    code += "#include <map>\n";
    code += "#include <algorithm>\n";
    code += "using namespace std;\n\n";

    // 生成词法单元结构
    code += "struct Token {\n";
    code += "    int code;\n";
    code += "    string lexeme;\n";
    code += "};\n\n";

    // 生成单词编码映射
    code += generateTokenCodeMap(regexItems);

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
            // 单单词情况，实现标识符、数字等匹配逻辑
            if (item.name == "_identifier") {
                // 标识符匹配：字母或下划线开头，后跟字母、数字或下划线
                code += "            if ((isalpha(static_cast<unsigned char>(source[pos])) || source[pos] == '_')) {\n";
                code += "                size_t matchLen = 0;\n";
                code += "                size_t currentPos = pos;\n";
                code += "                while (currentPos < sourceLen && (isalnum(static_cast<unsigned char>(source[currentPos])) || source[currentPos] == '_')) {\n";
                code += "                    currentLexeme += source[currentPos++];\n";
                code += "                    matchLen++;\n";
                code += "                }\n";
                code += "                currentMatched = matchLen > 0;\n";
            } else if (item.name == "_number") {
                // 数字匹配：整数或浮点数
                code += "            if (isdigit(static_cast<unsigned char>(source[pos]))) {\n";
                code += "                size_t matchLen = 0;\n";
                code += "                size_t currentPos = pos;\n";
                code += "                while (currentPos < sourceLen && isdigit(static_cast<unsigned char>(source[currentPos]))) {\n";
                code += "                    currentLexeme += source[currentPos++];\n";
                code += "                    matchLen++;";
                code += "                }\n";
                code += "                // 检查小数部分\n";
                code += "                if (currentPos < sourceLen && source[currentPos] == '.') {\n";
                code += "                    currentLexeme += source[currentPos++];\n";
                code += "                    matchLen++;";
                code += "                    while (currentPos < sourceLen && isdigit(static_cast<unsigned char>(source[currentPos]))) {\n";
                code += "                        currentLexeme += source[currentPos++];\n";
                code += "                        matchLen++;";
                code += "                    }\n";
                code += "                }\n";
                code += "                currentMatched = matchLen > 0;\n";
            } else if (item.name == "_string") {
                // 字符串匹配：双引号括起来的内容
                code += "            if (source[pos] == '\"') {\n";
                code += "                size_t matchLen = 1;\n";
                code += "                size_t currentPos = pos + 1;\n";
                code += "                currentLexeme = \"\"\";\n";
                code += "                while (currentPos < sourceLen && source[currentPos] != '\"') {\n";
                code += "                    // 处理转义字符\n";
                code += "                    if (source[currentPos] == '\\' && currentPos + 1 < sourceLen) {\n";
                code += "                        currentLexeme += source[currentPos++];\n";
                code += "                        matchLen++;";
                code += "                    }\n";
                code += "                    currentLexeme += source[currentPos++];\n";
                code += "                    matchLen++;";
                code += "                }\n";
                code += "                if (currentPos < sourceLen) {\n";
                code += "                    currentLexeme += '\"';\n";
                code += "                    matchLen++;";
                code += "                    currentMatched = true;\n";
                code += "                }\n";
                code += "            }\n";
            } else {
                // 其他单字符匹配
                code += "            // 单字符匹配\n";
                code += "            currentLexeme = source[pos];\n";
                code += "            currentMatched = true;\n";
            }

            code += "            if (currentMatched) {\n";
            code += "                if (currentLexeme.length() > maxMatchLen) {\n";
            code += "                    maxMatchLen = currentLexeme.length();\n";
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
    code += "            // 使用预定义的单词编码或动态分配\n";
    code += "            int finalTokenCode = tokenCode;\n";
    code += "            // 确保编码不为零，使用动态分配的编码\n";
    code += "            if (finalTokenCode == 0) {\n";
    code += "                finalTokenCode = nextTokenCode++;\n";
    code += "            }\n";
    code += "            // 对于多单词类型，使用tokenCodeMap中的编码，实现大小写不敏感处理\n";
    code += "            string lowercaseLexeme = matchedLexeme;\n";
    code += "            transform(lowercaseLexeme.begin(), lowercaseLexeme.end(), lowercaseLexeme.begin(), ::tolower);\n";
    code += "            if (tokenCodeMap.find(lowercaseLexeme) != tokenCodeMap.end()) {\n";
    code += "                finalTokenCode = tokenCodeMap[lowercaseLexeme];\n";
    code += "            }\n";
    code += "            tokens.push_back({finalTokenCode, matchedLexeme});\n";
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
    code += "    // 初始化单词编码映射表\n";
    code += "    initializeTokenCodeMap();\n\n";
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
    code += "            cerr << \"Error: Could not open file '\" << argv[1] << \".\" << endl;\n";
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

QString LexerGenerator::generateStateTransitionLexer(const QList<RegexItem> &regexItems, const DFA &minimizedDFA)
{
    QString code;

    // 生成头文件
    code += "#include <iostream>\n";
    code += "#include <fstream>\n";
    code += "#include <string>\n";
    code += "#include <map>\n";
    code += "#include <algorithm>\n";
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

    // 生成单词编码映射表
    code += generateTokenCodeMap(regexItems);
    code += "\n";

    // 生成词法分析函数
    code += "void analyzeToken() {\n";
    code += "\tint state = " + QString::number(minimizedDFA.startState) + ";\n";
    code += "\tint lastAcceptState = -1;\n";
    code += "\tint lastAcceptPos = read_cnt;\n";
    code += "\tchar c;\n";
    code += "\tbuf.clear();\n\n";
    code += "\twhile ((c = in.peek()) != EOF) {\n";
    code += "\t\tint input = static_cast<unsigned char>(c);\n";
    code += "\t\tint nextState = transitions[state][input];\t\t\n";
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
    code += "\t\t// 检查tokenCodeMap中是否有对应的条目，实现大小写不敏感处理\n";
    code += "\t\tstring lowercaseBuf = buf;\n";
    code += "\t\ttransform(lowercaseBuf.begin(), lowercaseBuf.end(), lowercaseBuf.begin(), ::tolower);\n";
    code += "\t\tif (tokenCodeMap.find(lowercaseBuf) != tokenCodeMap.end()) {\n";
    code += "\t\t\ttokenCode = tokenCodeMap[lowercaseBuf];\n";
    code += "\t\t}\n";
    code += "\t\tcout << buf << '\\t' << tokenCode << endl;\n";
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
    code += "\t// 初始化单词编码映射表\n";
    code += "\tinitializeTokenCodeMap();\n\n";
    code += "\tif (argc < 2) {\n";
    code += "\t\tcerr << \"Usage: \" << argv[0] << \" <source_file>\" << endl;\n";
    code += "\t\treturn 1;\n";
    code += "\t}\n\n";
    code += "\t// 打开输入文件\n";
    code += "\tin.open(argv[1], ios::in);\n";
    code += "\tif (!in.is_open()) {\n";
    code += "\t\tcerr << \"Error: Could not open file '\" << argv[1] << \".\" << endl;\n";
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

QString LexerGenerator::generateStateTransitionTable(const DFA &minimizedDFA)
{
    QString code;

    code += "// DFA状态转移表\n";
    code += "const int NUM_STATES = " + QString::number(minimizedDFA.states.size()) + ";\n";
    code += "const int ERROR_STATE = -1;\n\n";

    // 构建完整的状态转移表
    QVector<QVector<int>> transitionTable(minimizedDFA.states.size(), QVector<int>(256, ERROR_STATE));

    // 填充状态转移表
    for (const auto &transition : minimizedDFA.transitions) {
        if (transition.input.size() == 1) {
            char c = transition.input.at(0).toLatin1();
            int asciiValue = static_cast<int>(static_cast<unsigned char>(c));
            transitionTable[transition.fromState][asciiValue] = transition.toState;
        } else {
            // 处理特殊字符
            if (transition.input == "\\n") {
                transitionTable[transition.fromState][10] = transition.toState; // 换行符
            } else if (transition.input == "\\t") {
                transitionTable[transition.fromState][9] = transition.toState; // 制表符
            } else if (transition.input == "\\r") {
                transitionTable[transition.fromState][13] = transition.toState; // 回车符
            } else if (transition.input == "\\0") {
                transitionTable[transition.fromState][0] = transition.toState; // 空字符
            }
        }
    }

    // 生成状态转移表代码
    code += "// 状态转移表: transitions[当前状态][输入字符ASCII值] = 下一个状态\n";
    code += "int transitions[NUM_STATES][256] = {\n";

    for (int i = 0; i < transitionTable.size(); i++) {
        code += "    {";
        for (int j = 0; j < transitionTable[i].size(); j++) {
            if (j > 0) {
                code += ", ";
            }
            code += QString::number(transitionTable[i][j]);
        }
        code += "}";
        if (i < transitionTable.size() - 1) {
            code += ",\n";
        }
    }

    code += "\n};\n";

    return code;
}

QString LexerGenerator::generateAcceptStatesMap(const QList<RegexItem> &regexItems, const DFA &minimizedDFA)
{
    QString code;

    code += "// 接受状态映射\n";

    // 构建接受状态映射
    int numStates = minimizedDFA.states.size();
    QVector<bool> isAccept(numStates, false);
    QVector<int> tokens(numStates, -1);

    // 1. 遍历所有接受状态，将它们标记为接受状态
    for (const auto &state : minimizedDFA.acceptStates) {
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
                    // 只对单单词类型设置具体编码，多单词类型由tokenCodeMap处理
                    if (!regexItems[regexIndex].isMultiWord) {
                        tokens[state] = regexItems[regexIndex].code;
                    }
                    // 多单词类型保持-1，由tokenCodeMap覆盖
                }
            }
            // 对于没有映射的接受态，保持-1，由tokenCodeMap覆盖
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

QString LexerGenerator::generateTokenCodeMap(const QList<RegexItem> &regexItems)
{
    QString code;

    code += "// 单词编码映射\n";
    code += "map<string, int> tokenCodeMap;\n";
    code += "int nextTokenCode = 1;\n\n";

    // 创建初始化函数，将所有赋值操作放在函数内部
    code += "// 初始化单词编码映射表\n";
    code += "void initializeTokenCodeMap() {\n";

    // 为每个正则表达式项生成映射
    for (const RegexItem &item : regexItems) {
        if (item.isMultiWord) {
            // 多单词情况，实现大小写不敏感处理
            int currentCode = item.code;

            // 使用QMap来临时存储小写单词到编码的映射，确保同一单词的不同大小写形式映射到同一个编码
            QMap<QString, int> lowercaseToCodeMap;

            // 第一步：收集所有单词的小写形式，确保同一单词的不同大小写形式映射到同一个编码
            for (const QString &word : item.wordList) {
                QString lowercaseWord = word.toLower();
                if (!lowercaseToCodeMap.contains(lowercaseWord)) {
                    lowercaseToCodeMap[lowercaseWord] = currentCode++;
                }
            }

            // 第二步：为每个原始单词生成映射，使用其小写形式对应的编码
            // 这样可以确保同一关键字的所有大小写变体都能在映射表中完整展示
            for (const QString &word : item.wordList) {
                QString lowercaseWord = word.toLower();
                int codeValue = lowercaseToCodeMap[lowercaseWord];

                // 只对引号进行转义，不对其他字符进行过度转义
                QString escapedWord = word;

                // 移除多余的转义字符，只保留引号的转义
                // 首先移除所有转义字符
                escapedWord.replace("\\", "");

                // 然后只对引号进行转义
                escapedWord.replace("\"", "\\\"");

                // 生成tokenCodeMap赋值语句
                code += QString("    tokenCodeMap[\"%1\"] = %2;\n").arg(escapedWord).arg(codeValue);
            }
        } else {
            // 单单词情况，不需要在tokenCodeMap中注册，直接使用item.code
            // 这些类型（如标识符、数字）将通过状态转移或直接匹配来识别
        }
    }

    code += "}\n\n";

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
                // 修复转义字符，只保留必要的转义
                QString tokenValue = it.key();
                // 去除多余的转义字符
                tokenValue.replace("\\", "");
                mapContent += QString("%1=%2\n").arg(it.value()).arg(tokenValue);
            }
        } else {
            // 单单词情况（标识符、数字等）
            QString tokenName = item.name.mid(1); // 移除下划线前缀
            // 提取纯名称，去除后面的编号
            QString pureTokenName;
            for (QChar c : tokenName) {
                if (!c.isDigit()) {
                    pureTokenName.append(c);
                } else {
                    break;
                }
            }
            // 为单编码token添加|single标记
            mapContent += QString("%1=%2|single\n").arg(item.code).arg(pureTokenName);
        }
    }

    return mapContent;
}

bool LexerGenerator::saveTokenMap(const QList<RegexItem> &regexItems, const QString &outputPath)
{
    QString mapContent = generateTokenMap(regexItems);
    QFile file(outputPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << mapContent;
        file.close();
        return true;
    }
    return false;
}
