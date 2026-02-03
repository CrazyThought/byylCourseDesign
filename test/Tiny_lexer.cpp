#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cctype>
using namespace std;

ifstream in;
string buf;
int read_cnt = 0;

void skipBlank() {
	char c;
	while ((c = in.get()) != EOF) {
		if (isspace(static_cast<unsigned char>(c))) {
			read_cnt++;
		} else {
			in.unget();
			break;
		}
	}
}

// DFA状态转移表（嵌套switch-case结构）
const int NUM_STATES = 9;
const int ERROR_STATE = -1;

// 字符类型辅助函数
bool IsDigit(char c) {
    return c >= '0' && c <= '9';
}

bool IsAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool IsAlnum(char c) {
    return IsAlpha(c) || IsDigit(c);
}

bool IsLower(char c) {
    return c >= 'a' && c <= 'z';
}

bool IsUpper(char c) {
    return c >= 'A' && c <= 'Z';
}

// 状态转移函数
int getNextState(int currentState, char input) {
    switch(currentState) {
        case 0: {
            switch(input) {
                case '0':
                    return 0;
                case '1':
                    return 0;
                case '2':
                    return 0;
                case '3':
                    return 0;
                case '4':
                    return 0;
                case '5':
                    return 0;
                case '6':
                    return 0;
                case '7':
                    return 0;
                case '8':
                    return 0;
                case '9':
                    return 0;
                case 'A':
                    return 0;
                case 'B':
                    return 0;
                case 'C':
                    return 0;
                case 'D':
                    return 0;
                case 'E':
                    return 0;
                case 'F':
                    return 0;
                case 'G':
                    return 0;
                case 'H':
                    return 0;
                case 'I':
                    return 0;
                case 'J':
                    return 0;
                case 'K':
                    return 0;
                case 'L':
                    return 0;
                case 'M':
                    return 0;
                case 'N':
                    return 0;
                case 'O':
                    return 0;
                case 'P':
                    return 0;
                case 'Q':
                    return 0;
                case 'R':
                    return 0;
                case 'S':
                    return 0;
                case 'T':
                    return 0;
                case 'U':
                    return 0;
                case 'V':
                    return 0;
                case 'W':
                    return 0;
                case 'X':
                    return 0;
                case 'Y':
                    return 0;
                case 'Z':
                    return 0;
                case 'a':
                    return 0;
                case 'b':
                    return 0;
                case 'c':
                    return 0;
                case 'd':
                    return 0;
                case 'e':
                    return 0;
                case 'f':
                    return 0;
                case 'g':
                    return 0;
                case 'h':
                    return 0;
                case 'i':
                    return 0;
                case 'j':
                    return 0;
                case 'k':
                    return 0;
                case 'l':
                    return 0;
                case 'm':
                    return 0;
                case 'n':
                    return 0;
                case 'o':
                    return 0;
                case 'p':
                    return 0;
                case 'q':
                    return 0;
                case 'r':
                    return 0;
                case 's':
                    return 0;
                case 't':
                    return 0;
                case 'u':
                    return 0;
                case 'v':
                    return 0;
                case 'w':
                    return 0;
                case 'x':
                    return 0;
                case 'y':
                    return 0;
                case 'z':
                    return 0;
                default:
                    return ERROR_STATE;
            }
        }
        case 1: {
            switch(input) {
                case '0':
                    return 1;
                case '1':
                    return 1;
                case '2':
                    return 1;
                case '3':
                    return 1;
                case '4':
                    return 1;
                case '5':
                    return 1;
                case '6':
                    return 1;
                case '7':
                    return 1;
                case '8':
                    return 1;
                case '9':
                    return 1;
                default:
                    return ERROR_STATE;
            }
        }
        case 2: {
            switch(input) {
                case '=':
                    return 4;
                case '>':
                    return 4;
                default:
                    return ERROR_STATE;
            }
        }
        case 3: {
            switch(input) {
                case '=':
                    return 4;
                default:
                    return ERROR_STATE;
            }
        }
        case 4: {
            switch(input) {
                default:
                    return ERROR_STATE;
            }
        }
        case 5: {
            switch(input) {
                case '%':
                    return 4;
                case '*':
                    return 4;
                case '+':
                    return 4;
                case '-':
                    return 4;
                case '/':
                    return 4;
                case '0':
                    return 1;
                case '1':
                    return 1;
                case '2':
                    return 1;
                case '3':
                    return 1;
                case '4':
                    return 1;
                case '5':
                    return 1;
                case '6':
                    return 1;
                case '7':
                    return 1;
                case '8':
                    return 1;
                case '9':
                    return 1;
                case ':':
                    return 6;
                case ';':
                    return 4;
                case '<':
                    return 2;
                case '=':
                    return 4;
                case '>':
                    return 3;
                case 'A':
                    return 0;
                case 'B':
                    return 0;
                case 'C':
                    return 0;
                case 'D':
                    return 0;
                case 'E':
                    return 0;
                case 'F':
                    return 0;
                case 'G':
                    return 0;
                case 'H':
                    return 0;
                case 'I':
                    return 0;
                case 'J':
                    return 0;
                case 'K':
                    return 0;
                case 'L':
                    return 0;
                case 'M':
                    return 0;
                case 'N':
                    return 0;
                case 'O':
                    return 0;
                case 'P':
                    return 0;
                case 'Q':
                    return 0;
                case 'R':
                    return 0;
                case 'S':
                    return 0;
                case 'T':
                    return 0;
                case 'U':
                    return 0;
                case 'V':
                    return 0;
                case 'W':
                    return 0;
                case 'X':
                    return 0;
                case 'Y':
                    return 0;
                case 'Z':
                    return 0;
                case '^':
                    return 4;
                case 'a':
                    return 0;
                case 'b':
                    return 0;
                case 'c':
                    return 0;
                case 'd':
                    return 0;
                case 'e':
                    return 0;
                case 'f':
                    return 0;
                case 'g':
                    return 0;
                case 'h':
                    return 0;
                case 'i':
                    return 0;
                case 'j':
                    return 0;
                case 'k':
                    return 0;
                case 'l':
                    return 0;
                case 'm':
                    return 0;
                case 'n':
                    return 0;
                case 'o':
                    return 0;
                case 'p':
                    return 0;
                case 'q':
                    return 0;
                case 'r':
                    return 0;
                case 's':
                    return 0;
                case 't':
                    return 0;
                case 'u':
                    return 0;
                case 'v':
                    return 0;
                case 'w':
                    return 0;
                case 'x':
                    return 0;
                case 'y':
                    return 0;
                case 'z':
                    return 0;
                case '{':
                    return 7;
                default:
                    return ERROR_STATE;
            }
        }
        case 6: {
            switch(input) {
                case '=':
                    return 4;
                default:
                    return ERROR_STATE;
            }
        }
        case 7: {
            switch(input) {
                case '$':
                    return 7;
                case '%':
                    return 7;
                case '*':
                    return 7;
                case '+':
                    return 7;
                case '-':
                    return 7;
                case '/':
                    return 7;
                case '0':
                    return 7;
                case '1':
                    return 7;
                case '2':
                    return 7;
                case '3':
                    return 7;
                case '4':
                    return 7;
                case '5':
                    return 7;
                case '6':
                    return 7;
                case '7':
                    return 7;
                case '8':
                    return 7;
                case '9':
                    return 7;
                case ':':
                    return 8;
                case ';':
                    return 7;
                case '<':
                    return 7;
                case '=':
                    return 7;
                case '>':
                    return 7;
                case 'A':
                    return 7;
                case 'B':
                    return 7;
                case 'C':
                    return 7;
                case 'D':
                    return 7;
                case 'E':
                    return 7;
                case 'F':
                    return 7;
                case 'G':
                    return 7;
                case 'H':
                    return 7;
                case 'I':
                    return 7;
                case 'J':
                    return 7;
                case 'K':
                    return 7;
                case 'L':
                    return 7;
                case 'M':
                    return 7;
                case 'N':
                    return 7;
                case 'O':
                    return 7;
                case 'P':
                    return 7;
                case 'Q':
                    return 7;
                case 'R':
                    return 7;
                case 'S':
                    return 7;
                case 'T':
                    return 7;
                case 'U':
                    return 7;
                case 'V':
                    return 7;
                case 'W':
                    return 7;
                case 'X':
                    return 7;
                case 'Y':
                    return 7;
                case 'Z':
                    return 7;
                case '^':
                    return 7;
                case 'a':
                    return 7;
                case 'b':
                    return 7;
                case 'c':
                    return 7;
                case 'd':
                    return 7;
                case 'e':
                    return 7;
                case 'f':
                    return 7;
                case 'g':
                    return 7;
                case 'h':
                    return 7;
                case 'i':
                    return 7;
                case 'j':
                    return 7;
                case 'k':
                    return 7;
                case 'l':
                    return 7;
                case 'm':
                    return 7;
                case 'n':
                    return 7;
                case 'o':
                    return 7;
                case 'p':
                    return 7;
                case 'q':
                    return 7;
                case 'r':
                    return 7;
                case 's':
                    return 7;
                case 't':
                    return 7;
                case 'u':
                    return 7;
                case 'v':
                    return 7;
                case 'w':
                    return 7;
                case 'x':
                    return 7;
                case 'y':
                    return 7;
                case 'z':
                    return 7;
                case '}':
                    return 4;
                default:
                    return ERROR_STATE;
            }
        }
        case 8: {
            switch(input) {
                case '=':
                    return 7;
                default:
                    return ERROR_STATE;
            }
        }
        default:
            return ERROR_STATE;
    }
}


bool isAcceptState[NUM_STATES] = {true, true, true, true, true, false, false, false, false};
int acceptTokens[NUM_STATES] = {100, 101, 103, 103, 102, -1, -1, -1, -1};

// Token映射表
#include <unordered_map>

unordered_map<string, int> tokenCodeMap;

void initializeTokenCodeMap() {
    tokenCodeMap["%"] = 103;
    tokenCodeMap["-"] = 104;
    tokenCodeMap["/"] = 105;
    tokenCodeMap[":="] = 106;
    tokenCodeMap[";"] = 107;
    tokenCodeMap["<"] = 108;
    tokenCodeMap["<="] = 109;
    tokenCodeMap["<>"] = 110;
    tokenCodeMap["="] = 111;
    tokenCodeMap[">"] = 112;
    tokenCodeMap[">="] = 113;
    tokenCodeMap["*"] = 114;
    tokenCodeMap["+"] = 115;
    tokenCodeMap["^"] = 116;
    tokenCodeMap["else"] = 200;
    tokenCodeMap["end"] = 201;
    tokenCodeMap["if"] = 202;
    tokenCodeMap["read"] = 203;
    tokenCodeMap["repeat"] = 204;
    tokenCodeMap["then"] = 205;
    tokenCodeMap["until"] = 206;
    tokenCodeMap["write"] = 207;
}

// 关键字列表
bool isKeyword(const string &lexeme) {
    static const string keywords[] = {"ELSE", "ELSe", "ELsE", "ELse", "END", "ENd", "ElSE", "ElSe", "ElsE", "Else", "EnD", "End", "IF", "If", "READ", "REAd", "REPEAT", "REPEAt", "REPEaT", "REPEat", "REPeAT", "REPeAt", "REPeaT", "REPeat", "REaD", "REad", "REpEAT", "REpEAt", "REpEaT", "REpEat", "REpeAT", "REpeAt", "REpeaT", "REpeat", "ReAD", "ReAd", "RePEAT", "RePEAt", "RePEaT", "RePEat", "RePeAT", "RePeAt", "RePeaT", "RePeat", "ReaD", "Read", "RepEAT", "RepEAt", "RepEaT", "RepEat", "RepeAT", "RepeAt", "RepeaT", "Repeat", "THEN", "THEn", "THeN", "THen", "ThEN", "ThEn", "TheN", "Then", "UNTIL", "UNTIl", "UNTiL", "UNTil", "UNtIL", "UNtIl", "UNtiL", "UNtil", "UnTIL", "UnTIl", "UnTiL", "UnTil", "UntIL", "UntIl", "UntiL", "Until", "WRITE", "WRITe", "WRItE", "WRIte", "WRiTE", "WRiTe", "WRitE", "WRite", "WrITE", "WrITe", "WrItE", "WrIte", "WriTE", "WriTe", "WritE", "Write", "eLSE", "eLSe", "eLsE", "eLse", "eND", "eNd", "elSE", "elSe", "elsE", "else", "enD", "end", "iF", "if", "rEAD", "rEAd", "rEPEAT", "rEPEAt", "rEPEaT", "rEPEat", "rEPeAT", "rEPeAt", "rEPeaT", "rEPeat", "rEaD", "rEad", "rEpEAT", "rEpEAt", "rEpEaT", "rEpEat", "rEpeAT", "rEpeAt", "rEpeaT", "rEpeat", "reAD", "reAd", "rePEAT", "rePEAt", "rePEaT", "rePEat", "rePeAT", "rePeAt", "rePeaT", "rePeat", "reaD", "read", "repEAT", "repEAt", "repEaT", "repEat", "repeAT", "repeAt", "repeaT", "repeat", "tHEN", "tHEn", "tHeN", "tHen", "thEN", "thEn", "theN", "then", "uNTIL", "uNTIl", "uNTiL", "uNTil", "uNtIL", "uNtIl", "uNtiL", "uNtil", "unTIL", "unTIl", "unTiL", "unTil", "untIL", "untIl", "untiL", "until", "wRITE", "wRITe", "wRItE", "wRIte", "wRiTE", "wRiTe", "wRitE", "wRite", "wrITE", "wrITe", "wrItE", "wrIte", "wriTE", "wriTe", "writE", "write"};
    static const int keywordCount = sizeof(keywords) / sizeof(keywords[0]);
    
    for (int i = 0; i < keywordCount; i++) {
        if (lexeme == keywords[i]) {
            return true;
        }
    }
    return false;
}

void analyzeToken() {
	int state = 5;
	int lastAcceptState = -1;
	int lastAcceptPos = read_cnt;
	char c;
	buf.clear();

	while ((c = in.peek()) != EOF) {
		int nextState = getNextState(state, c);
		if (nextState != ERROR_STATE) {
			state = nextState;
			buf += c;
			in.get(c);
			read_cnt++;			
			if (isAcceptState[state]) {
				lastAcceptState = state;
				lastAcceptPos = read_cnt;
			}
		}
		else {
			break;
		}
	}

	if (lastAcceptState != -1) {
		// 回退到最后一个接受状态
		int backoff = read_cnt - lastAcceptPos;
		in.seekg(-backoff, ios::cur);
		read_cnt = lastAcceptPos;
		buf.resize(buf.size() - backoff);		
		
		// 输出格式：单词	编码
		int tokenCode = acceptTokens[lastAcceptState];

		// 检查是否在token映射表中（多单词编码）
		// 首先尝试精确匹配
		auto it = tokenCodeMap.find(buf);
		if (it != tokenCodeMap.end()) {
			tokenCode = it->second;
		}
		// 尝试转义后的形式（处理*+^等特殊字符）
		else {
			string escapedBuf = "\\" + buf + "\\";
			auto escapedIt = tokenCodeMap.find(escapedBuf);
			if (escapedIt != tokenCodeMap.end()) {
				tokenCode = escapedIt->second;
			}
			// 然后尝试大小写不敏感匹配
			else {
				string lowercaseBuf;
				for (char ch : buf) {
					lowercaseBuf += tolower(static_cast<unsigned char>(ch));
				}
				auto lowercaseIt = tokenCodeMap.find(lowercaseBuf);
				if (lowercaseIt != tokenCodeMap.end()) {
					tokenCode = lowercaseIt->second;
				}
			}
		}

		cout << buf << '	' << tokenCode << endl;
	}
	else {
		// 跳过无法识别的字符
		buf.clear();
		in.get();
		read_cnt++;
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " <source_file>" << endl;
		return 1;
	}

	// 打开输入文件
	in.open(argv[1], ios::in);
	if (!in.is_open()) {
		cerr << "Error: Could not open file '" << argv[1] << "'." << endl;
		return 1;
	}

	// 初始化token代码映射表
	initializeTokenCodeMap();

	// 跳过初始空白字符
	skipBlank();

	// 主循环
	while (in.peek() != EOF) {
		analyzeToken();
		skipBlank();
	}

	// 关闭文件
	in.close();
	return 0;
}
