/*
 * @file TokenMapBuilder.cpp
 * @id tokenmapbuilder-cpp
 * @brief 实现TokenMapBuilder类的功能，用于构建、保存和解析token映射表
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task2/TokenMapBuilder.h"
#include "task2/configconstants.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>

/**
 * @brief 移除字符串中的转义字符
 * 
 * 将形如"\\n"的转义字符转换为实际字符"n"
 * 
 * @param s 包含转义字符的字符串
 * @return QString 移除转义字符后的字符串
 */
static QString unescape(const QString& s)
{
    QString r;
    for (int i = 0; i < s.size(); ++i)
    {
        if (s[i] == '\\' && i + 1 < s.size())
        {
            r += s[i + 1];
            i++;
        }
        else
        {
            r += s[i];
        }
    }
    return r;
}

/**
 * @brief 收集AST节点中的所有替代分支
 * 
 * 递归遍历AST，收集所有Union节点的替代分支
 * 
 * @param n 要遍历的AST节点
 * @param out 输出向量，用于存储收集到的替代分支
 */
static void collectAlternatives(ASTNode* n, QVector<ASTNode*>& out)
{
    if (!n)
        return;
    if (n->type == ASTNode::Union)
    {
        // 递归收集左右子树的替代分支
        collectAlternatives(n->children[0], out);
        collectAlternatives(n->children[1], out);
    }
    else
    {
        // 非Union节点直接添加到输出
        out.push_back(n);
    }
}

/**
 * @brief 从AST节点生成字面量
 * 
 * 遍历AST节点，生成对应的字面量字符串
 * 
 * @param n AST节点
 * @return QString 生成的字面量字符串
 */
static QString literalFromAst(ASTNode* n)
{
    if (!n)
        return QString();
    
    if (n->type == ASTNode::Union)
    {
        // 取第一个分支的字面量（用于大小写折叠等）
        return literalFromAst(n->children[0]);
    }
    
    if (n->type == ASTNode::Symbol)
        return n->value;
    
    if (n->type == ASTNode::CharSet)
    {
        QString v = n->value;
        if (v.isEmpty())
            return QString();
        return QString(v[0]).toLower();
    }
    
    if (n->type == ASTNode::Concat)
    {
        QString s;
        for (auto c : n->children) s += literalFromAst(c);
        return s;
    }
    
    // 对其他节点（Star/Plus/Question）不期望出现在终结符定义中，返回空
    return QString();
}

/**
 * @brief 构建token映射表
 * 
 * 根据正则表达式文本和解析后的文件内容，生成token类型到正则表达式的映射关系
 * 
 * @param regexText 正则表达式文本，包含token定义规则
 * @param pf 解析后的文件对象，包含语法规则信息
 * @return QMap<QString, QString> 生成的token映射表，键为token类型，值为对应的正则表达式
 */
QMap<QString, QString> TokenMapBuilder::build(const QString& regexText, const ParsedFile& pf)
{
    QMap<QString, QString> m;
    
    // 遍历所有token规则
    for (const auto& pt : pf.tokens)
    {
        const auto& name = pt.rule.name;     // token规则名称
        int         base = pt.rule.code;     // token基础编码
        const auto& expr = pt.rule.expr;     // token表达式
        
        // 使用启发式算法生成token映射
        if (ConfigConstants::tokenMapUseHeuristics() && name.contains("identifier", Qt::CaseInsensitive))
        {
            // 标识符类型token
            m[QString::number(base)] = "identifier";
        }
        else if (ConfigConstants::tokenMapUseHeuristics() && name.contains("number", Qt::CaseInsensitive))
        {
            // 数字类型token
            m[QString::number(base)] = "number";
        }
        else if (ConfigConstants::tokenMapUseHeuristics() && (pt.rule.isGroup || name.contains('S')))
        {
            // 分组类型token，收集所有替代分支
            QVector<ASTNode*> alts;
            collectAlternatives(pt.ast, alts);
            
            // 为每个替代分支生成token映射
            int idx = 0;
            for (auto a : alts)
            {
                QString term = literalFromAst(a).toLower();
                m[QString::number(base + idx)] = term;
                idx++;
            }
        }
        else
        {
            // 默认：使用规则名（供文法作者通过配置覆盖）
            m[QString::number(base)] = name;
        }
    }
    
    return m;
}

/**
 * @brief 保存token映射表为JSON文件
 * 
 * 将token映射表以JSON格式保存到指定路径
 * 
 * @param m 要保存的token映射表
 * @param path 保存文件的路径
 * @return bool 保存成功返回true，失败返回false
 */
bool TokenMapBuilder::saveJson(const QMap<QString, QString>& m, const QString& path)
{
    // 构建JSON对象
    QJsonObject obj;
    for (auto it = m.begin(); it != m.end(); ++it) {
        obj.insert(it.key(), it.value());
    }
    
    QJsonDocument doc(obj);
    
    // 写入文件
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    f.write(doc.toJson(QJsonDocument::Compact));
    f.close();
    
    return true;
}

/**
 * @brief 从JSON文件解析token映射表
 * 
 * 从指定路径读取JSON文件，并解析为token映射表
 * 
 * @param path 要读取的JSON文件路径
 * @return QMap<int, QString> 解析后的token映射表，键为token类型ID，值为token类型名称
 */
QMap<int, QString> TokenMapBuilder::parseTokenMap(const QString& path)
{
    QMap<int, QString> tokenMap;
    QFile file(path);
    
    // 打开文件
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return tokenMap;
    }
    
    QTextStream in(&file);
    int lineNumber = 0;
    
    // 逐行读取文件
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineNumber++;
        
        // 跳过空行和注释行
        if (line.isEmpty() || line.startsWith("//")) {
            continue;
        }
        
        // 查找第一个等号位置
        int equalsPos = line.indexOf('=');
        if (equalsPos == -1) {
            continue; // 无效行，跳过
        }
        
        // 提取编码和token名称，只使用第一个等号作为分隔符
        QString codeStr = line.left(equalsPos).trimmed();
        QString tokenName = line.mid(equalsPos + 1).trimmed();
        
        // 转换编码为整数
        bool ok = false;
        int code = codeStr.toInt(&ok);
        if (ok) {
            tokenMap[code] = tokenName;
        } else {
            // 无效编码，跳过
            continue;
        }
    }
    
    file.close();
    return tokenMap;
}