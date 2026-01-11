# PA5

COOL编译器代码生成器（PA5）实现指南

项目概述

本项目实现了COOL（Classroom Object-Oriented Language）编译器的代码生成器，将COOL语言的抽象语法树（AST）转换为MIPS汇编代码，可在SPIM模拟器上运行。

项目结构

核心文件

• cgen.cc - 代码生成器主要实现文件

• cgen.h - 头文件，定义核心类结构

• cool-tree.h - AST节点定义

• cool-tree.handcode.h - 手写代码扩展

依赖文件（不可修改）

• Makefile - 构建配置

• cool-tree.cc - AST节点实现

• cgen-phase.cc - 代码生成阶段驱动

• emit.h - MIPS指令生成辅助函数

• cgen_supp.cc - 支持函数

核心架构

主要类结构

1. CgenClassTable（类表管理）

• 管理所有类的代码生成

• 生成全局数据结构（类名表、对象表、分发表等）

• 协调整个代码生成过程

2. CgenNode（类节点）

• 表示单个类的代码生成信息

• 管理类的属性、方法、继承关系

• 生成原型对象、初始化方法、类方法

3. Environment（环境管理）

• 管理代码生成时的符号表环境

• 跟踪变量、参数、属性的位置信息

• 处理作用域嵌套

4. Expression Classes（表达式类）

• 各种COOL表达式节点的代码生成实现

• 采用递归方式生成代码

代码生成流程

1. 初始化阶段

program_class::cgen(ostream& os)

• 初始化预定义符号

• 创建CgenClassTable

• 启动代码生成过程

2. 类表构建

CgenClassTable::CgenClassTable(Classes classes, ostream& s)

• 安装基本类（Object, IO, Int, Bool, String）

• 安装用户定义类

• 构建继承树结构

3. 代码生成主流程

CgenClassTable::code()

按顺序生成以下部分：

3.1 数据段（.data）

• 全局符号声明

• 类标签定义

• 常量定义（字符串、整数、布尔值）

3.2 类表结构

• 类名表（class_nameTab） - 存储所有类名的字符串地址

• 对象表（class_objTab） - 存储每个类的原型对象和初始化方法地址

• 分发表（dispatch tables） - 每个类的虚函数表

3.3 原型对象（prototype objects）

• 定义每个类的内存布局

• 包含类标签、对象大小、分发表地址

• 初始化属性默认值

3.4 初始化方法

• 生成每个类的构造函数

• 先调用父类初始化，再初始化当前类属性

• 返回新创建的对象

3.5 类方法

• 生成每个方法的实际代码

• 处理参数传递、局部变量、返回值

关键实现细节

表达式代码生成模式

所有表达式遵循统一的代码生成模式：
void expression_class::code(ostream& s, Environment env) {
    // 1. 递归生成子表达式代码
    child_expr->code(s, env);
    
    // 2. 保存中间结果（如需）
    emit_push(ACC, s);
    
    // 3. 生成其他子表达式
    another_child->code(s, env);
    
    // 4. 恢复中间结果
    emit_load(T1, 0, SP, s);
    
    // 5. 执行操作
    emit_add(ACC, T1, ACC, s);
    
    // 6. 结果在ACC中
}


对象模型

对象内存布局


偏移  内容              说明
----  ----------------  -------------------------
0     class tag        类标签（用于类型识别）
4     size             对象大小（以字为单位）
8     dispatch table   分发表地址
12    attribute 1      第一个属性
16    attribute 2      第二个属性
...   ...              ...


方法调用约定

• 参数传递：从右到左压栈

• 返回值：始终在$a0（ACC）寄存器中

• 栈管理：调用者负责清理参数

• 寄存器约定：

  • ACC（$a0）：表达式结果

  • SELF（$s0）：当前对象

  • FP（$fp）：帧指针

  • SP（$sp）：栈指针

继承处理

属性继承

• 子类继承父类的所有属性

• 属性按继承顺序排列在对象中

• 使用GetFullAttribs()获取完整属性列表

方法重写

• 子类可以重写父类方法

• 分发表中优先使用子类方法实现

• 使用GetFullMethods()构建正确的分发表

初始化顺序

1. 调用父类初始化方法
2. 初始化当前类属性
3. 返回初始化后的对象

MIPS汇编指令使用

核心指令集

指令类别 主要指令 用途

数据传送 lw, sw, li, la, move 加载/存储数据

算术运算 add, sub, mul, div, neg 数学运算

控制流 beq, bne, blt, ble, b 条件分支

函数调用 jal, jalr, jr 方法调用和返回

栈操作 addiu (SP调整) 栈管理

栈操作约定

• 栈增长方向：向低地址增长

• 压栈操作：先存储数据，再调整SP

• 弹栈操作：先调整SP，再读取数据

测试方法

编译和运行

# 1. 确保链接必要文件
ln -sf /usr/class/bin/lexer .
ln -sf /usr/class/bin/parser .
ln -sf /usr/class/bin/semant .

# 2. 编译代码生成器
make clean
make cgen

# 3. 编译COOL程序
./lexer test.cl | ./parser test.cl 2>&1 | ./semant test.cl 2>&1 | ./cgen -o test.s test.cl

# 4. 运行生成的汇编代码
/usr/class/bin/spim -file test.s


测试用例

• 简单测试：基础表达式和语句

• 继承测试：类继承和方法重写

• 复杂测试：完整程序功能验证

常见问题解决

编译错误

1. 缺少方法定义：确保在cool-tree.h中添加必要的辅助方法
2. 函数签名不匹配：检查code()方法的参数列表

运行时错误

1. 段错误：检查指针使用和栈操作
2. 未定义符号：验证全局数据生成是否正确
3. 方法调用失败：检查分发表构建和索引计算

逻辑错误

1. 表达式值错误：验证中间结果保存和恢复
2. 继承关系错误：检查属性/方法索引计算
3. 对象初始化错误：验证初始化顺序

实现注意事项

代码质量

• 保持代码清晰和模块化

• 添加充分的注释说明

• 遵循一致的编码风格

性能考虑

• 优化寄存器使用

• 减少不必要的内存访问

• 合理使用临时寄存器

正确性验证

• 与官方参考实现对比输出

• 测试边界情况

• 验证错误处理

扩展功能

可选优化

• 常量折叠和传播

• 死代码消除

• 寄存器分配优化

调试支持

• 生成调试信息

• 添加运行时检查

• 提供错误诊断

总结

本代码生成器实现了完整的COOL到MIPS的编译流程，正确处理了面向对象特性（继承、多态、动态分发）和复杂的表达式求值。通过模块化的设计和清晰的代码结构，确保了生成代码的正确性和可维护性。

关键成功因素：
• 深入理解MIPS汇编和运行时结构

• 正确处理面向对象继承关系

• 严格的测试和验证流程

• 清晰的代码组织和注释

通过本实现，可以成功将COOL高级语言程序转换为可在SPIM模拟器上运行的MIPS汇编代码。
