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

bool check_token() {
	int state = 0;
	char c;
	while ((c = in.peek()) != EOF) {
		switch(state) {
		case 0:
			if (isalpha(c) || c == '_') {
				state = 1;
				buf += c;
				in.get(c);
				break;
			}
			if (isdigit(c)) {
				state = 2;
				buf += c;
				in.get(c);
				break;
			}
			return state == 1 || state == 2;
		case 1:
			if (isalnum(c) || c == '_') {
				state = 1;
				buf += c;
				in.get(c);
				break;
			}
			return true;
		case 2:
			if (isdigit(c)) {
				state = 2;
				buf += c;
				in.get(c);
				break;
			}
			return true;
		default:
			return false;
		}
	}
	return state == 1 || state == 2;
}

int main(int argc, char *argv[]) {
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
	while ((char)in.peek() != EOF) {
		token_suc.clear();
		buf_suc.clear();
		if (!check_token()) buf_err = buf;
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
		if (!isupper(token_suc[0])) {
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
	return 0;
}
