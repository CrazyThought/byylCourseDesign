/*
 * @file task1window.h
 * @id task1window-h
 * @brief 实现任务1的主窗口，包含正则表达式处理、NFA/DFA转换、DFA最小化、词法分析器生成和测试功能
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#ifndef TASK1WINDOW_H
#define TASK1WINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QTextEdit>
#include "regexprocessor.h"
#include "nfabuilder.h"
#include "dfabuilder.h"
#include "dfaminimizer.h"
#include "lexergenerator.h"
#include "lexertester.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Task1Window; }
QT_END_NAMESPACE

/**
 * @brief 任务1窗口类
 * 
 * 实现任务1的主界面，包含正则表达式处理、NFA/DFA转换、DFA最小化、词法分析器生成和测试功能
 */
class Task1Window : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * 
     * @param parent 父窗口指针，默认为nullptr
     */
    Task1Window(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~Task1Window();

private slots:
    // 正则表达式模块
    /**
     * @brief 打开正则表达式文件按钮点击事件
     * 
     * 处理打开正则表达式文件按钮的点击事件
     */
    void on_btnOpenRegex_clicked();
    
    /**
     * @brief 保存正则表达式文件按钮点击事件
     * 
     * 处理保存正则表达式文件按钮的点击事件
     */
    void on_btnSaveRegex_clicked();
    
    /**
     * @brief 解析正则表达式按钮点击事件
     * 
     * 处理解析正则表达式按钮的点击事件
     */
    void on_btnParseRegex_clicked();
    
    /**
     * @brief 打开正则表达式菜单项触发事件
     * 
     * 处理打开正则表达式菜单项的触发事件
     */
    void on_actionOpenRegex_triggered();
    
    /**
     * @brief 保存正则表达式菜单项触发事件
     * 
     * 处理保存正则表达式菜单项的触发事件
     */
    void on_actionSaveRegex_triggered();
    
    /**
     * @brief 退出菜单项触发事件
     * 
     * 处理退出菜单项的触发事件
     */
    void on_actionExit_triggered();

    // NFA模块
    /**
     * @brief 生成NFA按钮点击事件
     * 
     * 处理生成NFA按钮的点击事件
     */
    void on_btnGenerateNFA_clicked();
    
    /**
     * @brief 刷新NFA显示按钮点击事件
     * 
     * 处理刷新NFA显示按钮的点击事件
     */
    void on_btnRefreshNFA_clicked();

    // DFA模块
    /**
     * @brief 生成DFA按钮点击事件
     * 
     * 处理生成DFA按钮的点击事件
     */
    void on_btnGenerateDFA_clicked();
    
    /**
     * @brief 刷新DFA显示按钮点击事件
     * 
     * 处理刷新DFA显示按钮的点击事件
     */
    void on_btnRefreshDFA_clicked();

    // 最小化DFA模块
    /**
     * @brief 最小化DFA按钮点击事件
     * 
     * 处理最小化DFA按钮的点击事件
     */
    void on_btnMinimizeDFA_clicked();
    
    /**
     * @brief 刷新最小化DFA显示按钮点击事件
     * 
     * 处理刷新最小化DFA显示按钮的点击事件
     */
    void on_btnRefreshMinDFA_clicked();

    // 词法分析器生成模块
    /**
     * @brief 生成词法分析器按钮点击事件
     * 
     * 处理生成词法分析器按钮的点击事件
     */
    void on_btnGenerateLexer_clicked();
    
    /**
     * @brief 保存词法分析器按钮点击事件
     * 
     * 处理保存词法分析器按钮的点击事件
     */
    void on_btnSaveLexer_clicked();

    // 测试模块
    /**
     * @brief 打开测试文件按钮点击事件
     * 
     * 处理打开测试文件按钮的点击事件
     */
    void on_btnOpenTestFile_clicked();
    
    /**
     * @brief 运行词法分析器按钮点击事件
     * 
     * 处理运行词法分析器按钮的点击事件
     */
    void on_btnRunLexer_clicked();
    
    /**
     * @brief 保存测试输出按钮点击事件
     * 
     * 处理保存测试输出按钮的点击事件
     */
    void on_btnSaveTestOutput_clicked();

    // 正则表达式选择切换
    /**
     * @brief NFA下拉列表选择变化事件
     * 
     * 处理NFA下拉列表选择变化事件
     * 
     * @param arg1 选中的正则表达式名称
     */
    void on_comboBoxNFA_currentIndexChanged(const QString &arg1);
    
    /**
     * @brief DFA下拉列表选择变化事件
     * 
     * 处理DFA下拉列表选择变化事件
     * 
     * @param arg1 选中的正则表达式名称
     */
    void on_comboBoxDFA_currentIndexChanged(const QString &arg1);
    
    /**
     * @brief 最小化DFA下拉列表选择变化事件
     * 
     * 处理最小化DFA下拉列表选择变化事件
     * 
     * @param arg1 选中的正则表达式名称
     */
    void on_comboBoxMinDFA_currentIndexChanged(const QString &arg1);

private:
    /**
     * @brief 初始化UI组件
     * 
     * 初始化窗口的UI组件
     */
    void initializeUI();

    /**
     * @brief 更新正则表达式下拉列表
     * 
     * 更新所有正则表达式下拉列表的内容
     */
    void updateRegexComboBox();

    /**
     * @brief 更新NFA显示
     * 
     * 更新NFA图表的显示
     */
    void updateNFADisplay();

    /**
     * @brief 更新DFA显示
     * 
     * 更新DFA图表的显示
     */
    void updateDFADisplay();

    /**
     * @brief 更新最小化DFA显示
     * 
     * 更新最小化DFA图表的显示
     */
    void updateMinimizedDFADisplay();

    /**
     * @brief 显示NFA图表
     * 
     * 在指定表格中显示NFA图表
     * 
     * @param nfa 要显示的NFA
     * @param table 显示NFA的表格
     */
    void displayNFA(const NFA &nfa, QTableWidget *table);

    /**
     * @brief 显示DFA图表
     * 
     * 在指定表格中显示DFA图表
     * 
     * @param dfa 要显示的DFA
     * @param table 显示DFA的表格
     */
    void displayDFA(const DFA &dfa, QTableWidget *table);

    /**
     * @brief 显示最小化DFA图表
     * 
     * 在指定表格中显示最小化DFA图表
     * 
     * @param minimizedDFA 要显示的最小化DFA
     * @param table 显示最小化DFA的表格
     */
    void displayMinimizedDFA(const DFA &minimizedDFA, QTableWidget *table);

    /**
     * @brief 创建动态表格
     * 
     * 创建用于显示NFA、DFA和最小化DFA的动态表格
     */
    void createDynamicTables();
    
    /**
     * @brief 清理动态表格
     * 
     * 清理动态创建的表格资源
     */
    void cleanupDynamicTables();

    /**
     * @brief 绘制NFA图表
     * 
     * 在指定表格中绘制NFA图表
     * 
     * @param nfa 要绘制的NFA
     * @param table 绘制NFA的表格
     */
    void drawNFAGraph(const NFA &nfa, QTableWidget *table);

    /**
     * @brief 绘制DFA图表
     * 
     * 在指定表格中绘制DFA图表
     * 
     * @param dfa 要绘制的DFA
     * @param table 绘制DFA的表格
     */
    void drawDFAGraph(const DFA &dfa, QTableWidget *table);

    /**
     * @brief 绘制最小化DFA图表
     * 
     * 在指定表格中绘制最小化DFA图表
     * 
     * @param minimizedDFA 要绘制的最小化DFA
     * @param table 绘制最小化DFA的表格
     */
    void drawMinimizedDFAGraph(const DFA &minimizedDFA, QTableWidget *table);
    
    /**
     * @brief 初始化测试输出表格
     * 
     * 初始化测试输出表格的列头和属性
     * 
     * @param table 要初始化的表格
     * @param headers 表格列头
     */
    void initTableWidget(QTableWidget *table, const QStringList &headers);
    
    /**
     * @brief 合并多个NFA
     * 
     * 将多个NFA合并为一个
     * 
     * @param nfAs 要合并的NFA列表
     * @return NFA 合并后的NFA
     */
    NFA mergeNFAs(const QList<NFA> &nfAs);
    
    /**
     * @brief 显示词法分析结果
     * 
     * 在表格中显示词法分析结果
     * 
     * @param results 词法分析结果列表
     */
    void displayLexicalResults(const QList<LexicalResult> &results);
    
    /**
     * @brief 添加表格行
     * 
     * 向表格中添加一行数据
     * 
     * @param table 要添加数据的表格
     * @param data 行数据列表
     */
    void addTableRow(QTableWidget *table, const QStringList &data);
    
    /**
     * @brief 清空表格
     * 
     * 清空表格中的所有数据
     * 
     * @param table 要清空的表格
     */
    void clearTable(QTableWidget *table);
    
    /**
     * @brief 处理正则表达式引用
     * 
     * 处理正则表达式中的引用关系
     * 
     * @param regexItems 正则表达式项列表
     * @return QMap<QString, QString> 字符到引用的映射
     */
    QMap<QString, QString> processRegexReferences(const QList<RegexItem> &regexItems);
    
    /**
     * @brief 合并转换
     * 
     * 根据字符引用映射合并转换
     * 
     * @param transitions 转换集合
     * @param charToRefMap 字符到引用的映射
     * @return QMap<QString, QList<QString>> 合并后的转换映射
     */
    QMap<QString, QList<QString>> mergeTransitionsByRef(const QSet<QString> &transitions, const QMap<QString, QString> &charToRefMap);
    
    /**
     * @brief 更新正则表达式下拉列表，不改变当前选择
     * 
     * 更新正则表达式下拉列表，但保持当前选择不变
     * 
     * @param currentRegexName 当前选中的正则表达式名称
     * @param isTotalView 是否为总表视图
     */
    void updateRegexComboBoxWithoutChangingSelection(const QString &currentRegexName, bool isTotalView);
    
    /**
     * @brief 重置图表显示状态
     * 
     * 重置所有图表的显示状态
     */
    void resetChartDisplayState();

private:
    Ui::Task1Window *ui;                     ///< UI界面指针

    QTextEdit *m_textEditTestOutput;         ///< 测试输出文本框

    RegexProcessor m_regexProcessor;         ///< 正则表达式处理器
    NFABuilder m_nfaBuilder;                 ///< NFA构建器
    DFABuilder m_dfaBuilder;                 ///< DFA构建器
    DFAMinimizer m_dfaMinimizer;             ///< DFA最小化器
    LexerGenerator m_lexerGenerator;         ///< 词法分析器生成器
    LexerTester m_lexerTester;               ///< 词法分析器测试器

    QList<RegexItem> m_currentRegexItems;    ///< 正则表达式项列表
    QMap<QString, NFA> m_nfaMap;             ///< NFA映射：正则表达式名称 -> NFA
    NFA m_totalNFA;                          ///< 总NFA
    QMap<QString, DFA> m_dfaMap;             ///< DFA映射：正则表达式名称 -> DFA
    DFA m_totalDFA;                          ///< 总DFA
    QMap<QString, DFA> m_minimizedDfaMap;    ///< 最小化DFA映射：正则表达式名称 -> 最小化DFA
    DFA m_totalMinimizedDFA;                 ///< 总最小化DFA

    QString m_currentRegexName;              ///< 当前选中的正则表达式名称
    bool m_isTotalView;                      ///< 是否为总表视图

    QTableWidget *m_dynamicTableNFA;         ///< 动态NFA表格
    QTableWidget *m_dynamicTableDFA;         ///< 动态DFA表格
    QTableWidget *m_dynamicTableMinDFA;      ///< 动态最小化DFA表格

    QList<LexicalResult> m_currentLexicalResults; ///< 词法分析结果
};

#endif // TASK1WINDOW_H
