/*
 * 版权信息：Copyright (c) 2023 林展星
 * 文件名称：TokenMapBuilder.cpp
 *
 * 当前版本：1.0.0
 * 作    者：林展星
 * 完成日期：2023年12月07日
 *
 * 版本历史：
 * 1.0.0 2023-12-07 林展星 初始版本
 */
#include "task2/TokenMapBuilder.h"
#include "task2/configconstants.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>

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

static void collectAlternatives(ASTNode* n, QVector<ASTNode*>& out)
{
    if (!n)
        return;
    if (n->type == ASTNode::Union)
    {
        collectAlternatives(n->children[0], out);
        collectAlternatives(n->children[1], out);
    }
    else
    {
        out.push_back(n);
    }
}

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

QMap<QString, QString> TokenMapBuilder::build(const QString& regexText, const ParsedFile& pf)
{
    QMap<QString, QString> m;
    for (const auto& pt : pf.tokens)
    {
        const auto& name = pt.rule.name;
        int         base = pt.rule.code;
        const auto& expr = pt.rule.expr;
        if (ConfigConstants::tokenMapUseHeuristics() && name.contains("identifier", Qt::CaseInsensitive))
        {
            m[QString::number(base)] = "identifier";
        }
        else if (ConfigConstants::tokenMapUseHeuristics() && name.contains("number", Qt::CaseInsensitive))
        {
            m[QString::number(base)] = "number";
        }
        else if (ConfigConstants::tokenMapUseHeuristics() && (pt.rule.isGroup || name.contains('S')))
        {
            QVector<ASTNode*> alts;
            collectAlternatives(pt.ast, alts);
            int idx = 0;
            for (auto a : alts)
            {
                QString term                   = literalFromAst(a).toLower();
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

bool TokenMapBuilder::saveJson(const QMap<QString, QString>& m, const QString& path)
{
    QJsonObject obj;
    for (auto it = m.begin(); it != m.end(); ++it) obj.insert(it.key(), it.value());
    QJsonDocument doc(obj);
    QFile         f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    f.write(doc.toJson(QJsonDocument::Compact));
    f.close();
    return true;
}

QMap<int, QString> TokenMapBuilder::parseTokenMap(const QString& path)
{
    QMap<int, QString> tokenMap;
    QFile file(path);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return tokenMap;
    }
    
    QTextStream in(&file);
    int lineNumber = 0;
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