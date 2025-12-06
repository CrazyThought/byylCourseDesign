#ifndef DOTGENERATOR_H
#define DOTGENERATOR_H

#include <QString>
#include <QList>
#include <QSet>
#include <QMap>
#include "nfabuilder.h"
#include "dfabuilder.h"

class DotGenerator
{
public:
    DotGenerator();
    ~DotGenerator();
    
    // 生成NFA的DOT格式字符串
    QString generateNFA(const NFA &nfa, const QString &graphName = "NFA");
    
    // 生成DFA的DOT格式字符串
    QString generateDFA(const DFA &dfa, const QString &graphName = "DFA");
    
    // 生成最小化DFA的DOT格式字符串
    QString generateMinimizedDFA(const DFA &minimizedDFA, const QString &graphName = "MinimizedDFA");
    
    // 将DOT字符串保存到文件
    bool saveDotToFile(const QString &dotContent, const QString &fileName);
    
    // 获取错误信息
    QString getErrorMessage() const;
    
private:
    // 格式化状态名称
    QString formatStateName(int state);
    
    // 格式化输入字符（处理特殊字符）
    QString formatInput(const QString &input);
    
    // 生成状态节点定义
    QString generateStates(const QList<int> &states, const QSet<int> &acceptStates, int startState);
    
    // 生成转换关系
    QString generateTransitions(const QList<NFATransition> &transitions);
    QString generateDFATransitions(const QList<DFATransition> &transitions);
    
    // 生成图属性
    QString generateGraphAttributes(const QString &graphName);
    
    // 错误信息
    QString m_errorMessage;
};

#endif // DOTGENERATOR_H