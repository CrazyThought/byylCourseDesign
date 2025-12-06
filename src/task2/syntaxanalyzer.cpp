#include "include/task2/syntaxanalyzer.h"
#include "include/task2/lr1table.h"
#include "include/task2/bnfparser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

// Token结构体实现
bool Token::operator==(const Token &other) const {
    return type == other.type && value == other.value;
}

QString Token::toString() const {
    return QString("%1\t%2").arg(type).arg(value);
}

// SyntaxTreeNode结构体实现
QString SyntaxTreeNode::toString(int indent) const {
    QString indentStr(indent, ' ');
    QString result = indentStr + value + "\n";
    for (const SyntaxTreeNode &child : children) {
        result += child.toString(indent + 2);
    }
    return result;
}

void SyntaxTreeNode::addChild(const SyntaxTreeNode &child) {
    children.append(child);
}

QTreeWidgetItem *SyntaxTreeNode::toTreeWidgetItem() const {
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, value);
    for (const SyntaxTreeNode &child : children) {
        item->addChild(child.toTreeWidgetItem());
    }
    return item;
}

// AnalysisStep结构体实现
QString AnalysisStep::toString() const {
    return QString("Step %1: StateStack=%2, SymbolStack=%3, Input=%4, Action=%5")
        .arg(step)
        .arg(stateStack)
        .arg(symbolStack)
        .arg(inputString)
        .arg(action);
}

// AnalysisResult结构体实现
QString AnalysisResult::toString() const {
    QString result = success ? "Success" : "Failure";
    if (!success) {
        result += ": " + errorMessage;
    }
    return result;
}

// SyntaxAnalyzer类实现
SyntaxAnalyzer::SyntaxAnalyzer()
    : m_lr1table(nullptr),
      m_parser(nullptr),
      m_currentTokenIndex(0),
      m_endMarker("$")
{
    m_result.success = false;
}

SyntaxAnalyzer::~SyntaxAnalyzer() {
    clear();
}

void SyntaxAnalyzer::setLR1Table(LR1Table *lr1table) {
    m_lr1table = lr1table;
}

void SyntaxAnalyzer::setParser(BNFParser *parser) {
    m_parser = parser;
}

bool SyntaxAnalyzer::loadTokensFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_errorMessage = QString("Failed to open token file: %1").arg(filePath);
        return false;
    }

    QTextStream in(&file);
    QString tokenString = in.readAll();
    file.close();

    return loadTokensFromString(tokenString);
}

bool SyntaxAnalyzer::loadTokensFromString(const QString &tokenString) {
    m_tokens = parseTokens(tokenString);
    if (m_tokens.isEmpty()) {
        m_errorMessage = "No valid tokens found";
        return false;
    }
    return true;
}

AnalysisResult SyntaxAnalyzer::analyze() {
    m_result = AnalysisResult();
    m_result.success = false;
    m_result.tokens = m_tokens;
    m_result.steps.clear();
    m_result.syntaxTree = SyntaxTreeNode();
    m_result.syntaxTree.type = NODE_ROOT;
    m_result.syntaxTree.value = "ROOT";

    if (!m_lr1table || !m_parser) {
        m_errorMessage = "LR1 table or parser not set";
        m_result.errorMessage = m_errorMessage;
        return m_result;
    }

    if (m_tokens.isEmpty()) {
        m_errorMessage = "No tokens to analyze";
        m_result.errorMessage = m_errorMessage;
        return m_result;
    }

    initAnalysis();

    while (stepAnalysis(m_result)) {
        if (!m_result.success) {
            break;
        }
    }

    return m_result;
}

AnalysisResult SyntaxAnalyzer::getResult() const {
    return m_result;
}

void SyntaxAnalyzer::clear() {
    m_tokens.clear();
    m_errorMessage.clear();
    m_result = AnalysisResult();
    m_stateStack.clear();
    m_symbolStack.clear();
    m_currentTokenIndex = 0;
    m_nodeStack.clear();
}

QString SyntaxAnalyzer::getErrorMessage() const {
    return m_errorMessage;
}

bool SyntaxAnalyzer::saveSyntaxTreeToImage(const QString &filePath) const {
    Q_UNUSED(filePath);
    // 暂未实现
    return true;
}

bool SyntaxAnalyzer::saveStepsToFile(const QString &filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    for (const AnalysisStep &step : m_result.steps) {
        out << step.toString() << "\n";
    }

    file.close();
    return true;
}

QList<Token> SyntaxAnalyzer::getTokens() const {
    return m_tokens;
}

void SyntaxAnalyzer::setStartSymbol(const QString &startSymbol) {
    m_startSymbol = startSymbol;
}

QString SyntaxAnalyzer::getStartSymbol() const {
    return m_startSymbol;
}

void SyntaxAnalyzer::initAnalysis() {
    m_stateStack.clear();
    m_symbolStack.clear();
    m_currentTokenIndex = 0;
    m_nodeStack.clear();

    m_stateStack.push_back(0);
    m_symbolStack.push_back(m_endMarker);
}

bool SyntaxAnalyzer::stepAnalysis(AnalysisResult &result) {
    if (m_currentTokenIndex >= m_tokens.size()) {
        return false;
    }

    QString currentSymbol = getCurrentInputSymbol();
    int currentState = m_stateStack.back();
    Action action = m_lr1table->getAction(currentState, currentSymbol);

    AnalysisStep step;
    step.step = result.steps.size() + 1;
    step.stateStack = stateStackToString();
    step.symbolStack = symbolStackToString();
    step.inputString = remainingInputToString();

    switch (action.type) {
    case ACTION_ACCEPT:
        step.action = "Accept";
        result.steps.push_back(step);
        result.success = true;
        return false;
    case ACTION_SHIFT:
        step.action = QString("Shift %1").arg(action.value);
        result.steps.push_back(step);
        return handleShiftAction(action, result);
    case ACTION_REDUCE:
        step.action = QString("Reduce %1").arg(action.value);
        result.steps.push_back(step);
        return handleReduceAction(action, result);
    default: // Error
        step.action = "Error";
        result.steps.push_back(step);
        result.success = false;
        result.errorMessage = QString("Syntax error at token %1: %2, lookahead: %3")
            .arg(m_currentTokenIndex + 1)
            .arg(m_tokens[m_currentTokenIndex].value)
            .arg(currentSymbol);
        return false;
    }
}

bool SyntaxAnalyzer::handleShiftAction(const Action &action, AnalysisResult &result) {
    QString currentSymbol = getCurrentInputSymbol();
    m_stateStack.push_back(action.value);
    m_symbolStack.push_back(currentSymbol);

    // 创建词法单元节点并添加到语法树
    SyntaxTreeNode *tokenNode = new SyntaxTreeNode();
    tokenNode->type = NODE_TERMINAL;
    tokenNode->value = currentSymbol;
    m_nodeStack.push_back(tokenNode);

    nextInputSymbol();
    return true;
}

bool SyntaxAnalyzer::handleReduceAction(const Action &action, AnalysisResult &result) {
    int productionIndex = action.value;
    const QList<Production> &productions = m_parser->getProductions();
    if (productionIndex < 0 || productionIndex >= productions.size()) {
        result.success = false;
        result.errorMessage = QString("Invalid production index: %1").arg(productionIndex);
        return false;
    }

    const Production &production = productions[productionIndex];
    // 获取产生式的候选式（这里只处理第一个候选式，因为BNFParser将一个产生式的所有候选式放在candidates列表中）
    if (production.candidates.isEmpty()) {
        result.success = false;
        result.errorMessage = QString("Invalid production: no candidates");
        return false;
    }
    const QStringList &right = production.candidates[0];
    int rightSize = right.size();

    // 从栈中弹出相应数量的符号和状态
    QList<QString> poppedSymbols;
    QList<SyntaxTreeNode*> poppedNodes;
    for (int i = 0; i < rightSize; ++i) {
        if (!m_symbolStack.isEmpty()) {
            poppedSymbols.prepend(m_symbolStack.takeLast());
        }
        if (!m_nodeStack.isEmpty()) {
            poppedNodes.prepend(m_nodeStack.takeLast());
        }
        if (!m_stateStack.isEmpty()) {
            m_stateStack.removeLast();
        }
    }

    // 获取非终结符
    QString nonTerminal = production.nonTerminal;
    m_symbolStack.push_back(nonTerminal);

    // 处理GOTO动作
    int currentState = m_stateStack.back();
    int gotoState = m_lr1table->getGoto(currentState, nonTerminal);
    if (gotoState == -1) {
        result.success = false;
        result.errorMessage = QString("Goto error: no transition for state %1 and symbol %2").arg(currentState).arg(nonTerminal);
        return false;
    }
    m_stateStack.push_back(gotoState);

    // 创建非终结符节点并添加子节点
    SyntaxTreeNode *nonTermNode = new SyntaxTreeNode();
    nonTermNode->type = NODE_NON_TERMINAL;
    nonTermNode->value = nonTerminal;
    nonTermNode->productionIndex = productionIndex;
    for (SyntaxTreeNode *node : poppedNodes) {
        nonTermNode->addChild(*node);
    }
    m_nodeStack.push_back(nonTermNode);

    // 更新语法树
    updateSyntaxTree(result, productionIndex);

    return true;
}

bool SyntaxAnalyzer::handleAcceptAction(AnalysisResult &result) {
    result.success = true;
    result.errorMessage.clear();
    return false;
}

bool SyntaxAnalyzer::handleGotoAction(const QString &nonTerminal, AnalysisResult &result) {
    Q_UNUSED(nonTerminal);
    Q_UNUSED(result);
    // GOTO动作在handleReduceAction中处理
    return true;
}

void SyntaxAnalyzer::updateSyntaxTree(AnalysisResult &result, int productionIndex) {
    Q_UNUSED(productionIndex);
    // 如果是最顶层的归约，更新根节点的子节点
    if (m_symbolStack.size() == 2 && m_symbolStack.back() == m_startSymbol) {
        if (!m_nodeStack.isEmpty()) {
            result.syntaxTree.children.clear();
            result.syntaxTree.addChild(*m_nodeStack.back());
        }
    }
}

QList<Token> SyntaxAnalyzer::parseTokens(const QString &tokenString) const {
    QList<Token> tokens;
    QStringList lines = tokenString.split("\n", Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.isEmpty()) {
            continue;
        }

        QStringList parts = trimmedLine.split("\t", Qt::SkipEmptyParts);
        if (parts.size() < 2) {
            continue;
        }

        Token token;
        token.type = parts[0];
        token.value = parts[1];
        token.line = parts.size() > 2 ? parts[2].toInt() : 0;
        token.column = parts.size() > 3 ? parts[3].toInt() : 0;
        tokens.append(token);
    }

    return tokens;
}

QString SyntaxAnalyzer::getCurrentInputSymbol() const {
    if (m_currentTokenIndex >= m_tokens.size()) {
        return m_endMarker;
    }
    return m_tokens[m_currentTokenIndex].value;
}

void SyntaxAnalyzer::nextInputSymbol() {
    m_currentTokenIndex++;
}

QString SyntaxAnalyzer::stateStackToString() const {
    QString result = "[";
    for (int state : m_stateStack) {
        result += QString::number(state) + ",";
    }
    if (result.endsWith(",")) {
        result.chop(1);
    }
    result += "]";
    return result;
}

QString SyntaxAnalyzer::symbolStackToString() const {
    QString result = "[";
    for (const QString &symbol : m_symbolStack) {
        result += symbol + ",";
    }
    if (result.endsWith(",")) {
        result.chop(1);
    }
    result += "]";
    return result;
}

QString SyntaxAnalyzer::remainingInputToString() const {
    QString result;
    for (int i = m_currentTokenIndex; i < m_tokens.size(); ++i) {
        result += m_tokens[i].value + " ";
    }
    result += m_endMarker;
    return result;
}
