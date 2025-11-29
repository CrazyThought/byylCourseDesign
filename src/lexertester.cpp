#include "lexertester.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

LexerTester::LexerTester()
{}

LexerTester::~LexerTester()
{
    cleanupTempFiles();
}

bool LexerTester::compileLexer(const QString &lexerCode, const QString &outputFileName)
{
    m_errorMessage.clear();
    m_compileOutput.clear();
    
    // 保存词法分析器代码到临时文件
    QString sourceFileName = outputFileName + ".cpp";
    if (!saveTempFile(lexerCode, sourceFileName)) {
        return false;
    }
    
    // 编译命令
    QString command = QString("g++ %1 -o %2 -std=c++11").arg(sourceFileName, outputFileName);
    
    // 执行编译命令
    QString output;
    if (!executeCommand(command, QDir::currentPath(), output)) {
        m_errorMessage = "编译失败";
        m_compileOutput = output;
        return false;
    }
    
    m_compileOutput = output;
    
    // 将可执行文件添加到临时文件列表
    m_tempFiles << outputFileName + (QSysInfo::productType() == "windows" ? ".exe" : "");
    
    return true;
}

QList<LexicalResult> LexerTester::testLexer(const QString &sourceFile)
{
    m_errorMessage.clear();
    m_testOutput.clear();
    
    // 检查源文件是否存在
    QFile source(sourceFile);
    if (!source.exists()) {
        m_errorMessage = "源文件不存在: " + sourceFile;
        return QList<LexicalResult>();
    }
    
    // 执行词法分析器
    QString lexerExecutable = "lexer";
    if (QSysInfo::productType() == "windows") {
        lexerExecutable += ".exe";
    }
    QString command = QString("./%1 %2").arg(lexerExecutable, sourceFile);
    
    QString output;
    if (!executeCommand(command, QDir::currentPath(), output)) {
        m_errorMessage = "词法分析执行失败";
        m_testOutput = output;
        return QList<LexicalResult>();
    }
    
    m_testOutput = output;
    
    // 解析词法分析结果
    return parseLexicalResult(output);
}

QString LexerTester::getError() const
{
    return m_errorMessage;
}

QString LexerTester::getCompileOutput() const
{
    return m_compileOutput;
}

QString LexerTester::getTestOutput() const
{
    return m_testOutput;
}

bool LexerTester::saveTempFile(const QString &content, const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_errorMessage = "无法创建临时文件: " + fileName;
        return false;
    }
    
    QTextStream out(&file);
    out << content;
    file.close();
    
    // 将文件添加到临时文件列表
    m_tempFiles << fileName;
    
    return true;
}

bool LexerTester::executeCommand(const QString &command, const QString &workingDir, QString &output)
{
    QProcess process;
    process.setWorkingDirectory(workingDir);
    process.start(command);
    
    if (!process.waitForFinished()) {
        output = process.readAllStandardError();
        return false;
    }
    
    output = process.readAllStandardOutput();
    QString errorOutput = process.readAllStandardError();
    
    if (!errorOutput.isEmpty()) {
        output += "\n错误输出:\n" + errorOutput;
    }
    
    return process.exitCode() == 0;
}

QList<LexicalResult> LexerTester::parseLexicalResult(const QString &output)
{
    // 解析词法分析结果
    QList<LexicalResult> results;
    QString outputCopy = output;
    QTextStream stream(&outputCopy);
    QString line;
    
    while (stream.readLineInto(&line)) {
        // 跳过空行和非结果行
        if (line.isEmpty() || !line.contains('\t')) {
            continue;
        }
        
        // 解析行号、单词和编码
        QStringList parts = line.split('\t');
        // 移除空部分
        parts.removeAll("");
        if (parts.size() >= 3) {
            LexicalResult result;
            result.line = parts[0].toInt();
            result.lexeme = parts[1];
            result.tokenCode = parts[2].toInt();
            results.append(result);
        }
    }
    
    return results;
}

void LexerTester::cleanupTempFiles()
{
    foreach (const QString &file, m_tempFiles) {
        QFile::remove(file);
    }
    m_tempFiles.clear();
}
