#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
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

// DFA状态转移表
const int NUM_STATES = 9;
const int ERROR_STATE = -1;

// 状态转移表: transitions[当前状态][输入字符ASCII值] = 下一个状态
int transitions[NUM_STATES][256] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4, -1, -1, -1, -1, 4, 4, -1, 4, -1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7, 4, 2, 4, 3, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 4, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 6, 6, -1, -1, -1, -1, 6, 6, -1, 6, -1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 6, 6, 6, 6, -1, -1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, -1, -1, -1, 6, -1, -1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

// 接受状态映射
bool isAcceptState[NUM_STATES] = {true, true, true, true, true, false, false, false, false};
int acceptTokens[NUM_STATES] = {100, 100, 100, 100, 100, -1, -1, -1, -1};

// 单词编码映射
map<string, int> tokenCodeMap;
int nextTokenCode = 1;

// 初始化单词编码映射表
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
    tokenCodeMap["ELSE"] = 200;
    tokenCodeMap["ELSe"] = 200;
    tokenCodeMap["ELsE"] = 200;
    tokenCodeMap["ELse"] = 200;
    tokenCodeMap["END"] = 201;
    tokenCodeMap["ENd"] = 201;
    tokenCodeMap["ElSE"] = 200;
    tokenCodeMap["ElSe"] = 200;
    tokenCodeMap["ElsE"] = 200;
    tokenCodeMap["Else"] = 200;
    tokenCodeMap["EnD"] = 201;
    tokenCodeMap["End"] = 201;
    tokenCodeMap["IF"] = 202;
    tokenCodeMap["If"] = 202;
    tokenCodeMap["READ"] = 203;
    tokenCodeMap["REAd"] = 203;
    tokenCodeMap["REPEAT"] = 204;
    tokenCodeMap["REPEAt"] = 204;
    tokenCodeMap["REPEaT"] = 204;
    tokenCodeMap["REPEat"] = 204;
    tokenCodeMap["REPeAT"] = 204;
    tokenCodeMap["REPeAt"] = 204;
    tokenCodeMap["REPeaT"] = 204;
    tokenCodeMap["REPeat"] = 204;
    tokenCodeMap["REaD"] = 203;
    tokenCodeMap["REad"] = 203;
    tokenCodeMap["REpEAT"] = 204;
    tokenCodeMap["REpEAt"] = 204;
    tokenCodeMap["REpEaT"] = 204;
    tokenCodeMap["REpEat"] = 204;
    tokenCodeMap["REpeAT"] = 204;
    tokenCodeMap["REpeAt"] = 204;
    tokenCodeMap["REpeaT"] = 204;
    tokenCodeMap["REpeat"] = 204;
    tokenCodeMap["ReAD"] = 203;
    tokenCodeMap["ReAd"] = 203;
    tokenCodeMap["RePEAT"] = 204;
    tokenCodeMap["RePEAt"] = 204;
    tokenCodeMap["RePEaT"] = 204;
    tokenCodeMap["RePEat"] = 204;
    tokenCodeMap["RePeAT"] = 204;
    tokenCodeMap["RePeAt"] = 204;
    tokenCodeMap["RePeaT"] = 204;
    tokenCodeMap["RePeat"] = 204;
    tokenCodeMap["ReaD"] = 203;
    tokenCodeMap["Read"] = 203;
    tokenCodeMap["RepEAT"] = 204;
    tokenCodeMap["RepEAt"] = 204;
    tokenCodeMap["RepEaT"] = 204;
    tokenCodeMap["RepEat"] = 204;
    tokenCodeMap["RepeAT"] = 204;
    tokenCodeMap["RepeAt"] = 204;
    tokenCodeMap["RepeaT"] = 204;
    tokenCodeMap["Repeat"] = 204;
    tokenCodeMap["THEN"] = 205;
    tokenCodeMap["THEn"] = 205;
    tokenCodeMap["THeN"] = 205;
    tokenCodeMap["THen"] = 205;
    tokenCodeMap["ThEN"] = 205;
    tokenCodeMap["ThEn"] = 205;
    tokenCodeMap["TheN"] = 205;
    tokenCodeMap["Then"] = 205;
    tokenCodeMap["UNTIL"] = 206;
    tokenCodeMap["UNTIl"] = 206;
    tokenCodeMap["UNTiL"] = 206;
    tokenCodeMap["UNTil"] = 206;
    tokenCodeMap["UNtIL"] = 206;
    tokenCodeMap["UNtIl"] = 206;
    tokenCodeMap["UNtiL"] = 206;
    tokenCodeMap["UNtil"] = 206;
    tokenCodeMap["UnTIL"] = 206;
    tokenCodeMap["UnTIl"] = 206;
    tokenCodeMap["UnTiL"] = 206;
    tokenCodeMap["UnTil"] = 206;
    tokenCodeMap["UntIL"] = 206;
    tokenCodeMap["UntIl"] = 206;
    tokenCodeMap["UntiL"] = 206;
    tokenCodeMap["Until"] = 206;
    tokenCodeMap["WRITE"] = 207;
    tokenCodeMap["WRITe"] = 207;
    tokenCodeMap["WRItE"] = 207;
    tokenCodeMap["WRIte"] = 207;
    tokenCodeMap["WRiTE"] = 207;
    tokenCodeMap["WRiTe"] = 207;
    tokenCodeMap["WRitE"] = 207;
    tokenCodeMap["WRite"] = 207;
    tokenCodeMap["WrITE"] = 207;
    tokenCodeMap["WrITe"] = 207;
    tokenCodeMap["WrItE"] = 207;
    tokenCodeMap["WrIte"] = 207;
    tokenCodeMap["WriTE"] = 207;
    tokenCodeMap["WriTe"] = 207;
    tokenCodeMap["WritE"] = 207;
    tokenCodeMap["Write"] = 207;
    tokenCodeMap["eLSE"] = 200;
    tokenCodeMap["eLSe"] = 200;
    tokenCodeMap["eLsE"] = 200;
    tokenCodeMap["eLse"] = 200;
    tokenCodeMap["eND"] = 201;
    tokenCodeMap["eNd"] = 201;
    tokenCodeMap["elSE"] = 200;
    tokenCodeMap["elSe"] = 200;
    tokenCodeMap["elsE"] = 200;
    tokenCodeMap["else"] = 200;
    tokenCodeMap["enD"] = 201;
    tokenCodeMap["end"] = 201;
    tokenCodeMap["iF"] = 202;
    tokenCodeMap["if"] = 202;
    tokenCodeMap["rEAD"] = 203;
    tokenCodeMap["rEAd"] = 203;
    tokenCodeMap["rEPEAT"] = 204;
    tokenCodeMap["rEPEAt"] = 204;
    tokenCodeMap["rEPEaT"] = 204;
    tokenCodeMap["rEPEat"] = 204;
    tokenCodeMap["rEPeAT"] = 204;
    tokenCodeMap["rEPeAt"] = 204;
    tokenCodeMap["rEPeaT"] = 204;
    tokenCodeMap["rEPeat"] = 204;
    tokenCodeMap["rEaD"] = 203;
    tokenCodeMap["rEad"] = 203;
    tokenCodeMap["rEpEAT"] = 204;
    tokenCodeMap["rEpEAt"] = 204;
    tokenCodeMap["rEpEaT"] = 204;
    tokenCodeMap["rEpEat"] = 204;
    tokenCodeMap["rEpeAT"] = 204;
    tokenCodeMap["rEpeAt"] = 204;
    tokenCodeMap["rEpeaT"] = 204;
    tokenCodeMap["rEpeat"] = 204;
    tokenCodeMap["reAD"] = 203;
    tokenCodeMap["reAd"] = 203;
    tokenCodeMap["rePEAT"] = 204;
    tokenCodeMap["rePEAt"] = 204;
    tokenCodeMap["rePEaT"] = 204;
    tokenCodeMap["rePEat"] = 204;
    tokenCodeMap["rePeAT"] = 204;
    tokenCodeMap["rePeAt"] = 204;
    tokenCodeMap["rePeaT"] = 204;
    tokenCodeMap["rePeat"] = 204;
    tokenCodeMap["reaD"] = 203;
    tokenCodeMap["read"] = 203;
    tokenCodeMap["repEAT"] = 204;
    tokenCodeMap["repEAt"] = 204;
    tokenCodeMap["repEaT"] = 204;
    tokenCodeMap["repEat"] = 204;
    tokenCodeMap["repeAT"] = 204;
    tokenCodeMap["repeAt"] = 204;
    tokenCodeMap["repeaT"] = 204;
    tokenCodeMap["repeat"] = 204;
    tokenCodeMap["tHEN"] = 205;
    tokenCodeMap["tHEn"] = 205;
    tokenCodeMap["tHeN"] = 205;
    tokenCodeMap["tHen"] = 205;
    tokenCodeMap["thEN"] = 205;
    tokenCodeMap["thEn"] = 205;
    tokenCodeMap["theN"] = 205;
    tokenCodeMap["then"] = 205;
    tokenCodeMap["uNTIL"] = 206;
    tokenCodeMap["uNTIl"] = 206;
    tokenCodeMap["uNTiL"] = 206;
    tokenCodeMap["uNTil"] = 206;
    tokenCodeMap["uNtIL"] = 206;
    tokenCodeMap["uNtIl"] = 206;
    tokenCodeMap["uNtiL"] = 206;
    tokenCodeMap["uNtil"] = 206;
    tokenCodeMap["unTIL"] = 206;
    tokenCodeMap["unTIl"] = 206;
    tokenCodeMap["unTiL"] = 206;
    tokenCodeMap["unTil"] = 206;
    tokenCodeMap["untIL"] = 206;
    tokenCodeMap["untIl"] = 206;
    tokenCodeMap["untiL"] = 206;
    tokenCodeMap["until"] = 206;
    tokenCodeMap["wRITE"] = 207;
    tokenCodeMap["wRITe"] = 207;
    tokenCodeMap["wRItE"] = 207;
    tokenCodeMap["wRIte"] = 207;
    tokenCodeMap["wRiTE"] = 207;
    tokenCodeMap["wRiTe"] = 207;
    tokenCodeMap["wRitE"] = 207;
    tokenCodeMap["wRite"] = 207;
    tokenCodeMap["wrITE"] = 207;
    tokenCodeMap["wrITe"] = 207;
    tokenCodeMap["wrItE"] = 207;
    tokenCodeMap["wrIte"] = 207;
    tokenCodeMap["wriTE"] = 207;
    tokenCodeMap["wriTe"] = 207;
    tokenCodeMap["writE"] = 207;
    tokenCodeMap["write"] = 207;
}


void analyzeToken() {
	int state = 5;
	int lastAcceptState = -1;
	int lastAcceptPos = read_cnt;
	char c;
	buf.clear();

	while ((c = in.peek()) != EOF) {
		int input = static_cast<unsigned char>(c);
		int nextState = transitions[state][input];		
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
		// 检查tokenCodeMap中是否有对应的条目，实现大小写不敏感处理
		string lowercaseBuf = buf;
		transform(lowercaseBuf.begin(), lowercaseBuf.end(), lowercaseBuf.begin(), ::tolower);
		if (tokenCodeMap.find(lowercaseBuf) != tokenCodeMap.end()) {
			tokenCode = tokenCodeMap[lowercaseBuf];
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
	// 初始化单词编码映射表
	initializeTokenCodeMap();

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " <source_file>" << endl;
		return 1;
	}

	// 打开输入文件
	in.open(argv[1], ios::in);
	if (!in.is_open()) {
		cerr << "Error: Could not open file '" << argv[1] << "." << endl;
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
