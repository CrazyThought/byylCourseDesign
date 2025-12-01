1. 在RegexProcessor类中添加getRegexReferences方法，用于获取正则表达式引用映射
2. 在MainWindow类的displayNFA、displayDFA和displayMinimizedDFA函数中，修改列名生成逻辑：

   * 遍历所有转换字符

   * 对于每个字符，检查是否存在对应的正则表达式引用

   * 如果存在，使用引用名称作为列名；否则使用原字符

   * 若存在多个使用同一引用名称的字符，则合并为同一列
3. 确保只修改列名显示逻辑，不影响其他功能

