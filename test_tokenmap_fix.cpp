#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include "task1/regexprocessor.h"
#include "task1/lexergenerator.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    // 读取regex.txt文件
    QFile file("test/测试样例/mini-c/regex.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开regex.txt文件";
        return 1;
    }
    
    QString regexText = file.readAll();
    file.close();
    
    // 解析正则表达式
    RegexProcessor processor;
    if (!processor.parse(regexText)) {
        qDebug() << "正则表达式解析失败:" << processor.getErrorMessage();
        return 1;
    }
    
    // 获取解析后的正则表达式项
    QList<RegexItem> regexItems = processor.getRegexItems();
    
    // 生成tokenmap
    LexerGenerator generator;
    QString tokenMap = generator.generateTokenMap(regexItems);
    
    // 输出结果
    qDebug() << "生成的tokenmap内容:";
    qDebug() << tokenMap;
    
    // 检查是否包含括号
    if (tokenMap.contains('(') && tokenMap.contains(')')) {
        qDebug() << "✓ 修复成功: tokenmap中包含括号";
    } else {
        qDebug() << "✗ 修复失败: tokenmap中缺少括号";
    }
    
    // 将结果保存到文件
    QFile outputFile("debug/fixed_sample.tokenmap");
    if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&outputFile);
        out << tokenMap;
        outputFile.close();
        qDebug() << "修复后的tokenmap已保存到debug/fixed_sample.tokenmap";
    }
    
    return 0;
}