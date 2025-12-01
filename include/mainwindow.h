#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include "regexprocessor.h"
#include "nfabuilder.h"
#include "dfabuilder.h"
#include "dfaminimizer.h"
#include "lexergenerator.h"
#include "lexertester.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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

    // DFA模块
    void on_btnGenerateDFA_clicked();

    // 最小化DFA模块
    void on_btnMinimizeDFA_clicked();

    // 词法分析器生成模块
    void on_btnGenerateLexer_clicked();
    void on_btnSaveLexer_clicked();

    // 测试模块
    void on_btnOpenTestFile_clicked();
    void on_btnRunLexer_clicked();
    void on_btnSaveTestOutput_clicked();

private:
    Ui::MainWindow *ui;
    RegexProcessor m_regexProcessor; // 正则表达式处理器
    NFABuilder m_nfaBuilder; // NFA构建器
    DFABuilder m_dfaBuilder; // DFA构建器
    DFAMinimizer m_dfaMinimizer; // DFA最小化器
    LexerGenerator m_lexerGenerator; // 词法分析器生成器
    LexerTester m_lexerTester; // 词法分析器测试器
    
    // 多个NFA和DFA的管理
    QList<RegexItem> m_currentRegexItems; // 当前正则表达式列表
    QMap<QString, NFA> m_nfaMap; // 存储所有NFA，键为正则表达式名称
    QMap<QString, DFA> m_dfaMap; // 存储所有DFA，键为正则表达式名称
    QMap<QString, DFA> m_minimizedDfaMap; // 存储所有最小化DFA，键为正则表达式名称
    
    // 总表
    NFA m_totalNFA; // 合并后的总NFA
    DFA m_totalDFA; // 合并后的总DFA
    DFA m_totalMinimizedDFA; // 合并后的总最小化DFA
    
    QString m_currentRegexName; // 当前选中的正则表达式名称
    bool m_isTotalView; // 是否显示总表
    
    QList<LexicalResult> m_currentLexicalResults; // 当前词法分析结果
    
    // 辅助函数
    void initTableWidget(QTableWidget *table, const QStringList &headers);
    void addTableRow(QTableWidget *table, const QStringList &data);
    void clearTable(QTableWidget *table);
    void displayNFA(const NFA &nfa); // 显示NFA到表格
    void displayDFA(const DFA &dfa); // 显示DFA到表格
    void displayMinimizedDFA(const DFA &dfa); // 显示最小化DFA到表格
    void displayLexicalResults(const QList<LexicalResult> &results); // 显示词法分析结果
    void updateRegexComboBox(); // 更新正则表达式下拉列表
    void updateNFADisplay(); // 更新NFA显示
    void updateDFADisplay(); // 更新DFA显示
    void updateMinimizedDFADisplay(); // 更新最小化DFA显示
    NFA mergeNFAs(const QList<NFA> &nfAs); // 合并多个NFA
    
private slots:
    // 新增：正则表达式选择切换
    void on_comboBoxRegex_currentIndexChanged(const QString &arg1);
};
#endif // MAINWINDOW_H
