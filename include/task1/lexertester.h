/*
 * @file lexertester.h
 * @id lexertester-h
 * @brief 实现词法分析器测试功能，包括编译词法分析器和测试词法分析器
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#ifndef LEXERTESTER_H
#define LEXERTESTER_H

#include <QString>
#include <QList>
#include <QProcess>

/**
 * @brief 词法分析结果结构体
 * 
 * 表示词法分析器对源文件进行分析后产生的单个结果
 */
struct LexicalResult {
    int line;           ///< 单词所在的行号
    QString lexeme;     ///< 单词的实际内容
    int tokenCode;      ///< 单词对应的编码
};

/**
 * @brief 词法分析器测试类
 * 
 * 用于编译和测试词法分析器代码
 */
class LexerTester
{
public:
    /**
     * @brief 构造函数
     */
    LexerTester();
    
    /**
     * @brief 析构函数
     */
    ~LexerTester();
    
    /**
     * @brief 编译词法分析器代码
     * 
     * @param lexerCode 词法分析器代码
     * @param outputFileName 输出可执行文件名称，默认为"lexer"
     * @return bool 编译成功返回true，失败返回false
     */
    bool compileLexer(const QString &lexerCode, const QString &outputFileName = "lexer");
    
    /**
     * @brief 测试词法分析器
     * 
     * @param sourceFile 要测试的源文件
     * @param lexerExecutable 词法分析器可执行文件，默认为"lexer"
     * @return QList<LexicalResult> 词法分析结果列表
     */
    QList<LexicalResult> testLexer(const QString &sourceFile, const QString &lexerExecutable = "lexer");
    
    /**
     * @brief 获取错误信息
     * 
     * @return QString 错误信息
     */
    QString getError() const;
    
    /**
     * @brief 获取编译输出
     * 
     * @return QString 编译输出
     */
    QString getCompileOutput() const;
    
    /**
     * @brief 获取测试输出
     * 
     * @return QString 测试输出
     */
    QString getTestOutput() const;
    
private:
    /**
     * @brief 保存临时文件
     * 
     * @param content 文件内容
     * @param fileName 文件名称
     * @return bool 保存成功返回true，失败返回false
     */
    bool saveTempFile(const QString &content, const QString &fileName);
    
    /**
     * @brief 执行命令
     * 
     * @param command 要执行的命令
     * @param workingDir 工作目录
     * @param output 输出参数，存储命令输出
     * @return bool 执行成功返回true，失败返回false
     */
    bool executeCommand(const QString &command, const QString &workingDir, QString &output);
    
    /**
     * @brief 解析词法分析结果
     * 
     * @param output 词法分析器的输出
     * @return QList<LexicalResult> 解析后的词法分析结果列表
     */
    QList<LexicalResult> parseLexicalResult(const QString &output);
    
    /**
     * @brief 清理临时文件
     */
    void cleanupTempFiles();
    
    QString m_errorMessage;  ///< 错误信息
    QString m_compileOutput;  ///< 编译输出
    QString m_testOutput;  ///< 测试输出
    QStringList m_tempFiles;  ///< 临时文件列表
};

#endif // LEXERTESTER_H
