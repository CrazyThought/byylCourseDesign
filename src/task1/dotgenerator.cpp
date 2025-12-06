#include "task1/dotgenerator.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

DotGenerator::DotGenerator()
{
}

DotGenerator::~DotGenerator()
{
}

QString DotGenerator::generateNFA(const NFA &nfa, const QString &graphName)
{
    m_errorMessage.clear();
    
    if (nfa.states.isEmpty()) {
        m_errorMessage = "NFA状态为空";
        return "";
    }
    
    QString dotContent;
    QTextStream stream(&dotContent);
    
    // 生成图定义
    stream << "digraph " << graphName << " {\n";
    stream << generateGraphAttributes(graphName);
    
    // 生成状态定义
    stream << generateStates(nfa.states, nfa.acceptStates, nfa.startState);
    
    // 生成转换关系
    stream << generateTransitions(nfa.transitions);
    
    stream << "}\n";
    
    return dotContent;
}

QString DotGenerator::generateDFA(const DFA &dfa, const QString &graphName)
{
    m_errorMessage.clear();
    
    if (dfa.states.isEmpty()) {
        m_errorMessage = "DFA状态为空";
        return "";
    }
    
    QString dotContent;
    QTextStream stream(&dotContent);
    
    // 生成图定义
    stream << "digraph " << graphName << " {\n";
    stream << generateGraphAttributes(graphName);
    
    // 生成状态定义
    stream << generateStates(dfa.states, dfa.acceptStates, dfa.startState);
    
    // 生成转换关系
    stream << generateDFATransitions(dfa.transitions);
    
    stream << "}\n";
    
    return dotContent;
}

QString DotGenerator::generateMinimizedDFA(const DFA &minimizedDFA, const QString &graphName)
{
    return generateDFA(minimizedDFA, graphName);
}

bool DotGenerator::saveDotToFile(const QString &dotContent, const QString &fileName)
{
    if (dotContent.isEmpty()) {
        m_errorMessage = "DOT内容为空";
        return false;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_errorMessage = "无法打开文件: " + file.errorString();
        return false;
    }
    
    QTextStream out(&file);
    out << dotContent;
    file.close();
    
    return true;
}

QString DotGenerator::getErrorMessage() const
{
    return m_errorMessage;
}

QString DotGenerator::formatStateName(int state)
{
    return QString("q%1").arg(state);
}

QString DotGenerator::formatInput(const QString &input)
{
    // 处理特殊字符
    QString result = input;
    result.replace("\\\"", "\\\\\\\""); // 转义双引号
    result.replace("\\n", "\\\\n"); // 转义换行符
    result.replace("\\t", "\\\\t"); // 转义制表符
    
    // 如果包含空格或特殊字符，用双引号包围
    if (result.contains(' ') || result.contains('\\') || result.contains('\"')) {
        result = "\\\"" + result + "\\\"";
    }
    
    return result;
}

QString DotGenerator::generateStates(const QList<int> &states, const QSet<int> &acceptStates, int startState)
{
    QString result;
    QTextStream stream(&result);
    
    // 添加起始状态的特殊标记
    stream << "    // 起始状态\n";
    stream << "    node [shape=circle];\n";
    stream << "    \"\" [shape=none, width=0, height=0];\n";
    stream << "    \"\" -> " << formatStateName(startState) << ";\n";
    
    // 添加普通状态
    stream << "    // 普通状态\n";
    for (int state : states) {
        if (state == startState) continue; // 起始状态已经处理过
        
        if (acceptStates.contains(state)) {
            // 接受状态（双圈）
            stream << "    " << formatStateName(state) << " [shape=doublecircle];\n";
        } else {
            // 普通状态（单圈）
            stream << "    " << formatStateName(state) << " [shape=circle];\n";
        }
    }
    
    // 添加起始状态的接受状态检查（如果起始状态也是接受状态）
    if (acceptStates.contains(startState)) {
        stream << "    " << formatStateName(startState) << " [shape=doublecircle];\n";
    }
    
    return result;
}

QString DotGenerator::generateTransitions(const QList<NFATransition> &transitions)
{
    QString result;
    QTextStream stream(&result);
    
    stream << "    // 转换关系\n";
    
    // 按源状态分组转换，以便合并相同源状态和目标状态的转换
    QMap<QPair<int, int>, QSet<QString>> groupedTransitions;
    
    for (const NFATransition &transition : transitions) {
        QPair<int, int> key(transition.fromState, transition.toState);
        groupedTransitions[key].insert(transition.input);
    }
    
    // 生成转换
    for (auto it = groupedTransitions.begin(); it != groupedTransitions.end(); ++it) {
        QPair<int, int> states = it.key();
        QSet<QString> inputs = it.value();
        
        QStringList formattedInputs;
        for (const QString &input : inputs) {
            formattedInputs << formatInput(input);
        }
        
        QString label = formattedInputs.join(", ");
        
        stream << "    " << formatStateName(states.first) 
               << " -> " << formatStateName(states.second)
               << " [label=\"" << label << "\"];\n";
    }
    
    return result;
}

QString DotGenerator::generateDFATransitions(const QList<DFATransition> &transitions)
{
    QString result;
    QTextStream stream(&result);
    
    stream << "    // 转换关系\n";
    
    // 按源状态分组转换，以便合并相同源状态和目标状态的转换
    QMap<QPair<int, int>, QSet<QString>> groupedTransitions;
    
    for (const DFATransition &transition : transitions) {
        QPair<int, int> key(transition.fromState, transition.toState);
        groupedTransitions[key].insert(transition.input);
    }
    
    // 生成转换
    for (auto it = groupedTransitions.begin(); it != groupedTransitions.end(); ++it) {
        QPair<int, int> states = it.key();
        QSet<QString> inputs = it.value();
        
        QStringList formattedInputs;
        for (const QString &input : inputs) {
            formattedInputs << formatInput(input);
        }
        
        QString label = formattedInputs.join(", ");
        
        stream << "    " << formatStateName(states.first) 
               << " -> " << formatStateName(states.second)
               << " [label=\"" << label << "\"];\n";
    }
    
    return result;
}

QString DotGenerator::generateGraphAttributes(const QString &graphName)
{
    QString result;
    QTextStream stream(&result);
    
    stream << "    // 图属性\n";
    stream << "    rankdir=LR;\n";  // 从左到右布局
    stream << "    size=\"8,5\";\n";
    stream << "    node [fontname=\"Arial\", fontsize=12];\n";
    stream << "    edge [fontname=\"Arial\", fontsize=10];\n";
    stream << "    label=\"" << graphName << "\";\n";
    stream << "    labelloc=t;\n";  // 标题在顶部
    
    return result;
}