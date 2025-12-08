#include <iostream>
#include <QCoreApplication>
#include "task1/regexprocessor.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    // 创建测试用的正则表达式文本
    QString regexText = "
// 关键字定义
keyword = if|then|else|while|do|int|float|double|char|void|return

// 标识符定义
_identifier = letter(letter|digit)*

// 数字定义
_number = digit+(.digit+)?(e[+-]?digit+)?

// 字符串定义
_string = \"([^\\"]|\\.)*\"

// 关键字（多单词）
_ifS = (i|I)(f|F)
_thenS = (t|T)(h|H)(e|E)(n|N)
_elseS = (e|E)(l|L)(s|S)(e|E)
_whileS = (w|W)(h|H)(i|I)(l|L)(e|E)
_doS = (d|D)(o|O)
_intS = (i|I)(n|N)(t|T)
_floatS = (f|F)(l|L)(o|O)(a|A)(t|T)
_doubleS = (d|D)(o|O)(u|U)(b|B)(l|L)(e|E)
_charS = (c|C)(h|H)(a|A)(r|R)
_voidS = (v|V)(o|O)(i|I)(d|D)
_returnS = (r|R)(e|E)(t|T)(u|U)(r|R)(n|N)
";
    
    // 创建正则表达式处理器
    RegexProcessor processor;
    
    // 解析正则表达式
    if (processor.parse(regexText)) {
        cout << "正则表达式解析成功！" << endl;
        
        // 获取解析后的正则表达式列表
        QList<RegexItem> regexItems = processor.getRegexItems();
        
        // 打印每个正则表达式项
        for (const RegexItem &item : regexItems) {
            cout << "名称: " << item.name.toStdString() << endl;
            cout << "模式: " << item.pattern.toStdString() << endl;
            cout << "编码: " << item.code << endl;
            cout << "是否为多单词: " << (item.isMultiWord ? "是" : "否") << endl;
            
            if (item.isMultiWord) {
                cout << "单词列表: " << endl;
                for (const QString &word : item.wordList) {
                    cout << "  - " << word.toStdString() << endl;
                }
                cout << "单词数量: " << item.wordList.size() << endl;
            }
            
            cout << "------------------------" << endl;
        }
        
        return 0;
    } else {
        cout << "正则表达式解析失败: " << processor.getErrorMessage().toStdString() << endl;
        return 1;
    }
}
