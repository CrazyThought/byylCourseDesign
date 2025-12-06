#include "task1/lexertester.h"
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
    
    // 检查g++是否可用
    QString checkCommand = "g++ --version";
    QString checkOutput;
    if (!executeCommand(checkCommand, QDir::currentPath(), checkOutput)) {
        m_errorMessage = "无法找到g++编译器，请确保已正确安装并配置环境变量";
        m_compileOutput = checkOutput;
        return false;
    }
    
    // 编译命令 - 移除末尾空格，确保命令格式正确
    QString command = QString("g++ %1 -o %2").arg(sourceFileName, outputFileName);
    
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

QList<LexicalResult> LexerTester::testLexer(const QString &sourceFile, const QString &lexerExecutable)
{
    m_errorMessage.clear();
    m_testOutput.clear();
    
    // 检查源文件是否存在
    QFile source(sourceFile);
    if (!source.exists()) {
        m_errorMessage = "源文件不存在: " + sourceFile;
        return QList<LexicalResult>();
    }
    
    // 构建完整的可执行文件路径
    QString fullExecutablePath;
    if (QSysInfo::productType() == "windows") {
        fullExecutablePath = lexerExecutable + ".exe";
        // 在Windows上，直接使用文件名，不需要./前缀
        fullExecutablePath = QDir::currentPath() + "/" + fullExecutablePath;
    } else {
        // 在Linux/Mac上，使用./前缀
        fullExecutablePath = "./" + lexerExecutable;
    }
    
    // 执行词法分析器命令
    QString command = QString("%1 %2").arg(fullExecutablePath, sourceFile);
    
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
    
    // 在Windows平台上，使用cmd /c来执行命令，确保命令能正确执行
    if (QSysInfo::productType() == "windows") {
        process.start("cmd.exe", QStringList() << "/c" << command);
    } else {
        // 在Linux/Mac平台上，使用bash来执行命令
        process.start("bash", QStringList() << "-c" << command);
    }
    
    // 添加超时时间（10秒），防止命令执行无限期等待
    const int timeout = 10000;
    if (!process.waitForStarted() || !process.waitForFinished(timeout)) {
        // 读取所有可用的输出，包括标准输出和错误输出
        QString stdOutput = process.readAllStandardOutput();
        QString errOutput = process.readAllStandardError();
        
        if (process.state() == QProcess::Running) {
            process.kill();
            process.waitForFinished(500);
            output = "命令执行超时，已终止进程\n";
        }
        
        if (!stdOutput.isEmpty()) {
            output += "标准输出:\n" + stdOutput + "\n";
        }
        if (!errOutput.isEmpty()) {
            output += "错误输出:\n" + errOutput + "\n";
        }
        
        return false;
    }
    
    // 读取所有输出
    QString stdOutput = process.readAllStandardOutput();
    QString errOutput = process.readAllStandardError();
    
    if (!stdOutput.isEmpty()) {
        output = stdOutput;
    }
    
    if (!errOutput.isEmpty()) {
        if (!output.isEmpty()) {
            output += "\n";
        }
        output += "错误输出:\n" + errOutput;
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
        // 跳过空行
        if (line.isEmpty()) {
            continue;
        }
        
        // 处理制表符分隔格式：行号\t单词\t编码
        if (line.contains('\t')) {
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
            continue;
        }
        
        // 处理旧格式：Token(code: X, lexeme: "Y", line: Z)
        if (line.startsWith("Token(code: ")) {
            // 提取code
            int codeStart = line.indexOf("code: ") + 6;
            int codeEnd = line.indexOf(", lexeme: ", codeStart);
            if (codeStart < codeEnd) {
                QString codeStr = line.mid(codeStart, codeEnd - codeStart);
                int tokenCode = codeStr.toInt();
                
                // 提取lexeme
                int lexemeStart = line.indexOf(", lexeme: \\\"") + 11;
                int lexemeEnd = line.indexOf("\\\"\", line: ", lexemeStart);
                if (lexemeStart < lexemeEnd) {
                    QString lexeme = line.mid(lexemeStart, lexemeEnd - lexemeStart);
                    
                    // 提取line
                    int lineStart = line.indexOf("line: ") + 6;
                    int lineEnd = line.indexOf(")", lineStart);
                    if (lineStart < lineEnd) {
                        QString lineStr = line.mid(lineStart, lineEnd - lineStart);
                        int lineNumber = lineStr.toInt();
                        
                        LexicalResult result;
                        result.line = lineNumber;
                        result.lexeme = lexeme;
                        result.tokenCode = tokenCode;
                        results.append(result);
                    }
                }
            }
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
