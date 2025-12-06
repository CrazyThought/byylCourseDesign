#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

// 简化的RegexItem结构体
struct RegexItem {
    std::string name;
    std::string pattern;
    int code;
    bool isMultiWord;
};

// 简化的DFA结构体
struct DFATransition {
    int fromState;
    std::string input;
    int toState;
};

struct DFA {
    int startState;
    std::vector<int> states;
    std::vector<int> acceptingStates;
    std::vector<DFATransition> transitions;
};

// 简化的LexerGenerator类，包含修复后的generateStateTransitionLexer函数
class LexerGenerator {
public:
    std::string generateStateTransitionLexer(const std::vector<RegexItem> &regexItems, const DFA &minimizedDFA) {
        std::string code;
        std::string dfaKey = "token";
        
        // 生成头文件
        code += "#include <iostream>\n";
        code += "#include <fstream>\n";
        code += "#include <string>\n";
        code += "#include <cstring>\n";
        code += "#include <cctype>\n";
        code += "#include <map>\n";
        code += "using namespace std;\n\n";
        
        // 生成全局变量
        code += "ifstream in;\n";
        code += "ofstream out(\"sample.lex\", ios::out | ios::trunc);\n";
        code += "string buf, buf_err, buf_suc;\n";
        code += "string token, token_suc;\n";
        code += "int read_cnt;\n";
        code += "int line = 1;\n";
        code += "map<string, int> mp;\n";
        code += "int idx = 1;\n\n";
        
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
        
        // 生成check函数
        code += "bool check_token() {\n";
        code += "\tint state = 0;\n";
        code += "\tchar c;\n";
        code += "\twhile ((c = in.peek()) != EOF) {\n";
        code += "\t\tswitch(state) {\n";
        code += "\t\tcase 0:\n";
        code += "\t\t\tif (isalpha(c) || c == '_') {\n";
        code += "\t\t\t\tstate = 1;\n";
        code += "\t\t\t\tbuf += c;\n";
        code += "\t\t\t\tin.get(c);\n";
        code += "\t\t\t\tbreak;\n";
        code += "\t\t\t}\n";
        code += "\t\t\tif (isdigit(c)) {\n";
        code += "\t\t\t\tstate = 2;\n";
        code += "\t\t\t\tbuf += c;\n";
        code += "\t\t\t\tin.get(c);\n";
        code += "\t\t\t\tbreak;\n";
        code += "\t\t\t}\n";
        code += "\t\t\treturn state == 1 || state == 2;\n";
        code += "\t\tcase 1:\n";
        code += "\t\t\tif (isalnum(c) || c == '_') {\n";
        code += "\t\t\t\tstate = 1;\n";
        code += "\t\t\t\tbuf += c;\n";
        code += "\t\t\t\tin.get(c);\n";
        code += "\t\t\t\tbreak;\n";
        code += "\t\t\t}\n";
        code += "\t\t\treturn true;\n";
        code += "\t\tcase 2:\n";
        code += "\t\t\tif (isdigit(c)) {\n";
        code += "\t\t\t\tstate = 2;\n";
        code += "\t\t\t\tbuf += c;\n";
        code += "\t\t\t\tin.get(c);\n";
        code += "\t\t\t\tbreak;\n";
        code += "\t\t\t}\n";
        code += "\t\t\treturn true;\n";
        code += "\t\tdefault:\n";
        code += "\t\t\treturn false;\n";
        code += "\t\t}\n";
        code += "\t}\n";
        code += "\treturn state == 1 || state == 2;\n";
        code += "}\n\n";
        
        // 生成主函数
        code += "int main(int argc, char *argv[]) {\n";
        code += "\tif (argc < 2) {\n";
        code += "\t\tcerr << \"Usage: \" << argv[0] << \" <source_file>\" << endl;\n";
        code += "\t\treturn 1;\n";
        code += "\t}\n";
        code += "\tin.open(argv[1], ios::in);\n";
        code += "\tif (!in.is_open()) {\n";
        code += "\t\tcerr << \"Error: Could not open file '\" << argv[1] << \"'.\" << endl;\n";
        code += "\t\treturn 1;\n";
        code += "\t}\n";
        code += "\tread_cnt = 0;\n";
        code += "\tskipBlank();\n";
        code += "\twhile ((char)in.peek() != EOF) {\n";
        code += "\t	token_suc.clear();\n";
        code += "\t\tbuf_suc.clear();\n";
        code += "\t\tif (!check_token()) buf_err = buf;\n";
        code += "\t\telse if (buf.size() > buf_suc.size()) {\n";
        code += "\t\t\tbuf_suc = buf;\n";
        code += "\t\t\ttoken_suc = token;\n";
        code += "\t\t}\n";
        code += "\t\tbuf.clear();\n";
        code += "\t\tin.seekg(read_cnt, ios::beg);\n";
        code += "\t\tif (buf_suc.empty()) {\n";
        code += "\t\t\tout << buf_err << \" UNKNOWN\" << endl;\n";
        code += "\t\t\tcout << line << \"\\t\" << buf_err << \"\\t" << -1 << "\\n";\n";
        code += "\t\t\texit(1);\n";
        code += "\t\t}\n";
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
        code += "\t\tcout << line << \"\\t\" << buf_suc << \"\\t\" << currentCode << \"\\n";\n";
        code += "\t\tread_cnt += buf_suc.size();\n";
        code += "\t\tin.seekg(read_cnt, ios::beg);\n";
        code += "\t	skipBlank();\n";
        code += "\t}\n";
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
};

int main() {
    // 创建RegexItem列表
    std::vector<RegexItem> regexItems;
    regexItems.push_back({"_keyword", "if|else|while|for|int|float|double|char|bool|void", 100, true});
    regexItems.push_back({"_identifier", "[a-zA-Z_][a-zA-Z0-9_]*", 200, false});
    regexItems.push_back({"_number", "[0-9]+(\.[0-9]+)?", 300, false});
    regexItems.push_back({"_operator", "+|-|*|/|=|==|!=|<|>|<=|">=", 400, false});
    regexItems.push_back({"_delimiter", "\\(|\\)|\\{|\\}|\\[\\]|;|,", 500, false});
    
    // 创建DFA
    DFA dfa;
    dfa.startState = 0;
    dfa.states = {0, 1, 2};
    dfa.acceptingStates = {1, 2};
    dfa.transitions = {
        {0, "[a-zA-Z_]", 1},
        {0, "[0-9]", 2},
        {1, "[a-zA-Z0-9_]", 1},
        {2, "[0-9]", 2}
    };
    
    // 生成词法分析器
    LexerGenerator generator;
    std::string lexerCode = generator.generateStateTransitionLexer(regexItems, dfa);
    
    // 保存到文件
    std::ofstream out("test_fixed_lexer.cpp");
    out << lexerCode;
    out.close();
    
    std::cout << "修复后的词法分析器生成完成，保存到 test_fixed_lexer.cpp" << std::endl;
    
    return 0;
}
