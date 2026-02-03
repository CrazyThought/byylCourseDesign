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
                case '\%':
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
                    return 7;
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
                    return 6;
                default:
                    return ERROR_STATE;
            }
        }
        case 6: {
            switch(input) {
                case '$':
                    return 6;
                case '\%':
                    return 6;
                case '*':
                    return 6;
                case '+':
                    return 6;
                case '-':
                    return 6;
                case '/':
                    return 6;
                case '0':
                    return 6;
                case '1':
                    return 6;
                case '2':
                    return 6;
                case '3':
                    return 6;
                case '4':
                    return 6;
                case '5':
                    return 6;
                case '6':
                    return 6;
                case '7':
                    return 6;
                case '8':
                    return 6;
                case '9':
                    return 6;
                case ':':
                    return 8;
                case ';':
                    return 6;
                case '<':
                    return 6;
                case '=':
                    return 6;
                case '>':
                    return 6;
                case 'A':
                    return 6;
                case 'B':
                    return 6;
                case 'C':
                    return 6;
                case 'D':
                    return 6;
                case 'E':
                    return 6;
                case 'F':
                    return 6;
                case 'G':
                    return 6;
                case 'H':
                    return 6;
                case 'I':
                    return 6;
                case 'J':
                    return 6;
                case 'K':
                    return 6;
                case 'L':
                    return 6;
                case 'M':
                    return 6;
                case 'N':
                    return 6;
                case 'O':
                    return 6;
                case 'P':
                    return 6;
                case 'Q':
                    return 6;
                case 'R':
                    return 6;
                case 'S':
                    return 6;
                case 'T':
                    return 6;
                case 'U':
                    return 6;
                case 'V':
                    return 6;
                case 'W':
                    return 6;
                case 'X':
                    return 6;
                case 'Y':
                    return 6;
                case 'Z':
                    return 6;
                case '^':
                    return 6;
                case 'a':
                    return 6;
                case 'b':
                    return 6;
                case 'c':
                    return 6;
                case 'd':
                    return 6;
                case 'e':
                    return 6;
                case 'f':
                    return 6;
                case 'g':
                    return 6;
                case 'h':
                    return 6;
                case 'i':
                    return 6;
                case 'j':
                    return 6;
                case 'k':
                    return 6;
                case 'l':
                    return 6;
                case 'm':
                    return 6;
                case 'n':
                    return 6;
                case 'o':
                    return 6;
                case 'p':
                    return 6;
                case 'q':
                    return 6;
                case 'r':
                    return 6;
                case 's':
                    return 6;
                case 't':
                    return 6;
                case 'u':
                    return 6;
                case 'v':
                    return 6;
                case 'w':
                    return 6;
                case 'x':
                    return 6;
                case 'y':
                    return 6;
                case 'z':
                    return 6;
                case '}':
                    return 4;
                default:
                    return ERROR_STATE;
            }
        }
        case 7: {
            switch(input) {
                case '=':
                    return 4;
                default:
                    return ERROR_STATE;
            }
        }
        case 8: {
            switch(input) {
                case '=':
                    return 6;
                default:
                    return ERROR_STATE;
            }
        }
        default:
            return ERROR_STATE;
    }
}


bool isAcceptState[NUM_STATES] = {true, true, true, true, true, false, true, false, false};
int acceptTokens[NUM_STATES] = {100, 101, 103, 103, 103, -1, 102, -1, -1};

// 关键字列表
bool isKeyword(const string &lexeme) {
    static const string keywords[] = {"if", "then", "else", "end", "read", "write", "repeat", "until"};
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

		// 检查是否为注释
		if (buf.size() >= 2 && buf[0] == '{' && buf[buf.size()-1] == '}') {
			tokenCode = 102;
		}
		// 检查是否为关键字
		else if (tokenCode == 100 && isKeyword(buf)) {
			tokenCode = 200;
		}

		cout << buf << '\t' << tokenCode << endl;
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
