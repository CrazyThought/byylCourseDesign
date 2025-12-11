/*
 * @file Grammar.h
 * @id Grammar-h
 * @brief 文法定义的头文件，包含文法的结构定义和类声明
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#pragma once
#include <QString>
#include <QVector>
#include <QMap>
#include <QSet>

/**
 * @brief 产生式结构体
 * @struct Production
 * @details 表示一个文法产生式，包含左部非终结符、右部符号列表和产生式所在行号
 */
struct Production
{
    QString          left;           ///< 产生式左部非终结符
    QVector<QString> right;          ///< 产生式右部符号列表
    int              line = -1;      ///< 产生式所在行号，默认为-1
};

/**
 * @brief 文法类
 * @class Grammar
 * @details 表示一个完整的文法，包含终结符、非终结符、开始符号和产生式集合
 */
class Grammar
{
   public:
    QSet<QString>                      terminals;         ///< 终结符集合
    QSet<QString>                      nonterminals;      ///< 非终结符集合
    QString                            startSymbol;       ///< 开始符号
    QMap<QString, QVector<Production>> productions;       ///< 产生式集合，按左部非终结符组织
    
    /**
     * @brief 判断产生式右部是否为空串
     * @param rhs 产生式右部符号列表
     * @return 如果右部只有一个空串符号("#")，返回true；否则返回false
     */
    bool                               hasEpsilon(const QVector<QString>& rhs) const;
};