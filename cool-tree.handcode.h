//
// 文件名: cool-tree-handcode.h
// 描述: 定义Cool语言抽象语法树(AST)节点的基类和共享功能
//       该头文件必须被首先包含
//

#ifndef COOL_TREE_HANDCODE_H
#define COOL_TREE_HANDCODE_H

#include <iostream>
#include "tree.h"        // 包含树结构的基本定义
#include "cool.h"        // 包含Cool语言的基本定义
#include "stringtab.h"   // 包含字符串表处理功能

// 定义当前行号全局变量，用于词法分析/语法分析时记录行号
#define yylineno curr_lineno;
extern int yylineno;

// 前向声明Environment类，用于代码生成时的环境管理
class Environment;

// Boolean类型的辅助函数
inline Boolean copy_Boolean(Boolean b) {return b; }           // 复制Boolean值
inline void assert_Boolean(Boolean) {}                        // 断言Boolean值(空实现)
inline void dump_Boolean(ostream& stream, int padding, Boolean b)  // 输出Boolean值到流
	{ stream << pad(padding) << (int) b << "\n"; }

// Symbol类型的辅助函数声明
void dump_Symbol(ostream& stream, int padding, Symbol b);  // 输出Symbol到流
void assert_Symbol(Symbol b);                               // 断言Symbol值
Symbol copy_Symbol(Symbol b);                               // 复制Symbol

// Cool语言AST节点类的前向声明和类型定义
class Program_class;              // 程序根节点
typedef Program_class *Program;   // 程序节点指针类型

class Class__class;               // 类定义节点
typedef Class__class *Class_;     // 类节点指针类型

class Feature_class;              // 类特征(方法或属性)节点
typedef Feature_class *Feature;   // 特征节点指针类型

class Formal_class;               // 形式参数节点
typedef Formal_class *Formal;     // 形式参数指针类型

class Expression_class;           // 表达式节点
typedef Expression_class *Expression;  // 表达式指针类型

class Case_class;                 // case分支节点
typedef Case_class *Case;         // case分支指针类型

// 列表类型定义，用于存储多个AST节点
typedef list_node<Class_> Classes_class;    // 类列表
typedef Classes_class *Classes;             // 类列表指针

typedef list_node<Feature> Features_class;  // 特征列表
typedef Features_class *Features;           // 特征列表指针

typedef list_node<Formal> Formals_class;     // 形参列表
typedef Formals_class *Formals;              // 形参列表指针

typedef list_node<Expression> Expressions_class;  // 表达式列表
typedef Expressions_class *Expressions;           // 表达式列表指针

typedef list_node<Case> Cases_class;         // case分支列表
typedef Cases_class *Cases;                  // case分支列表指针

// Program类的额外方法声明宏
#define Program_EXTRAS                          \
virtual void cgen(ostream&) = 0;		/* 纯虚函数：生成代码 */ \
virtual void dump_with_types(ostream&, int) = 0; /* 纯虚函数：带类型信息输出 */

// program类的具体方法声明宏
#define program_EXTRAS                          \
void cgen(ostream&);     			/* 生成代码的具体实现 */ \
void dump_with_types(ostream&, int);            /* 带类型信息输出的具体实现 */

// Class__类的额外方法声明宏
#define Class__EXTRAS                   \
virtual Symbol get_name() = 0;  	/* 纯虚函数：获取类名 */ \
virtual Symbol get_parent() = 0;    	/* 纯虚函数：获取父类名 */ \
virtual Symbol get_filename() = 0;      /* 纯虚函数：获取文件名 */ \
virtual void dump_with_types(ostream&,int) = 0; /* 纯虚函数：带类型信息输出 */

// class__类的具体方法声明宏
#define class__EXTRAS                                  \
Symbol get_name()   { return name; }		       /* 获取类名实现 */ \
Symbol get_parent() { return parent; }     	       /* 获取父类名实现 */ \
Symbol get_filename() { return filename; }             /* 获取文件名实现 */ \
void dump_with_types(ostream&,int);                    /* 带类型信息输出实现 */

// Feature类的额外方法声明宏
#define Feature_EXTRAS                                        \
virtual void dump_with_types(ostream&,int) = 0; /* 纯虚函数：带类型信息输出 */

// Feature类的共享方法声明宏
#define Feature_SHARED_EXTRAS                                       \
void dump_with_types(ostream&,int);    /* 带类型信息输出的具体实现 */

// Formal类的额外方法声明宏
#define Formal_EXTRAS                              \
virtual void dump_with_types(ostream&,int) = 0; /* 纯虚函数：带类型信息输出 */

// formal类的具体方法声明宏
#define formal_EXTRAS                           \
void dump_with_types(ostream&,int); /* 带类型信息输出的具体实现 */

// Case类的额外方法声明宏
#define Case_EXTRAS                             \
virtual void dump_with_types(ostream& ,int) = 0; /* 纯虚函数：带类型信息输出 */

// branch类的具体方法声明宏
#define branch_EXTRAS                                   \
void dump_with_types(ostream& ,int); /* 带类型信息输出的具体实现 */

// Expression类的额外成员和方法声明宏
#define Expression_EXTRAS                    \
Symbol type;                                 /* 表达式类型符号 */ \
Symbol get_type() { return type; }           /* 获取表达式类型 */ \
Expression set_type(Symbol s) { type = s; return this; } /* 设置表达式类型 */ \
virtual void code(ostream&, Environment) = 0; /* 纯虚函数：生成表达式代码 */ \
virtual void dump_with_types(ostream&,int) = 0;  /* 纯虚函数：带类型信息输出 */ \
void dump_type(ostream&, int);               /* 输出类型信息 */ \
Expression_class() { type = (Symbol) NULL; }  /* 构造函数：初始化类型为NULL */

// Expression类的共享方法声明宏
#define Expression_SHARED_EXTRAS           \
void code(ostream&, Environment); 			   /* 生成表达式代码的具体实现 */ \
void dump_with_types(ostream&,int); /* 带类型信息输出的具体实现 */

#endif