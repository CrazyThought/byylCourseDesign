/*
 * @file Grammar.cpp
 * @id grammar-cpp
 * @brief 文法定义的核心实现，包含文法的基本操作和判断
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#include "task2/Grammar.h"

/**
 * @brief 判断产生式右部是否为空串
 * @param rhs 产生式右部符号列表
 * @return 如果右部只有一个空串符号("#")，返回true；否则返回false
 */
bool Grammar::hasEpsilon(const QVector<QString>& rhs) const
{
    return rhs.size() == 1 && rhs[0] == "#";
}