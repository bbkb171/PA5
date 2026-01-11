#ifndef COOL_TREE_H
#define COOL_TREE_H
//////////////////////////////////////////////////////////
//
// file: cool-tree.h
//
// This file defines classes for each phylum and constructor
// 此文件定义了所有语法族（phylum）和具体构造器（constructor）的类
// 它是COOL语言抽象语法树（AST）的节点定义
//////////////////////////////////////////////////////////


#include "tree.h"           // 包含基础树节点定义
#include "cool-tree.handcode.h" // 包含可能的手写代码扩展
#include <vector>           // 包含标准向量库

class Environment; // 前向声明环境类，用于符号表管理等

// 定义语法族（Phylum）类
// 语法族是AST节点的抽象基类

// 定义简单语法族 - Program（程序根节点）
typedef class Program_class *Program;

class Program_class : public tree_node {
public:
   tree_node *copy()     { return copy_Program(); }
   virtual Program copy_Program() = 0; // 纯虚函数，用于复制Program节点

#ifdef Program_EXTRAS
   Program_EXTRAS        // 预处理器宏，用于在编译时添加额外成员
#endif
};


// 定义简单语法族 - Class_（类定义）
typedef class Class__class *Class_;

class Class__class : public tree_node {
public:
   tree_node *copy()     { return copy_Class_(); }
   virtual Class_ copy_Class_() = 0; // 纯虚函数，用于复制Class_节点

#ifdef Class__EXTRAS
   Class__EXTRAS
#endif
};


// 定义简单语法族 - Feature（类特征，即方法或属性）
typedef class Feature_class *Feature;

class Feature_class : public tree_node {
public:
   tree_node *copy()     { return copy_Feature(); }
   virtual Feature copy_Feature() = 0;
   virtual bool IsMethod() = 0; // 纯虚函数，判断该特征是方法还是属性

#ifdef Feature_EXTRAS
   Feature_EXTRAS
#endif
};


// 定义简单语法族 - Formal（形式参数）
typedef class Formal_class *Formal;

class Formal_class : public tree_node {
public:
   tree_node *copy()     { return copy_Formal(); }
   virtual Formal copy_Formal() = 0;
   virtual Symbol GetName() = 0; // 纯虚函数，获取形参的名称

#ifdef Formal_EXTRAS
   Formal_EXTRAS
#endif
};


// 定义简单语法族 - Expression（表达式）
typedef class Expression_class *Expression;

class Expression_class : public tree_node {
public:
   tree_node *copy()     { return copy_Expression(); }
   virtual Expression copy_Expression() = 0;
   virtual bool IsEmpty() { return false; } // 虚函数，判断是否为空表达式（no_expr）

#ifdef Expression_EXTRAS
   Expression_EXTRAS
#endif
};


// 定义简单语法族 - Case（case语句的分支）
typedef class Case_class *Case;

class Case_class : public tree_node {
public:
   tree_node *copy()     { return copy_Case(); }
   virtual Case copy_Case() = 0;

#ifdef Case_EXTRAS
   Case_EXTRAS
#endif
};


// 定义列表语法族（List Phylum） - 用于存储同类节点的链表

// 定义列表语法族 - Classes（类定义列表）
typedef list_node<Class_> Classes_class;
typedef Classes_class *Classes;


// 定义列表语法族 - Features（特征列表）
typedef list_node<Feature> Features_class;
typedef Features_class *Features;


// 定义列表语法族 - Formals（形式参数列表）
typedef list_node<Formal> Formals_class;
typedef Formals_class *Formals;


// 定义列表语法族 - Expressions（表达式列表）
typedef list_node<Expression> Expressions_class;
typedef Expressions_class *Expressions;


// 定义列表语法族 - Cases（case分支列表）
typedef list_node<Case> Cases_class;
typedef Cases_class *Cases;


// 定义具体构造器（Constructor）类
// 构造器是语法族的具体实现，代表AST中的具体节点类型

// 定义构造器 - program（程序）
class program_class : public Program_class {
public:
   Classes classes; // 程序包含的类定义列表
public:
   program_class(Classes a1) {
      classes = a1;
   }
   Program copy_Program();
   void dump(ostream& stream, int n); // 用于打印AST结构

#ifdef Program_SHARED_EXTRAS
   Program_SHARED_EXTRAS
#endif
#ifdef program_EXTRAS
   program_EXTRAS
#endif
};

class attr_class; // 前向声明属性类

// 定义构造器 - class_（类定义）
class class__class : public Class__class {
public:
   Symbol name;     // 类名
   Symbol parent;   // 父类名
   Features features; // 类中包含的特征（方法和属性）列表
   Symbol filename; // 定义该类的源文件名
public:
   class__class(Symbol a1, Symbol a2, Features a3, Symbol a4) {
      name = a1;
      parent = a2;
      features = a3;
      filename = a4;
   }
   Class_ copy_Class_();
   void dump(ostream& stream, int n);

#ifdef Class__SHARED_EXTRAS
   Class__SHARED_EXTRAS
#endif
#ifdef class__EXTRAS
   class__EXTRAS
#endif
};

class CgenNode; // 前向声明代码生成节点类

// 定义构造器 - method（方法）
class method_class : public Feature_class {
public:
   Symbol name;        // 方法名
   Formals formals;    // 形式参数列表
   Symbol return_type; // 返回类型
   Expression expr;    // 方法体表达式
public:
   method_class(Symbol a1, Formals a2, Symbol a3, Expression a4) {
      name = a1;
      formals = a2;
      return_type = a3;
      expr = a4;
   }
   Feature copy_Feature();
   void dump(ostream& stream, int n);
   bool IsMethod() { return true; } // 重写，表明此特征是方法
   void code(ostream& stream, CgenNode* class_node); // 代码生成函数
   int GetArgNum() { // 计算方法的参数数量
      int ret = 0;
      for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
         ++ret;
      }
      return ret;
   }
#ifdef Feature_SHARED_EXTRAS
   Feature_SHARED_EXTRAS
#endif
#ifdef method_EXTRAS
   method_EXTRAS
#endif
};


// 定义构造器 - attr（属性）
class attr_class : public Feature_class {
public:
   Symbol name;        // 属性名
   Symbol type_decl;   // 属性类型声明
   Expression init;    // 属性初始化表达式
public:
   attr_class(Symbol a1, Symbol a2, Expression a3) {
      name = a1;
      type_decl = a2;
      init = a3;
   }
   Feature copy_Feature();
   void dump(ostream& stream, int n);
   bool IsMethod() { return false; } // 重写，表明此特征是属性
#ifdef Feature_SHARED_EXTRAS
   Feature_SHARED_EXTRAS
#endif
#ifdef attr_EXTRAS
   attr_EXTRAS
#endif
};


// 定义构造器 - formal（形式参数）
class formal_class : public Formal_class {
public:
   Symbol name;      // 参数名
   Symbol type_decl; // 参数类型
public:
   formal_class(Symbol a1, Symbol a2) {
      name = a1;
      type_decl = a2;
   }
   Formal copy_Formal();
   void dump(ostream& stream, int n);
   Symbol GetName() { return name; } // 实现获取参数名
#ifdef Formal_SHARED_EXTRAS
   Formal_SHARED_EXTRAS
#endif
#ifdef formal_EXTRAS
   formal_EXTRAS
#endif
};


// 定义构造器 - branch（case语句分支）
class branch_class : public Case_class {
public:
   Symbol name;      // 分支变量名
   Symbol type_decl; // 分支变量类型
   Expression expr;  // 分支对应的表达式
public:
   branch_class(Symbol a1, Symbol a2, Expression a3) {
      name = a1;
      type_decl = a2;
      expr = a3;
   }
   Case copy_Case();
   void dump(ostream& stream, int n);

#ifdef Case_SHARED_EXTRAS
   Case_SHARED_EXTRAS
#endif
#ifdef branch_EXTRAS
   branch_EXTRAS
#endif
};


// 定义构造器 - assign（赋值表达式）
class assign_class : public Expression_class {
public:
   Symbol name;  // 被赋值的变量名
   Expression expr; // 要赋值的表达式
public:
   assign_class(Symbol a1, Expression a2) {
      name = a1;
      expr = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef assign_EXTRAS
   assign_EXTRAS
#endif
};


// 定义构造器 - static_dispatch（静态分派/调用）
class static_dispatch_class : public Expression_class {
public:
   Expression expr;       // 调用对象表达式
   Symbol type_name;      // 静态指定的目标类型名
   Symbol name;           // 要调用的方法名
   Expressions actual;    // 实际参数表达式列表
public:
   static_dispatch_class(Expression a1, Symbol a2, Symbol a3, Expressions a4) {
      expr = a1;
      type_name = a2;
      name = a3;
      actual = a4;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   std::vector<Expression> GetActuals() { // 将参数列表转换为vector方便处理
      std::vector<Expression> ret;
      for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
         ret.push_back(actual->nth(i));
      }
      return ret;
   }
#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef static_dispatch_EXTRAS
   static_dispatch_EXTRAS
#endif
};


// 定义构造器 - dispatch（动态分派/调用）
class dispatch_class : public Expression_class {
public:
   Expression expr;    // 调用对象表达式
   Symbol name;        // 要调用的方法名
   Expressions actual; // 实际参数表达式列表
public:
   dispatch_class(Expression a1, Symbol a2, Expressions a3) {
      expr = a1;
      name = a2;
      actual = a3;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   std::vector<Expression> GetActuals() { // 将参数列表转换为vector方便处理
      std::vector<Expression> ret;
      for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
         ret.push_back(actual->nth(i));
      }
      return ret;
   }
#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef dispatch_EXTRAS
   dispatch_EXTRAS
#endif
};


// 定义构造器 - cond（条件表达式，if-then-else）
class cond_class : public Expression_class {
public:
   Expression pred;     // 条件判断表达式
   Expression then_exp; // 条件为真时执行的表达式
   Expression else_exp; // 条件为假时执行的表达式
public:
   cond_class(Expression a1, Expression a2, Expression a3) {
      pred = a1;
      then_exp = a2;
      else_exp = a3;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef cond_EXTRAS
   cond_EXTRAS
#endif
};


// 定义构造器 - loop（循环表达式，while）
class loop_class : public Expression_class {
public:
   Expression pred; // 循环条件表达式
   Expression body; // 循环体表达式
public:
   loop_class(Expression a1, Expression a2) {
      pred = a1;
      body = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef loop_EXTRAS
   loop_EXTRAS
#endif
};


// 定义构造器 - typcase（类型case表达式）
class typcase_class : public Expression_class {
public:
   Expression expr;  // 被匹配的表达式
   Cases cases;      // case分支列表
public:
   typcase_class(Expression a1, Cases a2) {
      expr = a1;
      cases = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   std::vector<branch_class*> GetCases() { // 将分支列表转换为vector方便处理
      std::vector<branch_class*> ret;
      for (int i = cases->first(); cases->more(i); i = cases->next(i)) {
         ret.push_back((branch_class*)cases->nth(i));
      }
      return ret;
   }
#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef typcase_EXTRAS
   typcase_EXTRAS
#endif
};


// 定义构造器 - block（块表达式，顺序执行）
class block_class : public Expression_class {
public:
   Expressions body; // 块中的表达式列表
public:
   block_class(Expressions a1) {
      body = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef block_EXTRAS
   block_EXTRAS
#endif
};


// 定义构造器 - let（let表达式，引入局部变量）
class let_class : public Expression_class {
public:
   Symbol identifier; // 变量标识符
   Symbol type_decl;  // 变量类型声明
   Expression init;   // 变量初始化表达式
   Expression body;   // let表达式体
public:
   let_class(Symbol a1, Symbol a2, Expression a3, Expression a4) {
      identifier = a1;
      type_decl = a2;
      init = a3;
      body = a4;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef let_EXTRAS
   let_EXTRAS
#endif
};


// 定义构造器 - plus（加法表达式）
class plus_class : public Expression_class {
public:
   Expression e1; // 左操作数
   Expression e2; // 右操作数
public:
   plus_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef plus_EXTRAS
   plus_EXTRAS
#endif
};


// 定义构造器 - sub（减法表达式）
class sub_class : public Expression_class {
public:
   Expression e1; // 左操作数
   Expression e2; // 右操作数
public:
   sub_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef sub_EXTRAS
   sub_EXTRAS
#endif
};


// 定义构造器 - mul（乘法表达式）
class mul_class : public Expression_class {
public:
   Expression e1; // 左操作数
   Expression e2; // 右操作数
public:
   mul_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef mul_EXTRAS
   mul_EXTRAS
#endif
};


// 定义构造器 - divide（除法表达式）
class divide_class : public Expression_class {
public:
   Expression e1; // 左操作数
   Expression e2; // 右操作数
public:
   divide_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef divide_EXTRAS
   divide_EXTRAS
#endif
};


// 定义构造器 - neg（取负表达式）
class neg_class : public Expression_class {
public:
   Expression e1; // 操作数
public:
   neg_class(Expression a1) {
      e1 = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef neg_EXTRAS
   neg_EXTRAS
#endif
};


// 定义构造器 - lt（小于比较表达式）
class lt_class : public Expression_class {
public:
   Expression e1; // 左操作数
   Expression e2; // 右操作数
public:
   lt_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef lt_EXTRAS
   lt_EXTRAS
#endif
};


// 定义构造器 - eq（相等比较表达式）
class eq_class : public Expression_class {
public:
   Expression e1; // 左操作数
   Expression e2; // 右操作数
public:
   eq_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef eq_EXTRAS
   eq_EXTRAS
#endif
};


// 定义构造器 - leq（小于等于比较表达式）
class leq_class : public Expression_class {
public:
   Expression e1; // 左操作数
   Expression e2; // 右操作数
public:
   leq_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef leq_EXTRAS
   leq_EXTRAS
#endif
};


// 定义构造器 - comp（取反表达式，not）
class comp_class : public Expression_class {
public:
   Expression e1; // 操作数
public:
   comp_class(Expression a1) {
      e1 = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef comp_EXTRAS
   comp_EXTRAS
#endif
};


// 定义构造器 - int_const（整型常量）
class int_const_class : public Expression_class {
public:
   Symbol token; // 存储整数值的符号（通常为字符串，需要转换为整数）
public:
   int_const_class(Symbol a1) {
      token = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef int_const_EXTRAS
   int_const_EXTRAS
#endif
};


// 定义构造器 - bool_const（布尔常量）
class bool_const_class : public Expression_class {
public:
   Boolean val; // 布尔值（true或false）
public:
   bool_const_class(Boolean a1) {
      val = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef bool_const_EXTRAS
   bool_const_EXTRAS
#endif
};


// 定义构造器 - string_const（字符串常量）
class string_const_class : public Expression_class {
public:
   Symbol token; // 存储字符串值的符号
public:
   string_const_class(Symbol a1) {
      token = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef string_const_EXTRAS
   string_const_EXTRAS
#endif
};


// 定义构造器 - new_（对象实例化表达式）
class new__class : public Expression_class {
public:
   Symbol type_name; // 要实例化的类型名
public:
   new__class(Symbol a1) {
      type_name = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef new__EXTRAS
   new__EXTRAS
#endif
};


// 定义构造器 - isvoid（判断是否为void的表达式）
class isvoid_class : public Expression_class {
public:
   Expression e1; // 要检查的表达式
public:
   isvoid_class(Expression a1) {
      e1 = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef isvoid_EXTRAS
   isvoid_EXTRAS
#endif
};


// 定义构造器 - no_expr（空表达式，用于无初始化值等情况）
class no_expr_class : public Expression_class {
public:
public:
   no_expr_class() {
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   bool IsEmpty() { return true; } // 重写，表明此为空表达式
#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef no_expr_EXTRAS
   no_expr_EXTRAS
#endif
};


// 定义构造器 - object（对象标识符表达式，变量引用）
class object_class : public Expression_class {
public:
   Symbol name; // 变量名
public:
   object_class(Symbol a1) {
      name = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef object_EXTRAS
   object_EXTRAS
#endif
};


// 定义接口函数原型（辅助函数，用于创建AST节点和列表）
// 这些函数通常实现在另一个文件（如cool-tree.cc）中

// 列表创建函数：创建空列表、单元素列表、连接两个列表
Classes nil_Classes();
Classes single_Classes(Class_);
Classes append_Classes(Classes, Classes);
Features nil_Features();
Features single_Features(Feature);
Features append_Features(Features, Features);
Formals nil_Formals();
Formals single_Formals(Formal);
Formals append_Formals(Formals, Formals);
Expressions nil_Expressions();
Expressions single_Expressions(Expression);
Expressions append_Expressions(Expressions, Expressions);
Cases nil_Cases();
Cases single_Cases(Case);
Cases append_Cases(Cases, Cases);

// 节点创建函数：用于创建具体类型的AST节点
Program program(Classes);
Class_ class_(Symbol, Symbol, Features, Symbol);
Feature method(Symbol, Formals, Symbol, Expression);
Feature attr(Symbol, Symbol, Expression);
Formal formal(Symbol, Symbol);
Case branch(Symbol, Symbol, Expression);
Expression assign(Symbol, Expression);
Expression static_dispatch(Expression, Symbol, Symbol, Expressions);
Expression dispatch(Expression, Symbol, Expressions);
Expression cond(Expression, Expression, Expression);
Expression loop(Expression, Expression);
Expression typcase(Expression, Cases);
Expression block(Expressions);
Expression let(Symbol, Symbol, Expression, Expression);
Expression plus(Expression, Expression);
Expression sub(Expression, Expression);
Expression mul(Expression, Expression);
Expression divide(Expression, Expression);
Expression neg(Expression);
Expression lt(Expression, Expression);
Expression eq(Expression, Expression);
Expression leq(Expression, Expression);
Expression comp(Expression);
Expression int_const(Symbol);
Expression bool_const(Boolean);
Expression string_const(Symbol);
Expression new_(Symbol);
Expression isvoid(Expression);
Expression no_expr();
Expression object(Symbol);


#endif // 结束条件编译指令 #ifndef COOL_TREE_H