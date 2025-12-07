#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cctype>
#include <map>
using namespace std;

ifstream in;
ofstream out("sample.lex", ios::out | ios::trunc);
string buf, buf_err, buf_suc;
string token, token_suc;
int read_cnt;
int line = 1;
map<string, int> mp;
int idx = 1;

void skipBlank() {
	char c;
	while (in.get(c)) {
		read_cnt++;
		if (c == '\n') {
			read_cnt++;
			line++;
		}
		if (!isspace(c)) {
			read_cnt--;
			in.unget();
			break;
		}
	}
}

bool IsDigit(char c) {
	if (c >= '0' && c <= '9') return 1;
	else return 0;
}

bool IsPositiveDigit(char c) {
	if (c >= '1' && c <= '9') return 1;
	else return 0;
}

bool IsAlpha(char c) {
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1;
	else return 0;
}

bool IsLower(char c) {
	if (c >= 'a' && c <= 'z') return 1;
	else return 0;
}

bool IsUpper(char c) {
	if (c >= 'A' && c <= 'Z') return 1;
	else return 0;
}

bool check_identifier100() {
	int state = 5;
	char c;
	while ((c = in.peek()) != EOF) {
		switch(state) {
		case 0:
			if (IsDigit(c)) {
				state = 0;
				buf += c;
				in.get(c);
				break;
			}
			if (IsAlpha(c)) {
				state = 0;
				buf += c;
				in.get(c);
				break;
			}
			switch(c) {
			default:
				token = "identifier100";
				return true;
			}
			break;
		case 1:
			if (IsDigit(c)) {
				state = 1;
				buf += c;
				in.get(c);
				break;
			}
			switch(c) {
			default:
				token = "identifier100";
				return true;
			}
			break;
		case 2:
			switch(c) {
			case '=':
				state = 4;
				buf += c;
				in.get(c);
				break;
			case '>':
				state = 4;
				buf += c;
				in.get(c);
				break;
			default:
				token = "identifier100";
				return true;
			}
			break;
		case 3:
			switch(c) {
			case '=':
				state = 4;
				buf += c;
				in.get(c);
				break;
			default:
				token = "identifier100";
				return true;
			}
			break;
		case 4:
			switch(c) {
			default:
				token = "identifier100";
				return true;
			}
			break;
		case 5:
			if (IsDigit(c)) {
				state = 1;
				buf += c;
				in.get(c);
				break;
			}
			if (IsAlpha(c)) {
				state = 0;
				buf += c;
				in.get(c);
				break;
			}
			switch(c) {
			case '%':
				state = 4;
				buf += c;
				in.get(c);
				break;
			case '*':
				state = 4;
				buf += c;
				in.get(c);
				break;
			case '+':
				state = 4;
				buf += c;
				in.get(c);
				break;
			case '-':
				state = 4;
				buf += c;
				in.get(c);
				break;
			case '/':
				state = 4;
				buf += c;
				in.get(c);
				break;
			case ':':
				state = 6;
				buf += c;
				in.get(c);
				break;
			case ';':
				state = 4;
				buf += c;
				in.get(c);
				break;
			case '<':
				state = 2;
				buf += c;
				in.get(c);
				break;
			case '=':
				state = 4;
				buf += c;
				in.get(c);
				break;
			case '>':
				state = 3;
				buf += c;
				in.get(c);
				break;
			case '^':
				state = 4;
				buf += c;
				in.get(c);
				break;
			case '{':
				state = 7;
				buf += c;
				in.get(c);
				break;
			default:
				return false;
			}
			break;
		case 6:
			switch(c) {
			case '=':
				state = 4;
				buf += c;
				in.get(c);
				break;
			default:
				return false;
			}
			break;
		case 7:
			if (IsDigit(c)) {
				state = 7;
				buf += c;
				in.get(c);
				break;
			}
			if (IsAlpha(c)) {
				state = 7;
				buf += c;
				in.get(c);
				break;
			}
			switch(c) {
			case '$':
				state = 7;
				buf += c;
				in.get(c);
				break;
			case '%':
				state = 7;
				buf += c;
				in.get(c);
				break;
			case '*':
				state = 7;
				buf += c;
				in.get(c);
				break;
			case '+':
				state = 7;
				buf += c;
				in.get(c);
				break;
			case '-':
				state = 7;
				buf += c;
				in.get(c);
				break;
			case '/':
				state = 7;
				buf += c;
				in.get(c);
				break;
			case ':':
				state = 8;
				buf += c;
				in.get(c);
				break;
			case ';':
				state = 7;
				buf += c;
				in.get(c);
				break;
			case '<':
				state = 7;
				buf += c;
				in.get(c);
				break;
			case '=':
				state = 7;
				buf += c;
				in.get(c);
				break;
			case '>':
				state = 7;
				buf += c;
				in.get(c);
				break;
			case '^':
				state = 7;
				buf += c;
				in.get(c);
				break;
			case '}':
				state = 4;
				buf += c;
				in.get(c);
				break;
			default:
				return false;
			}
			break;
		case 8:
			switch(c) {
			case '=':
				state = 7;
				buf += c;
				in.get(c);
				break;
			default:
				return false;
			}
			break;
		}
	}
	if (state == 3||state == 0||state == 4||state == 2||state == 1) {
		token = "identifier100";
		return true;
	}
	else return false;
}

int main(int argc, char *argv[]) {
	bool flag;
	char c;
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " <source_file>" << endl;
		return 1;
	}
	in.open(argv[1], ios::in);
	if (!in.is_open()) {
		cerr << "Error: Could not open file '" << argv[1] << "'." << endl;
		return 1;
	}
	read_cnt = 0;
	skipBlank();
	while ((c = in.peek()) != EOF) {
		token_suc.clear();
		buf_suc.clear();
		if (!check_identifier100()) buf_err = buf;
		else if (buf.size() > buf_suc.size()) {
			buf_suc = buf;
			token_suc = token;
		}
		buf.clear();
		in.seekg(read_cnt, ios::beg);
		if (buf_suc.empty()) {
			out << buf_err << " UNKNOWN" << endl;
			cout << line << "\t" << buf_err << "\t" << -1 << "\n";
			exit(1);
		}
		int currentCode;
		if (token_suc.empty() || !isupper(token_suc[0])) {
			if (!mp.count(token_suc)) mp[token_suc] = idx++;
			currentCode = mp[token_suc];
			out << currentCode << ' ' << buf_suc << ' ';
		} else {
			if (!mp.count(buf_suc)) mp[buf_suc] = idx++;
			currentCode = mp[buf_suc];
			out << currentCode << ' ';
		}
			cout << line << "\t" << buf_suc << "\t" << currentCode << "\n";
		read_cnt += buf_suc.size();
		in.seekg(read_cnt, ios::beg);
		skipBlank();
	}
	out << endl;
	for (auto item : mp) {
		out << item.first << ' ' << item.second << ' ';
	}
	in.close();
	out.close();
	system("pause");
	return 0;
}
