#include "task1/lexergenerator.h"
#include <QDebug>
#include <QMap>
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
    code += "int main(int argc, char *argv[]) {\n";
    code += "    std::string source;\n";
    code += "    std::ifstream file;\n";
    code += "    \n";
    code += "    if (argc > 1) {\n";
    code += "        // 从文件读取源文件\n";
    code += "        file.open(argv[1]);\n";
    code += "        if (file.is_open()) {\n";
    code += "            std::string line;\n";
    code += "            while (std::getline(file, line)) {\n";
    code += "                source += line + '\\n';\n";
    code += "            }\n";
    code += "            file.close();\n";
    code += "        } else {\n";
    code += "            std::cerr << \"Error: Could not open file '\" << argv[1] << \"'.\\n\";\n";
    code += "            return 1;\n";
    code += "        }\n";
    code += "    } else {\n";
    code += "        // 从标准输入读取源文件\n";
    code += "        std::string line;\n";
    code += "        while (std::getline(std::cin, line)) {\n";
    code += "            source += line + '\\n';\n";
    code += "        }\n";
    code += "    }\n";
    code += "    \n";
    code += "    std::vector<Token> tokens = lexicalAnalysis(source);\n";
    code += "    \n";
    code += "    // 输出格式：行号\t单词\t编码，与parseLexicalResult函数期望的格式匹配\n";
    code += "    for (const auto &token : tokens) {\n";
    code += "        std::cout << token.line << '\t' << token.lexeme << '\t' << token.code << '\\n';\n";
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
    code += "#include <cstring>\n";
    code += "#include <cctype>\n";
    code += "#include <map>\n";
    code += "using namespace std;\n\n";
    
    // 生成全局变量
    code += "ifstream in;\n";         // 源代码存储的位置
    code += "ofstream out(\"sample.lex\", ios::out | ios::trunc);\n";      // 单词编码保存的位置
    code += "string buf, buf_err, buf_suc;\n";              // 存储单词字符串
    code += "string token, token_suc;\n";                   // 存储单词类型
    code += "int read_cnt;\n";                            // 当前读到文件的位置
    code += "int line = 1;\n";                            // 当前行号
    code += "map<string, int> mp;\n";                       // 存储单词编码的映射
    code += "int idx = 1;\n\n";                                   // 存储单词编码当前位置
    
    // 生成跳过空白字符函数
    code += "void skipBlank() {\n";
    code += "\tchar c;\n";
    code += "\twhile (in.get(c)) {\n";
    code += "\t\tread_cnt++;\n";
    code += "\t\tif (c == '\\n') {\n";
    code += "\t\t\tread_cnt++;\n";
    code += "\t\t\tline++;\n";
    code += "\t\t}\n";
    code += "\t\tif (!isspace(c)) {\n";
    code += "\t\t\tread_cnt--;\n";
    code += "\t\t\tin.unget();\n";
    code += "\t\t\tbreak;\n";
    code += "\t\t}\n";
    code += "\t}\n";
    code += "}\n\n";
    
    // 生成字符判断函数
    code += "bool IsDigit(char c) {\n";
    code += "\tif (c >= '0' && c <= '9') return 1;\n";
    code += "\telse return 0;\n";
    code += "}\n\n";
    
    code += "bool IsPositiveDigit(char c) {\n";
    code += "\tif (c >= '1' && c <= '9') return 1;\n";
    code += "\telse return 0;\n";
    code += "}\n\n";
    
    code += "bool IsAlpha(char c) {\n";
    code += "\tif ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1;\n";
    code += "\telse return 0;\n";
    code += "}\n\n";
    
    code += "bool IsLower(char c) {\n";
    code += "\tif (c >= 'a' && c <= 'z') return 1;\n";
    code += "\telse return 0;\n";
    code += "}\n\n";
    
    code += "bool IsUpper(char c) {\n";
    code += "\tif (c >= 'A' && c <= 'Z') return 1;\n";
    code += "\telse return 0;\n";
    code += "}\n\n";
    
    // 构建状态转移图，便于生成代码
    QMap<int, QMap<QString, int>> stateTransitions;
    
    // 填充状态转移图
    for (const auto &transition : minimizedDFA.transitions) {
        QString input = transition.input;
        if (input == "\\n") input = "\n"; // 处理换行符
        else if (input == "\\t") input = "\t"; // 处理制表符
        else if (input == "\\r") input = "\r"; // 处理回车符
        else if (input == "\\0") input = "\0"; // 处理空字符
        
        stateTransitions[transition.fromState][input] = transition.toState;
    }
    
    // 生成check函数 - 根据test/111.txt的实现，生成单个check_token函数
    // 由于当前只处理一个DFA，我们生成check_token函数
    QString dfaKey = "token"; // 默认token名称
    
    // 从regexItems中获取实际的token名称
    if (!regexItems.isEmpty()) {
        dfaKey = regexItems.first().name.mid(1); // 移除前缀下划线
    }
    
    code += "bool check_" + dfaKey + "() {\n";
    code += "\tint state = " + QString::number(minimizedDFA.startState) + ";\n";
    code += "\tchar c;\n";
    code += "\twhile ((c = in.peek()) != EOF) {\n";
    code += "\t\tswitch(state) {\n";
    
    // 遍历所有状态，生成对应的case分支
    for (int i = 0; i < minimizedDFA.states.size(); i++) {
        code += "\t\tcase " + QString::number(i) + ":\n";
        
        // 调用IsDigit
        bool IsDigitFlag = true;
        if (stateTransitions[i].keys().contains(QString::number(0))) {
            int tmpState = stateTransitions[i][QString::number(0)];
            for (int j = 1; j <= 9; j++) {
                if (!stateTransitions[i].keys().contains(QString::number(j)) || tmpState != stateTransitions[i][QString::number(j)]) {
                    IsDigitFlag = false;
                    break;
                }
            }
            if (IsDigitFlag) {
                code += "\t\t\tif (IsDigit(c)) {\n";
                code += "\t\t\t\tstate = " + QString::number(stateTransitions[i][QString::number(0)]) + ";\n";
                code += "\t\t\t\tbuf += c;\n";
                code += "\t\t\t\tin.get(c);\n";
                code += "\t\t\t\tbreak;\n";
                code += "\t\t\t}\n";
            }
        }
        
        // 调用IsPositiveDigit
        bool IsPositiveDigitFlag = true;
        if (!IsDigitFlag && stateTransitions[i].keys().contains(QString::number(1))) {
            int tmpState = stateTransitions[i][QString::number(1)];
            for (int j = 2; j <= 9; j++) {
                if (!stateTransitions[i].keys().contains(QString::number(j)) || tmpState != stateTransitions[i][QString::number(j)]) {
                    IsPositiveDigitFlag = false;
                    break;
                }
            }
            if (IsPositiveDigitFlag) {
                code += "\t\t\tif (IsPositiveDigit(c)) {\n";
                code += "\t\t\t\tstate = " + QString::number(stateTransitions[i][QString::number(1)]) + ";\n";
                code += "\t\t\t\tbuf += c;\n";
                code += "\t\t\t\tin.get(c);\n";
                code += "\t\t\t\tbreak;\n";
                code += "\t\t\t}\n";
            }
        }
        
        // 调用IsAlpha
        bool IsAlphaFlag = true;
        if (stateTransitions[i].keys().contains(QString('a'))) {
            int tmpState = stateTransitions[i][QString('a')];
            for (char j = 'a'; j <= 'z'; j++) {
                if (!stateTransitions[i].keys().contains(QString(j)) || tmpState != stateTransitions[i][QString(j)]) {
                    IsAlphaFlag = false;
                    break;
                }
            }
            for (char j = 'A'; j <= 'Z'; j++) {
                if (!stateTransitions[i].keys().contains(QString(j)) || tmpState != stateTransitions[i][QString(j)]) {
                    IsAlphaFlag = false;
                    break;
                }
            }
            if (IsAlphaFlag) {
                code += "\t\t\tif (IsAlpha(c)) {\n";
                code += "\t\t\t\tstate = " + QString::number(stateTransitions[i][QString('a')]) + ";\n";
                code += "\t\t\t\tbuf += c;\n";
                code += "\t\t\t\tin.get(c);\n";
                code += "\t\t\t\tbreak;\n";
                code += "\t\t\t}\n";
            }
        }
        
        // 调用IsLower
        bool IsLowerFlag = true;
        if (!IsAlphaFlag && stateTransitions[i].keys().contains(QString('a'))) {
            int tmpState = stateTransitions[i][QString('a')];
            for (char j = 'a'; j <= 'z'; j++) {
                if (!stateTransitions[i].keys().contains(QString(j)) || tmpState != stateTransitions[i][QString(j)]) {
                    IsLowerFlag = false;
                    break;
                }
            }
            if (IsLowerFlag) {
                code += "\t\t\tif (IsLower(c)) {\n";
                code += "\t\t\t\tstate = " + QString::number(stateTransitions[i][QString('a')]) + ";\n";
                code += "\t\t\t\tbuf += c;\n";
                code += "\t\t\t\tin.get(c);\n";
                code += "\t\t\t\tbreak;\n";
                code += "\t\t\t}\n";
            }
        }
        
        // 调用IsUpper
        bool IsUpperFlag = true;
        if (!IsAlphaFlag && stateTransitions[i].keys().contains(QString('A'))) {
            int tmpState = stateTransitions[i][QString('A')];
            for (char j = 'A'; j <= 'Z'; j++) {
                if (!stateTransitions[i].keys().contains(QString(j)) || tmpState != stateTransitions[i][QString(j)]) {
                    IsUpperFlag = false;
                    break;
                }
            }
            if (IsUpperFlag) {
                code += "\t\t\tif (IsUpper(c)) {\n";
                code += "\t\t\t\tstate = " + QString::number(stateTransitions[i][QString('A')]) + ";\n";
                code += "\t\t\t\tbuf += c;\n";
                code += "\t\t\t\tin.get(c);\n";
                code += "\t\t\t\tbreak;\n";
                code += "\t\t\t}\n";
            }
        }
        
        // 生成具体字符的case分支
        code += "\t\t\tswitch(c) {\n";
        
        for (const auto &changeItem : stateTransitions[i].keys()) {
            // 跳过已经通过字符类型判断处理的字符
            if (IsDigitFlag && changeItem[0] >= '0' && changeItem[0] <= '9') continue;
            if (IsPositiveDigitFlag && changeItem[0] >= '1' && changeItem[0] <= '9') continue;
            if (IsAlphaFlag && ((changeItem[0] >= 'a' && changeItem[0] <= 'z') || (changeItem[0] >= 'A' && changeItem[0] <= 'Z'))) continue;
            if (IsLowerFlag && changeItem[0] >= 'a' && changeItem[0] <= 'z') continue;
            if (IsUpperFlag && changeItem[0] >= 'A' && changeItem[0] <= 'Z') continue;
            
            code += "\t\t\tcase '" + changeItem + "':\n";
            code += "\t\t\t\tstate = " + QString::number(stateTransitions[i][changeItem]) + ";\n";
            code += "\t\t\t\tbuf += c;\n";
            code += "\t\t\t\tin.get(c);\n";
            code += "\t\t\t\tbreak;\n";
        }
        
        // 生成default分支
        code += "\t\t\tdefault:\n";
        
        // 判断当前是否为终态
        if (minimizedDFA.acceptStates.contains(i)) {
            code += "\t\t\t\ttoken = \"" + dfaKey + "\";\n";
            code += "\t\t\t\treturn true;\n";
        } else {
            code += "\t\t\t\treturn false;\n";
        }
        
        code += "\t\t\t}\n";
        code += "\t\t\tbreak;\n";
    }
    
    code += "\t\t}\n";
    code += "\t}\n";
    
    // 判断state是否为终态
    code += "\tif (";
    int cnt = 0;
    for (int i : minimizedDFA.acceptStates) {
        code += "state == " + QString::number(i);
        cnt++;
        if (cnt != minimizedDFA.acceptStates.size()) code += "||";
    }
    code += ") {\n";
    code += "\t\ttoken = \"" + dfaKey + "\";\n";
    code += "\t\treturn true;\n";
    code += "\t}\n";
    
    code += "\telse return false;\n";
    code += "}\n\n";
    
    // 生成词法分析主函数 - 基于test/111.txt的实现
    code += "int main(int argc, char *argv[]) {\n";
    code += "\tbool flag;\n";
    code += "\tchar c;\n";
    
    // 处理命令行参数
    code += "\tif (argc < 2) {\n";
    code += "\t\tcerr << \"Usage: \" << argv[0] << \" <source_file>\" << endl;\n";
    code += "\t\treturn 1;\n";
    code += "\t}\n";
    
    // 打开输入文件
    code += "\tin.open(argv[1], ios::in);\n";
    code += "\tif (!in.is_open()) {\n";
    code += "\t\tcerr << \"Error: Could not open file '\" << argv[1] << \"'.\" << endl;\n";
    code += "\t\treturn 1;\n";
    code += "\t}\n";
    
    code += "\tread_cnt = 0;\n";
    code += "\tskipBlank();\n";
    code += "\twhile ((c = in.peek()) != EOF) {\n";
    
    code += "\t\ttoken_suc.clear();\n";
    code += "\t\tbuf_suc.clear();\n";
    
    // 调用check函数 - 这里我们只有一个check_token函数
    code += "\t\tif (!check_" + dfaKey + "()) buf_err = buf;\n";
    code += "\t\telse if (buf.size() > buf_suc.size()) {\n";
    code += "\t\t\tbuf_suc = buf;\n";
    code += "\t\t\ttoken_suc = token;\n";
    code += "\t\t}\n";
    
    code += "\t\tbuf.clear();\n";
    code += "\t\tin.seekg(read_cnt, ios::beg);\n";
    
    // 判断成功或失败
    code += "\t\tif (buf_suc.empty()) {\n";
    code += "\t\t\tout << buf_err << \" UNKNOWN\" << endl;\n";
    code += "\t\t\tcout << line << \"\\t\" << buf_err << \"\\t\" << -1 << \"\\n\";\n";
    code += "\t\t\texit(1);\n";
    code += "\t\t}\n";
    
    // 获取当前单词的编码
    code += "\t\tint currentCode;\n";
    code += "\t\tif (token_suc.empty() || !isupper(token_suc[0])) {\n";
    code += "\t\t\tif (!mp.count(token_suc)) mp[token_suc] = idx++;\n";
    code += "\t\t\tcurrentCode = mp[token_suc];\n";
    code += "\t\t\tout << currentCode << ' ' << buf_suc << ' ';\n";
    code += "\t\t} else {\n";
    code += "\t\t\tif (!mp.count(buf_suc)) mp[buf_suc] = idx++;\n";
    code += "\t\t\tcurrentCode = mp[buf_suc];\n";
    code += "\t\t\tout << currentCode << ' ';\n";
    code += "\t\t}\n";
    
    // 输出到标准输出，格式：行号	单词	编码
    code += "\t\t\tcout << line << \"\\t\" << buf_suc << \"\\t\" << currentCode << \"\\n\";\n";
    
    // 更新读取位置
    code += "\t\tread_cnt += buf_suc.size();\n";
    code += "\t\tin.seekg(read_cnt, ios::beg);\n";
    code += "\t\tskipBlank();\n";
    
    code += "\t}\n";
    
    // 输出单词编码映射
    code += "\tout << endl;\n";
    code += "\tfor (auto item : mp) {\n";
    code += "\t\tout << item.first << ' ' << item.second << ' ';\n";
    code += "\t}\n";
    
    code += "\tin.close();\n";
    code += "\tout.close();\n";
    code += "\treturn 0;\n";
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
    
    // 第一步：在内存中构建完整的状态转移表
    QVector<QVector<int>> transitionTable(minimizedDFA.states.size(), QVector<int>(256, ERROR_STATE));
    
    // 填充实际转移
    for (const auto &transition : minimizedDFA.transitions) {
        if (transition.input.size() == 1) {
            // 单个字符，使用ASCII值进行索引
            char c = transition.input.at(0).toLatin1();
            int asciiValue = static_cast<int>(static_cast<unsigned char>(c));
            transitionTable[transition.fromState][asciiValue] = transition.toState;
        } else {
            // 特殊字符处理
            if (transition.input == "\\n") {
                // 换行符 ASCII值10
                transitionTable[transition.fromState][10] = transition.toState;
            } else if (transition.input == "\\t") {
                // 制表符 ASCII值9
                transitionTable[transition.fromState][9] = transition.toState;
            } else if (transition.input == "\\r") {
                // 回车符 ASCII值13
                transitionTable[transition.fromState][13] = transition.toState;
            } else if (transition.input == "\\0") {
                // 空字符 ASCII值0
                transitionTable[transition.fromState][0] = transition.toState;
            }
        }
    }
    
    // 第二步：生成完整的状态转移表初始化代码
    code += "// 状态转移表: transitions[当前状态][输入字符] = 下一个状态\n";
    code += "int transitions[NUM_STATES][256] = {\n";
    
    for (int i = 0; i < transitionTable.size(); i++) {
        code += "    {";
        for (int j = 0; j < transitionTable[i].size(); j++) {
            if (j > 0) code += ", ";
            code += QString::number(transitionTable[i][j]);
        }
        code += "}";
        if (i < transitionTable.size() - 1) code += ",\n";
    }
    
    code += "\n};\n";
    
    return code;
}

QString LexerGenerator::generateAcceptStatesMap(const QList<RegexItem> &regexItems, const DFA &minimizedDFA)
{
    QString code;
    
    code += "// 接受状态映射\n";
    
    // 第一步：在内存中构建完整的接受状态映射
    int numStates = minimizedDFA.states.size();
    QVector<bool> isAccept(numStates, false);
    QVector<int> tokens(numStates, -1);
    
    // 填充实际的接受状态和token code
    int stateIndex = 0;
    for (const auto &state : minimizedDFA.acceptStates) {
        if (state >= 0 && state < numStates) {
            isAccept[state] = true;
            
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
                tokens[state] = regexItems[regexIndex].code;
            }
        }
    }
    
    // 第二步：生成完整的初始化代码
    code += "bool isAcceptState[NUM_STATES] = {";
    for (int i = 0; i < isAccept.size(); ++i) {
        if (i > 0) {
            code += ", ";
        }
        code += isAccept[i] ? "true" : "false";
    }
    code += "};\n";
    
    code += "int acceptTokens[NUM_STATES] = {";
    for (int i = 0; i < tokens.size(); ++i) {
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
    code += "enum TokenCode {\n";
    
    for (const auto &regexItem : regexItems) {
        if (regexItem.name.startsWith('_')) {
            QString tokenName = regexItem.name.mid(1).toUpper();
            
            // 检查是否为多单词
            if (regexItem.isMultiWord) {
                // 对于多单词，从指定编码开始生成连续编码
                code += "    " + tokenName + " = " + QString::number(regexItem.code) + ",\n";
                
                // 为每个单词生成编码 - 按小写形式分组，确保同一单词的不同大小写形式使用相同编码
                int currentCode = regexItem.code;
                
                // 首先按小写形式分组单词
                QMap<QString, QStringList> lowercaseGroups;
                for (const QString &word : regexItem.wordList) {
                    QString lowercaseWord = word.toLower();
                    lowercaseGroups[lowercaseWord].append(word);
                }
                
                // 为每个分组生成编码
                for (auto it = lowercaseGroups.begin(); it != lowercaseGroups.end(); ++it) {
                    currentCode++;
                    const QString &lowercaseWord = it.key();
                    
                    // 使用小写形式生成枚举名称
                    QString wordTokenName = tokenName + "_" + lowercaseWord.toUpper().replace("+", "PLUS").replace("-", "MINUS").replace("*", "STAR").replace("/", "SLASH").replace(".", "DOT").replace("?", "QUESTION").replace("^", "CARET").replace("$", "DOLLAR").replace("(", "LPAREN").replace(")", "RPAREN").replace("[", "LBRACKET").replace("]", "RBRACKET").replace("{", "LBRACE").replace("}", "RBRACE").replace("|", "PIPE").replace("\\", "BACKSLASH").replace("=","EQUAL").replace("<","LESS").replace(">","MORE");
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
    
    code += "}\n";
    
    // 为多单词添加特殊处理代码
    code += "\n";
    code += "// 多单词映射：从字符串到编码的映射\n";
    code += "std::map<std::string, int> multiWordMap;\n";
    code += "void initMultiWordMap() {\n";
    
    // 初始化多单词映射 - 支持不区分大小写的关键词匹配
    for (const auto &regexItem : regexItems) {
        if (regexItem.name.startsWith('_') && regexItem.isMultiWord) {
            int currentCode = regexItem.code;
            
            // 按小写形式分组，确保同一单词的不同大小写形式使用相同编码
            QMap<QString, QStringList> lowercaseGroups;
            for (const QString &word : regexItem.wordList) {
                QString lowercaseWord = word.toLower();
                lowercaseGroups[lowercaseWord].append(word);
            }
            
            // 为每个分组生成编码并映射所有变体
            for (auto it = lowercaseGroups.begin(); it != lowercaseGroups.end(); ++it) {
                currentCode++;
                const QStringList &variants = it.value();
                
                // 映射该分组的所有大小写变体到同一个编码
                for (const QString &word : variants) {
                    if(word == "\\+") {
                        code += "    multiWordMap[\"+\"] = " + QString::number(currentCode) + ";\n";
                    } else if (word == "\\*") {
                        code += "    multiWordMap[\"*\"] = " + QString::number(currentCode) + ";\n";
                    } else {
                        code += "    multiWordMap[\"" + word + "\"] = " + QString::number(currentCode) + ";\n";
                    }
                }
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
