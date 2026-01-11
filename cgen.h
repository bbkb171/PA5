#include <assert.h>
#include <stdio.h>
#include <stack>
#include <vector>
#include <list>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"

// 枚举类型，用于标记类是否为基本类（如Object、Int等）
enum Basicness {
    Basic,    // 基本类
    NotBasic  // 非基本类（用户自定义类）
};
#define TRUE 1
#define FALSE 0

// 外部声明的符号，表示无类
extern Symbol No_class;

// 前向声明CgenClassTable类
class CgenClassTable;
// 定义CgenClassTable的指针类型
typedef CgenClassTable *CgenClassTableP;

// 前向声明CgenNode类
class CgenNode;
// 定义CgenNode的指针类型
typedef CgenNode *CgenNodeP;

// 代码生成类表，继承自符号表（键为Symbol，值为CgenNode）
class CgenClassTable : public SymbolTable<Symbol,CgenNode> {
private:
    List<CgenNode> *nds; // 类节点列表
    ostream& str; // 输出流引用，用于生成汇编代码
    int stringclasstag; // String类的标签ID
    int intclasstag;    // Int类的标签ID
    int boolclasstag;   // Bool类的标签ID
    // 存储所有类节点的向量，按某种顺序（如DFS顺序）排列
    std::vector<CgenNode*> m_class_nodes;
    // 映射：类名(Symbol) -> 类标签(整数ID)
    std::map<Symbol, int> m_class_tags;

// 以下是生成代码的私有方法，分别负责不同部分的代码生成
    // 生成全局数据段代码（如常量、原型对象等）
    void code_global_data();
    // 生成全局文本段代码（如GC选择、方法代码等）
    void code_global_text();
    // 生成布尔常量（true和false）的定义代码
    void code_bools(int);
    // 生成垃圾回收器选择代码
    void code_select_gc();
    // 生成常量定义（可能包括字符串常量等）
    void code_constants();
    // 生成类名表（一个字符串数组，用于对象类型标识）
    void code_class_nameTab();
    // 生成类对象表（包含每个类原型对象和初始化方法的地址）
    void code_class_objTab();
    // 生成分发表（即虚函数表，每个类一个）
    void code_dispatchTabs();
    // 生成所有类的原型对象（包含对象布局信息）
    void code_protObjs();
    // 生成所有类的初始化方法（用于初始化原型对象）
    void code_class_inits();
    // 生成所有类的方法的实际代码
    void code_class_methods();

// 以下方法用于从类列表构建继承图
    // 安装基本类（Object, IO, Int, Bool, String）到类表中
    void install_basic_classes();
    // 安装单个类节点到类表中
    void install_class(CgenNodeP nd);
    // 安装一组类（用户定义的类）到类表中
    void install_classes(Classes cs);
    // 构建继承树结构，建立父子关系
    void build_inheritance_tree();
    // 设置指定节点的父子关系
    void set_relations(CgenNodeP nd);

public:
    // 构造函数，传入类列表和输出流
    CgenClassTable(Classes, ostream& str);
    // 执行代码生成的主要入口点，完成后退出作用域
    void Execute() {
        code(); // 生成代码
        exitscope(); // 退出符号表作用域
    }
    // 代码生成的总调度方法
    void code();
    // 获取继承树的根节点（通常是Object类）
    CgenNodeP root();
    // 获取所有类节点的向量
    std::vector<CgenNode*> GetClassNodes();
    // 获取类名到类标签的映射
    std::map<Symbol, int> GetClassTags();
    // 根据类名获取对应的类节点
    CgenNode* GetClassNode(Symbol class_name) {
        // 确保m_class_nodes和m_class_tags已填充
        GetClassNodes();
        // 通过类标签作为索引在m_class_nodes中查找
        return m_class_nodes[m_class_tags[class_name]];
    }
};

// 代码生成类节点，继承自class__class（Cool语言的类AST节点）
class CgenNode : public class__class {
private:
    CgenNodeP parentnd;   // 指向父类节点的指针
    List<CgenNode> *children; // 子类节点列表
    Basicness basic_status; // 标记该类是否为基本类
    // 以下成员缓存了类的各种信息，避免重复计算

public:
    // 构造函数，传入Cool类AST节点、基本类状态和类表指针
    CgenNode(Class_ c, Basicness bstatus, CgenClassTableP class_table);

    // 添加一个子节点
    void add_child(CgenNodeP child);

    // 获取子节点列表
    List<CgenNode>* get_children() {
        return children;
    }

    // 获取子节点向量（更现代的接口）
    std::vector<CgenNode*> GetChildren() {
        std::vector<CgenNode*> ret;
        List<CgenNode>* _children = get_children();
        // 遍历链表，将节点加入向量
        while (_children != nullptr) {
            ret.push_back(_children->hd());
            _children = _children->tl();
        }
        return ret;
    }

    // 设置父节点
    void set_parentnd(CgenNodeP p);

    // 获取父节点
    CgenNodeP get_parentnd() {
        return parentnd;
    }

    // 判断是否为基本类
    int basic() {
        return (basic_status == Basic);
    }

    // 生成该类的原型对象代码
    void code_protObj(ostream& s);
    // 生成该类的初始化方法代码
    void code_init(ostream& s);
    // 生成该类所有方法的代码
    void code_methods(ostream& s);

    // 获取该类直接定义的方法列表（缓存）
    std::vector<method_class*> GetMethods();
    std::vector<method_class*> m_methods; // 缓存：本类方法

    // 获取该类完整的（包括继承的）方法列表（缓存），按覆盖顺序排列
    std::vector<method_class*> GetFullMethods();
    std::vector<method_class*> m_full_methods; // 缓存：完整方法表（用于分发表）

    // 获取分发表中方法名到定义该方法的类名的映射（缓存）
    std::map<Symbol, Symbol> GetDispatchClassTab();
    std::map<Symbol, Symbol> m_dispatch_class_tab; // 缓存：方法名->定义类名

    // 获取分发表中方法名到索引的映射（缓存）
    std::map<Symbol, int> GetDispatchIdxTab();
    std::map<Symbol, int> m_dispatch_idx_tab; // 缓存：方法名->分发表索引

    // 获取该类直接定义的属性列表（缓存）
    std::vector<attr_class*> GetAttribs();
    std::vector<attr_class*> m_attribs; // 缓存：本类属性

    // 获取该类完整的（包括继承的）属性列表（缓存），按继承顺序排列
    std::vector<attr_class*> GetFullAttribs();
    std::vector<attr_class*> m_full_attribs; // 缓存：完整属性列表（用于对象布局）

    // 获取属性名到其在对象布局中索引的映射（缓存）
    std::map<Symbol, int> GetAttribIdxTab();
    std::map<Symbol, int> m_attrib_idx_tab; // 缓存：属性名->对象属性槽索引

    // 获取从根类（Object）到当前类的继承链节点列表（缓存）
    std::vector<CgenNode*> GetInheritance();
    std::vector<CgenNode*> inheritance; // 缓存：继承链

    // 该类的唯一标签ID
    int class_tag;
};

// 布尔常量类，用于生成布尔常量的代码
class BoolConst
{
private:
    int val; // 布尔值（0或1）
public:
    // 构造函数
    BoolConst(int);
    // 生成布尔常量对象的定义代码（用于原型对象）
    void code_def(ostream&, int boolclasstag);
    // 生成布尔常量的引用代码（用于代码中的字面量）
    void code_ref(ostream&) const;
};

// 环境类，用于代码生成过程中的符号管理（变量、参数、属性查找）
class Environment {
public:
    // 构造函数，初始化类节点为空
    Environment() : m_class_node(nullptr) {}

    // 进入一个新的作用域（如进入一个block）
    void EnterScope() {
        // 记录新作用域的初始长度（变量数为0）
        m_scope_lengths.push_back(0);
    }

    // 退出当前作用域
    void ExitScope() {
        // 获取当前作用域增加的变量数量
        int num_vars_in_scope = m_scope_lengths[m_scope_lengths.size() - 1];
        // 从变量索引表中移除当前作用域的所有变量
        for (int i = 0; i < num_vars_in_scope; ++i) {
            m_var_idx_tab.pop_back();
        }
        // 移除记录的作用域长度信息
        m_scope_lengths.pop_back();
    }

    // 查找属性符号，返回其在对象布局中的索引（偏移量）
    int LookUpAttrib(Symbol sym) {
        // 获取当前类节点的属性索引映射表
        std::map<Symbol, int> attrib_idx_tab = m_class_node->GetAttribIdxTab();
        // 如果找到该属性，返回其索引
        if (attrib_idx_tab.find(sym) != attrib_idx_tab.end()) {
            return attrib_idx_tab[sym];
        }
        // 未找到返回-1
        return -1;
    }

    // 查找局部变量符号，返回其相对于栈顶的偏移量（反向查找）
    // 变量表是反向存储的，最后添加的变量在末尾，但栈帧中最新变量在低地址
    int LookUpVar(Symbol sym) {
        // 从变量表末尾向前查找（即从最近添加的变量开始找）
        for (int idx = m_var_idx_tab.size() - 1; idx >= 0; --idx) {
            if (m_var_idx_tab[idx] == sym) {
                // 计算偏移量：总变量数 - 1 - 找到的索引
                // 因为栈帧中，先压栈的参数/变量在更高地址，后压栈的在更低地址
                return m_var_idx_tab.size() - 1 - idx;
            }
        }
        // 未找到返回-1
        return -1;
    }

    // 添加一个局部变量到当前作用域，返回其在变量表中的索引
    int AddVar(Symbol sym) {
        // 将变量符号加入变量表末尾
        m_var_idx_tab.push_back(sym);
        // 当前作用域的变量数量加1
        ++m_scope_lengths[m_scope_lengths.size() - 1];
        // 返回新变量的索引（在变量表中的位置）
        return m_var_idx_tab.size() - 1;
    }

    // （此方法未实现）可能用于添加一个栈障碍？
    int AddObstacle();

    // 查找参数符号，返回其相对于栈帧基址的偏移量
    // 参数是按顺序压栈的，第一个参数在栈帧底部（高地址）
    int LookUpParam(Symbol sym) {
        // 在参数表中顺序查找
        for (int idx = 0; idx < m_param_idx_tab.size(); ++idx) {
            if (m_param_idx_tab[idx] == sym) {
                // 计算偏移量：参数总数 - 1 - 索引
                // 因为第一个参数（索引0）在离栈帧基址最远的位置（偏移量最大）
                return m_param_idx_tab.size() - 1 - idx;
            }
        }
        // 未找到返回-1
        return -1;
    }

    // 添加一个参数到参数表
    int AddParam(Symbol sym) {
        // 将参数符号加入参数表末尾
        m_param_idx_tab.push_back(sym);
        // 返回新参数的索引
        return m_param_idx_tab.size() - 1;
    }

    // 记录每个作用域开始时，变量表的长度增量
    std::vector<int> m_scope_lengths;
    // 变量符号表，按添加顺序存储所有局部变量（包括不同作用域的）
    std::vector<Symbol> m_var_idx_tab;
    // 参数符号表，按顺序存储方法的形参
    std::vector<Symbol> m_param_idx_tab;
    // 指向当前代码生成所在的类节点的指针
    CgenNode* m_class_node;
};