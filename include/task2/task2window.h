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

class Task2Window : public QMainWindow
{
    Q_OBJECT

public:
    Task2Window(QWidget *parent = nullptr);
    ~Task2Window();

private slots:
    // 文件操作相关槽函数
    void on_actionExit_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    
    // BNF文法处理相关槽函数
    void on_pushButtonParseBNF_clicked();
    void on_pushButtonClearBNF_clicked();
    void on_pushButtonLoadExample_clicked();
    
    // First集合与Follow集合计算相关槽函数
    void on_pushButtonCalculateFirst_clicked();
    void on_pushButtonCalculateFollow_clicked();
    
    // LR(0) DFA生成相关槽函数
    void on_pushButtonGenerateLR0DFA_clicked();
    void on_comboBoxLR0View_currentIndexChanged(int index);
    
    // SLR(1)文法判断相关槽函数
    void on_pushButtonCheckSLR1_clicked();
    
    // LR(1) DFA生成相关槽函数
    void on_pushButtonGenerateLR1DFA_clicked();
    void on_comboBoxLR1View_currentIndexChanged(int index);
    
    // LR(1)分析表生成相关槽函数
    void on_pushButtonGenerateLR1Table_clicked();
    void on_pushButtonSaveLR1Table_clicked();
    
    // 语法分析与语法树生成相关槽函数
    void on_pushButtonOpenTokenFile_clicked();
    void on_pushButtonAnalyzeSyntax_clicked();
    
    // Token映射相关槽函数
    void on_pushButtonLoadTokenMap_clicked();
    
    // 语义动作相关槽函数
    void on_pushButtonLoadSemantics_clicked();

private:
    Ui::Task2Window *ui;
    
    // 文法对象
    Grammar m_grammar;
    
    // 语法分析相关数据
    LL1Info m_ll1Info;
    LR0Graph m_lr0Graph;
    SLRCheckResult m_slrResult;
    LR1Graph m_lr1Graph;
    LR1ActionTable m_lr1Table;
    
    // 词法分析和语法分析相关数据
    QVector<TokenInfo> m_tokens; // 存储token序列，包含token类型和词素
    QString m_tokenFileContent;
    ParseResult m_parseResult;
    QMap<QString, QString> m_tokenMap; // token 编码到终结符名称的映射
    QSet<QString> m_singleCodeTokens; // 存储单编码token的名称，这些token需要词素
    
    // 语义动作相关数据
    QMap<QString, QVector<QVector<int>>> m_semanticActions; // 语义动作映射
    QMap<int, QString> m_roleMeaning; // 角色含义映射
    
    // 辅助函数
    void initUI();
    void loadExampleGrammar();
    void displayFirstSet();
    void displayFollowSet();
    void displayLR0DFA();
    void displaySLR1Result();
    void displayLR1DFA();
    void displayLR1Table();
    void displaySyntaxAnalysisResult();
    bool loadTokenMap(const QString &mapPath);
    bool parseSemanticActions(const QString &semPath);
};

#endif // TASK2WINDOW_H