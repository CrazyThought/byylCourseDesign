#ifndef LEXERTESTER_H
#define LEXERTESTER_H

#include <QString>
#include <QList>
#include <QProcess>

// 词法分析结果结构体
struct LexicalResult {
    int line;           // 行号
    QString lexeme;     // 单词
    int tokenCode;      // 单词编码
};

class LexerTester
{
public:
    LexerTester();
    ~LexerTester();
    
    // 编译词法分析器代码
    bool compileLexer(const QString &lexerCode, const QString &outputFileName = "lexer");
    
    // 测试词法分析器
    QList<LexicalResult> testLexer(const QString &sourceFile, const QString &lexerExecutable = "lexer");
    
    // 获取错误信息
    QString getError() const;
    
    // 获取编译输出
    QString getCompileOutput() const;
    
    // 获取测试输出
    QString getTestOutput() const;
    
private:
    // 保存临时文件
    bool saveTempFile(const QString &content, const QString &fileName);
    
    // 执行命令
    bool executeCommand(const QString &command, const QString &workingDir, QString &output);
    
    // 解析词法分析结果
    QList<LexicalResult> parseLexicalResult(const QString &output);
    
    // 清理临时文件
    void cleanupTempFiles();
    
    // 错误信息
    QString m_errorMessage;
    
    // 编译输出
    QString m_compileOutput;
    
    // 测试输出
    QString m_testOutput;
    
    // 临时文件列表
    QStringList m_tempFiles;
};

#endif // LEXERTESTER_H
