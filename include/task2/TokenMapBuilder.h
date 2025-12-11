/*
 * @file TokenMapBuilder.h
 * @id TokenMapBuilder-h
 * @brief 提供Token映射构建器类，用于生成和管理词法分析器的token映射表
 * @version 1.0
 * @author 郭梓烽
 * @date 2025/12/07
 * @copyright Copyright (c) 2025 郭梓烽
 */
#pragma once
#include <QString>
#include <QMap>
#include "../regex/RegexParser.h"

/**
 * @brief Token映射构建器类
 * 
 * 该类提供静态方法用于构建、保存和解析token映射表，
 * 是词法分析器和语法分析器之间的桥梁组件
 */
class TokenMapBuilder
{
   public:
    /**
     * @brief 构建token映射表
     * 
     * 根据正则表达式文本和解析后的文件内容，生成token类型到正则表达式的映射关系
     * 
     * @param regexText 正则表达式文本，包含token定义规则
     * @param pf 解析后的文件对象，包含语法规则信息
     * @return QMap<QString, QString> 生成的token映射表，键为token类型，值为对应的正则表达式
     */
    static QMap<QString, QString> build(const QString& regexText, const ParsedFile& pf);
    
    /**
     * @brief 保存token映射表为JSON文件
     * 
     * 将token映射表以JSON格式保存到指定路径
     * 
     * @param m 要保存的token映射表
     * @param path 保存文件的路径
     * @return bool 保存成功返回true，失败返回false
     */
    static bool saveJson(const QMap<QString, QString>& m, const QString& path);
    
    /**
     * @brief 从JSON文件解析token映射表
     * 
     * 从指定路径读取JSON文件，并解析为token映射表
     * 
     * @param path 要读取的JSON文件路径
     * @return QMap<int, QString> 解析后的token映射表，键为token类型ID，值为token类型名称
     */
    static QMap<int, QString> parseTokenMap(const QString& path);
};