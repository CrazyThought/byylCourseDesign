#ifndef TASK1WINDOW_H
#define TASK1WINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include "regexprocessor.h"
#include "nfabuilder.h"
#include "dfabuilder.h"
#include "dfaminimizer.h"
#include "lexergenerator.h"
#include "lexertester.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Task1Window; }
QT_END_NAMESPACE

class Task1Window : public QMainWindow
{
    Q_OBJECT

public:
    Task1Window(QWidget *parent = nullptr);
    ~Task1Window();

private slots:
    // 正则表达式模块
    void on_btnOpenRegex_clicked();
    void on_btnSaveRegex_clicked();
    void on_btnParseRegex_clicked();
    void on_actionOpenRegex_triggered();
    void on_actionSaveRegex_triggered();
    void on_actionExit_triggered();

    // NFA模块
    void on_btnGenerateNFA_clicked();
    void on_btnRefreshNFA_clicked();

    // DFA模块
    void on_btnGenerateDFA_clicked();
    void on_btnRefreshDFA_clicked();

    // 最小化DFA模块
    void on_btnMinimizeDFA_clicked();
    void on_btnRefreshMinDFA_clicked();

    // 词法分析器生成模块
    void on_btnGenerateLexer_clicked();
    void on_btnSaveLexer_clicked();

    // 测试模块
    void on_btnOpenTestFile_clicked();
    void on_btnRunLexer_clicked();
    void on_btnSaveTestOutput_clicked();

    // 正则表达式选择切换
    void on_comboBoxNFA_currentIndexChanged(const QString &arg1);
    void on_comboBoxDFA_currentIndexChanged(const QString &arg1);
    void on_comboBoxMinDFA_currentIndexChanged(const QString &arg1);

private:
    // 初始化UI组件
    void initializeUI();

    // 更新正则表达式下拉列表
    void updateRegexComboBox();

    // 更新NFA显示
    void updateNFADisplay();

    // 更新DFA显示
    void updateDFADisplay();

    // 更新最小化DFA显示
    void updateMinimizedDFADisplay();

    // 显示NFA图表
    void displayNFA(const NFA &nfa, QTableWidget *table);

    // 显示DFA图表
    void displayDFA(const DFA &dfa, QTableWidget *table);

    // 显示最小化DFA图表
    void displayMinimizedDFA(const DFA &minimizedDFA, QTableWidget *table);

    // 创建动态表格
    void createDynamicTables();
    
    // 清理动态表格
    void cleanupDynamicTables();

    // 绘制NFA图表
    void drawNFAGraph(const NFA &nfa, QTableWidget *table);

    // 绘制DFA图表
    void drawDFAGraph(const DFA &dfa, QTableWidget *table);

    // 绘制最小化DFA图表
    void drawMinimizedDFAGraph(const DFA &minimizedDFA, QTableWidget *table);
    
    // 初始化测试输出表格
    void initTableWidget(QTableWidget *table, const QStringList &headers);
    
    // 合并多个NFA
    NFA mergeNFAs(const QList<NFA> &nfAs);
    
    // 显示词法分析结果
    void displayLexicalResults(const QList<LexicalResult> &results);
    
    // 添加表格行
    void addTableRow(QTableWidget *table, const QStringList &data);
    
    // 清空表格
    void clearTable(QTableWidget *table);
    
    // 处理正则表达式引用
    QMap<QString, QString> processRegexReferences(const QList<RegexItem> &regexItems);
    
    // 合并转换
    QMap<QString, QList<QString>> mergeTransitionsByRef(const QSet<QString> &transitions, const QMap<QString, QString> &charToRefMap);
    
    // 更新正则表达式下拉列表，不改变当前选择
    void updateRegexComboBoxWithoutChangingSelection(const QString &currentRegexName, bool isTotalView);
    
    // 重置图表显示状态
    void resetChartDisplayState();

private:
    Ui::Task1Window *ui;

    // 正则表达式处理器
    RegexProcessor m_regexProcessor;

    // NFA构建器
    NFABuilder m_nfaBuilder;

    // DFA构建器
    DFABuilder m_dfaBuilder;

    // DFA最小化器
    DFAMinimizer m_dfaMinimizer;

    // 词法分析器生成器
    LexerGenerator m_lexerGenerator;

    // 词法分析器测试器
    LexerTester m_lexerTester;

    // 正则表达式项列表
    QList<RegexItem> m_currentRegexItems;

    // NFA映射：正则表达式名称 -> NFA
    QMap<QString, NFA> m_nfaMap;

    // 总NFA
    NFA m_totalNFA;

    // DFA映射：正则表达式名称 -> DFA
    QMap<QString, DFA> m_dfaMap;

    // 总DFA
    DFA m_totalDFA;

    // 最小化DFA映射：正则表达式名称 -> 最小化DFA
    QMap<QString, DFA> m_minimizedDfaMap;

    // 总最小化DFA
    DFA m_totalMinimizedDFA;

    // 当前选中的正则表达式名称
    QString m_currentRegexName;

    // 是否为总表视图
    bool m_isTotalView;

    // 动态表格
    QTableWidget *m_dynamicTableNFA;
    QTableWidget *m_dynamicTableDFA;
    QTableWidget *m_dynamicTableMinDFA;

    // 词法分析结果
    QList<LexicalResult> m_currentLexicalResults;
};

#endif // TASK1WINDOW_H
