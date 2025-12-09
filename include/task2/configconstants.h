#pragma once

// 配置常量定义，用于替换Config类

class ConfigConstants {
public:
    // EOF符号
    static QString eofSymbol() {
        return "$";
    }
    
    // 空串符号
    static QString epsilonSymbol() {
        return "#";
    }
    
    // 增广文法后缀
    static QString augSuffix() {
        return "'";
    }
    
    // LR1冲突策略
    static QString lr1ConflictPolicy() {
        return "shift";
    }
    
    // 优先移进的token列表
    static QVector<QString> lr1PreferShiftTokens() {
        return QVector<QString>();
    }
    
    // 是否使用启发式token映射
    static bool tokenMapUseHeuristics() {
        return true;
    }
    
    // 标识符token名称列表
    static QVector<QString> identifierTokenNames() {
        return QVector<QString>({"identifier", "id", "var"});
    }
};
