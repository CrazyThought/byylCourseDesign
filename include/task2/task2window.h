/*
 * @file task2window.h
 * @id task2window-h
 * @brief 任务2窗口头文件，用于语法分析器的图形界面
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#ifndef TASK2WINDOW_H
#define TASK2WINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QListWidget>

// 添加必要的头文件包含
#include "task2/Grammar.h"
#include "task2/GrammarParser.h"
#include "task2/LL1.h"
#include "task2/LR0.h"
#include "task2/SLR.h"
#include "task2/LR1.h"
#include "task2/LR1Parser.h"
#include "task2/SyntaxParser.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Task2Window; }
QT_END_NAMESPACE

/**
 * @brief 任务2窗口类
 * @details 语法分析器的主窗口，包含文法处理、First/Follow集合计算、LR(0)DFA生成、SLR(1)检查、LR(1)DFA生成、LR(1)分析表生成和语法分析功能
 */
class Task2Window : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     * @details 初始化任务2窗口
     */
    Task2Window(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     * @details 释放窗口资源
     */
    ~Task2Window();

private slots:
    // 文件操作相关槽函数
    void on_actionExit_triggered();      ///< 退出菜单项触发槽函数
    void on_actionOpen_triggered();      ///< 打开文件菜单项触发槽函数
    void on_actionSave_triggered();      ///< 保存文件菜单项触发槽函数
    
    // BNF文法处理相关槽函数
    void on_pushButtonParseBNF_clicked();///< 解析BNF文法按钮点击槽函数
    void on_pushButtonClearBNF_clicked();///< 清空BNF文法按钮点击槽函数
    void on_pushButtonLoadExample_clicked();///< 加载示例文法按钮点击槽函数
    
    // First集合与Follow集合计算相关槽函数
    void on_pushButtonCalculateFirst_clicked();///< 计算First集合按钮点击槽函数
    void on_pushButtonCalculateFollow_clicked();///< 计算Follow集合按钮点击槽函数
    
    // LR(0) DFA生成相关槽函数
    void on_pushButtonGenerateLR0DFA_clicked();///< 生成LR(0)DFA按钮点击槽函数
    void on_comboBoxLR0View_currentIndexChanged(int index);///< LR(0)视图组合框变化槽函数
    
    // SLR(1)文法判断相关槽函数
    void on_pushButtonCheckSLR1_clicked();///< 检查SLR(1)文法按钮点击槽函数
    
    // LR(1) DFA生成相关槽函数
    void on_pushButtonGenerateLR1DFA_clicked();///< 生成LR(1)DFA按钮点击槽函数
    void on_comboBoxLR1View_currentIndexChanged(int index);///< LR(1)视图组合框变化槽函数
    
    // LR(1)分析表生成相关槽函数
    void on_pushButtonGenerateLR1Table_clicked();///< 生成LR(1)分析表按钮点击槽函数
    void on_pushButtonSaveLR1Table_clicked();///< 保存LR(1)分析表按钮点击槽函数
    
    // 语法分析与语法树生成相关槽函数
    void on_pushButtonOpenTokenFile_clicked();///< 打开Token文件按钮点击槽函数
    void on_pushButtonAnalyzeSyntax_clicked();///< 语法分析按钮点击槽函数
    
    // Token映射相关槽函数
    void on_pushButtonLoadTokenMap_clicked();///< 加载Token映射按钮点击槽函数
    
    // 语义动作相关槽函数
    void on_pushButtonLoadSemantics_clicked();///< 加载语义动作按钮点击槽函数

private:
    Ui::Task2Window *ui;                 ///< UI指针
    
    // 文法对象
    Grammar m_grammar;                   ///< 文法对象
    
    // 语法分析相关数据
    LL1Info m_ll1Info;                   ///< LL1分析器信息
    LR0Graph m_lr0Graph;                 ///< LR(0)状态图
    SLRCheckResult m_slrResult;          ///< SLR(1)检查结果
    LR1Graph m_lr1Graph;                 ///< LR(1)状态图
    LR1ActionTable m_lr1Table;           ///< LR(1)动作表
    
    // 词法分析和语法分析相关数据
    QVector<TokenInfo> m_tokens;         ///< 存储token序列，包含token类型和词素
    QString m_tokenFileContent;          ///< Token文件内容
    ParseResult m_parseResult;           ///< 语法分析结果
    QMap<QString, QString> m_tokenMap;   ///< token 编码到终结符名称的映射
    QSet<QString> m_singleCodeTokens;    ///< 存储单编码token的名称，这些token需要词素
    
    // 语义动作相关数据
    QMap<QString, QVector<QVector<int>>> m_semanticActions; ///< 语义动作映射
    QMap<int, QString> m_roleMeaning;    ///< 角色含义映射
    
    // 辅助函数
    void initUI();                       ///< 初始化UI
    void loadExampleGrammar();           ///< 加载示例文法
    void displayFirstSet();              ///< 显示First集合
    void displayFollowSet();             ///< 显示Follow集合
    void displayLR0DFA();                ///< 显示LR(0)DFA
    void displaySLR1Result();            ///< 显示SLR(1)检查结果
    void displayLR1DFA();                ///< 显示LR(1)DFA
    void displayLR1Table();              ///< 显示LR(1)分析表
    void displaySyntaxAnalysisResult();  ///< 显示语法分析结果
    bool loadTokenMap(const QString &mapPath);///< 加载Token映射
    bool parseSemanticActions(const QString &semPath);///< 解析语义动作
};

#endif // TASK2WINDOW_H