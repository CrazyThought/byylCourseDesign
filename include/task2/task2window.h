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

// 前向声明
class BNFParser;
class FirstFollow;
class LR0DFA;
class SLR1Checker;
class LR1DFA;
class LR1Table;
class SyntaxAnalyzer;

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
    void on_pushButtonSaveSyntaxTree_clicked();

private:
    Ui::Task2Window *ui;
    
    // 功能类指针
    BNFParser *m_bnfParser;
    FirstFollow *m_firstFollow;
    LR0DFA *m_lr0dfa;
    SLR1Checker *m_slr1Checker;
    LR1DFA *m_lr1dfa;
    LR1Table *m_lr1Table;
    SyntaxAnalyzer *m_syntaxAnalyzer;
    
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
};

#endif // TASK2WINDOW_H