#include <QCoreApplication>
#include <QList>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include "task1/lexergenerator.h"
#include "task1/dfabuilder.h"
#include "task1/nfabuilder.h"
#include "task1/dfaminimizer.h"
#include "task1/regexprocessor.h"

// 从文件读取正则表达式定义
QList<RegexItem> readRegexItemsFromFile(const QString &filePath)
{
    QList<RegexItem> regexItems;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件：" << filePath;
        return regexItems;
    }
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("//")) {
            continue;
        }
        
        // 解析正则表达式定义
        // 格式：name=pattern 或 nameCode=pattern 或 nameCodeS=pattern
        int equalPos = line.indexOf('=');
        if (equalPos == -1) {
            continue;
        }
        
        QString namePart = line.left(equalPos);
        QString pattern = line.mid(equalPos + 1);
        
        // 解析名称和代码
        QString name;
        int code = 0;
        bool isMultiWord = false;
        
        // 检查是否包含代码
        QRegularExpression digitRegex("\\d+");
        if (namePart.contains(digitRegex)) {
            // 提取代码
            QRegularExpressionMatch match = digitRegex.match(namePart);
            if (match.hasMatch()) {
                code = match.captured(0).toInt();
                name = namePart.left(match.capturedStart(0));
            }
        } else {
            name = namePart;
        }
        
        // 检查是否为多单词项（以S结尾）
        if (namePart.endsWith('S')) {
            isMultiWord = true;
        }
        
        // 创建RegexItem
        RegexItem item;
        item.name = name;
        item.code = code;
        item.isMultiWord = isMultiWord;
        item.pattern = pattern;
        
        // 对于多单词项，提取单词列表
        if (isMultiWord) {
            // 对于keyword项，提取单词列表
            if (name.contains("keyword", Qt::CaseInsensitive)) {
                // 从pattern中提取单词
                // pattern格式：(i|I)(f|F)|(t|T)(h|H)(e|E)(n|N)|...
                QStringList parts = pattern.split('|');
                for (const QString &part : parts) {
                    QString word;
                    // 提取每个字母
                    for (int i = 0; i < part.length(); i++) {
                        QChar c = part.at(i);
                        if (c.isLetter()) {
                            word += c.toLower();
                        }
                    }
                    if (!word.isEmpty()) {
                        item.wordList << word;
                    }
                }
            } else if (name.contains("special", Qt::CaseInsensitive)) {
                // 对于special项，提取符号
                QStringList parts = pattern.split('|');
                for (const QString &part : parts) {
                    QString trimmedPart = part.trimmed();
                    if (!trimmedPart.isEmpty()) {
                        item.wordList << trimmedPart;
                    }
                }
            }
        }
        
        regexItems.append(item);
    }
    
    file.close();
    return regexItems;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    // 读取Tiny示例中的正则表达式定义
    QString regexFile = "test/Tiny/regix_sample.txt";
    QList<RegexItem> regexItems = readRegexItemsFromFile(regexFile);
    
    if (regexItems.isEmpty()) {
        qDebug() << "无法读取正则表达式定义";
        return 1;
    }
    
    qDebug() << "=== 读取的正则表达式项 ===";
    for (const RegexItem &item : regexItems) {
        qDebug() << "名称：" << item.name << "，代码：" << item.code << "，多单词：" << item.isMultiWord;
        if (item.isMultiWord) {
            qDebug() << "单词列表：" << item.wordList;
        }
    }
    
    // 为每个正则表达式生成NFA
    NFABuilder nfaBuilder;
    QMap<QString, NFA> nfaMap;
    
    for (int i = 0; i < regexItems.size(); i++) {
        const RegexItem &item = regexItems[i];
        // 仅为下划线开头的正则表达式生成NFA
        if (item.name.startsWith('_')) {
            NFA nfa = nfaBuilder.buildNFA(item);
            if (!nfa.states.isEmpty()) {
                // 设置接受状态的正则表达式索引
                for (const NFAState &state : nfa.acceptStates) {
                    nfa.acceptStateToRegexIndex[state] = i;
                }
                nfaMap[item.name] = nfa;
                qDebug() << "生成NFA成功：" << item.name;
            }
        }
    }
    
    if (nfaMap.isEmpty()) {
        qDebug() << "无法生成NFA";
        return 1;
    }
    
    // 合并所有NFA为一个总NFA
    NFA totalNFA;
    int stateOffset = 0;
    QList<NFAState> allStartStates;
    
    for (const NFA &nfa : nfaMap.values()) {
        // 处理状态
        for (const NFAState &state : nfa.states) {
            totalNFA.states.append(state + stateOffset);
        }
        
        // 处理转移
        for (const NFATransition &transition : nfa.transitions) {
            NFATransition newTransition;
            newTransition.fromState = transition.fromState + stateOffset;
            newTransition.input = transition.input;
            newTransition.toState = transition.toState + stateOffset;
            totalNFA.transitions.append(newTransition);
        }
        
        // 处理接受状态
        for (const NFAState &acceptState : nfa.acceptStates) {
            totalNFA.acceptStates.insert(acceptState + stateOffset);
            // 处理接受状态到正则表达式索引的映射
            if (nfa.acceptStateToRegexIndex.contains(acceptState)) {
                int regexIndex = nfa.acceptStateToRegexIndex[acceptState];
                totalNFA.acceptStateToRegexIndex[acceptState + stateOffset] = regexIndex;
            }
        }
        
        // 处理字母表
        totalNFA.alphabet.unite(nfa.alphabet);
        
        // 记录偏移后的起始状态
        allStartStates.append(nfa.startState + stateOffset);
        
        // 更新状态偏移量
        stateOffset += nfa.states.size();
    }
    
    // 创建总起始状态
    NFAState totalStartState = stateOffset;
    totalNFA.startState = totalStartState;
    totalNFA.states.append(totalStartState);
    
    // 从总起始状态添加ε转移到每个NFA的起始状态
    for (const NFAState &startState : allStartStates) {
        NFATransition transition;
        transition.fromState = totalStartState;
        transition.input = "#"; // ε转移
        transition.toState = startState;
        totalNFA.transitions.append(transition);
    }
    
    // 构建总NFA的邻接表
    totalNFA.transitionTable.clear();
    for (const NFATransition &transition : totalNFA.transitions) {
        totalNFA.transitionTable[transition.fromState][transition.input].insert(transition.toState);
    }
    
    qDebug() << "=== 合并NFA完成 ===";
    qDebug() << "总NFA状态数：" << totalNFA.states.size();
    qDebug() << "总NFA转移数：" << totalNFA.transitions.size();
    qDebug() << "总NFA接受状态数：" << totalNFA.acceptStates.size();
    
    // 将NFA转换为DFA
    DFABuilder dfaBuilder;
    DFA totalDFA = dfaBuilder.convertNFAToDFA(totalNFA);
    
    if (totalDFA.states.isEmpty()) {
        qDebug() << "无法将NFA转换为DFA";
        return 1;
    }
    
    qDebug() << "=== NFA转换为DFA完成 ===";
    qDebug() << "DFA状态数：" << totalDFA.states.size();
    qDebug() << "DFA转移数：" << totalDFA.transitions.size();
    qDebug() << "DFA接受状态数：" << totalDFA.acceptStates.size();
    
    // 最小化DFA
    DFAMinimizer dfaMinimizer;
    DFA minimizedDFA = dfaMinimizer.minimizeDFA(totalDFA);
    
    if (minimizedDFA.states.isEmpty()) {
        qDebug() << "无法最小化DFA";
        return 1;
    }
    
    qDebug() << "=== DFA最小化完成 ===";
    qDebug() << "最小化DFA状态数：" << minimizedDFA.states.size();
    qDebug() << "最小化DFA转移数：" << minimizedDFA.transitions.size();
    qDebug() << "最小化DFA接受状态数：" << minimizedDFA.acceptStates.size();
    
    // 创建词法分析器生成器
    LexerGenerator generator;
    
    // 生成状态转移法代码
    qDebug() << "=== 生成状态转移法代码 ===";
    QString stateCode = generator.generateLexer(regexItems, minimizedDFA, GenerationMethod::STATE_TRANSITION);
    
    if (stateCode.isEmpty()) {
        qDebug() << "生成词法分析器代码失败：" << generator.getErrorMessage();
        return 1;
    }
    
    // 检查生成的代码是否包含所有必要的接受状态代码
    qDebug() << "=== 检查生成的接受状态映射 ===";
    
    bool has100 = stateCode.contains("100");
    bool has101 = stateCode.contains("101");
    bool has102 = stateCode.contains("102");
    bool has103 = stateCode.contains("103");
    bool has200 = stateCode.contains("200");
    
    qDebug() << "包含 100 (identifier)：" << has100;
    qDebug() << "包含 101 (number)：" << has101;
    qDebug() << "包含 102 (comment)：" << has102;
    qDebug() << "包含 103 (special)：" << has103;
    qDebug() << "包含 200 (keyword)：" << has200;
    
    // 检查接受状态数组
    qDebug() << "=== 检查接受状态数组 ===";
    if (stateCode.contains("acceptTokens")) {
        int startPos = stateCode.indexOf("acceptTokens");
        int endPos = stateCode.indexOf("};", startPos);
        if (startPos != -1 && endPos != -1) {
            QString acceptTokensArray = stateCode.mid(startPos, endPos - startPos + 2);
            qDebug() << "接受状态数组：";
            qDebug().noquote() << acceptTokensArray;
        }
    }
    
    // 保存生成的代码到文件以便检查
    QFile outputFile("test/lexer.cpp");
    if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&outputFile);
        out << stateCode;
        outputFile.close();
        qDebug() << "生成的代码已保存到 test/lexer.cpp";
    }
    
    // 验证所有必要的代码都已包含
    bool allCodesPresent = has100 && has101 && has102 && has103 && has200;
    qDebug() << "=== 验证结果 ===";
    qDebug() << "所有必要的代码都已包含：" << allCodesPresent;
    
    if (allCodesPresent) {
        qDebug() << "修复成功！生成的接受状态映射包含所有必要的代码。";
    } else {
        qDebug() << "修复失败！生成的接受状态映射缺少某些代码。";
    }
    
    return 0;
}
