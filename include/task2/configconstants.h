/*
 * @file configconstants.h
 * @id configconstants-h
 * @brief 提供配置常量定义类，用于存储和获取语法分析器的各种配置参数
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#pragma once
#include <QString>
#include <QVector>

/**
 * @brief 配置常量类
 * 
 * 提供各种语法分析器所需的配置常量，包括符号定义、冲突策略、
 * token映射规则等，用于替代原有的Config类
 */
class ConfigConstants {
public:
    /**
     * @brief 获取EOF（文件结束）符号
     * 
     * @return QString EOF符号字符串
     */
    static QString eofSymbol() {
        return "$";
    }
    
    /**
     * @brief 获取空串（ε）符号
     * 
     * @return QString 空串符号字符串
     */
    static QString epsilonSymbol() {
        return "#";
    }
    
    /**
     * @brief 获取增广文法后缀
     * 
     * 用于生成增广文法时，添加到起始符号后的后缀
     * 
     * @return QString 增广文法后缀字符串
     */
    static QString augSuffix() {
        return "'";
    }
    
    /**
     * @brief 获取LR1冲突解决策略
     * 
     * 当LR1分析表中出现移进/归约冲突时使用的解决策略
     * 
     * @return QString 冲突解决策略，默认为"shift"（移进优先）
     */
    static QString lr1ConflictPolicy() {
        return "shift";
    }
    
    /**
     * @brief 获取优先移进的token列表
     * 
     * 在LR1冲突时，优先选择移进的token类型列表
     * 
     * @return QVector<QString> 优先移进的token列表
     */
    static QVector<QString> lr1PreferShiftTokens() {
        return QVector<QString>();
    }
    
    /**
     * @brief 是否使用启发式token映射
     * 
     * 当为true时，使用启发式算法自动生成token映射关系
     * 
     * @return bool 是否使用启发式token映射
     */
    static bool tokenMapUseHeuristics() {
        return true;
    }
    
    /**
     * @brief 获取标识符token名称列表
     * 
     * 常用的标识符token类型名称，用于识别程序中的变量名、函数名等
     * 
     * @return QVector<QString> 标识符token名称列表
     */
    static QVector<QString> identifierTokenNames() {
        return QVector<QString>({"identifier", "id", "var"});
    }
};
