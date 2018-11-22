#ifndef _TABS_H_
#define _TABS_H_
#include <string>
#include <map>
#include <set>
#include <vector>

using namespace std;

typedef enum symbol {
    intcon, charcon, stringcon, //整数  字符  字符串
    _plus, _minus, times, idiv, //+  -  *  /
    eql, neq, gtr, geq, lss, leq, //==  !=  >  >=  <  <=
    lparent, rparent, lbracket, rbracket, lbrace, rbrace, //( ) [ ] { } 
    comma, semicolon, colon, becomes, //, ; : =  
    ident, //mainsy, scanfsy, printfsy,
    constsy, returnsy, intsy, charsy, voidsy,
    ifsy, switchsy, casesy, defaultsy, whilesy, eofsy
} symbol;

typedef enum objtyp {
    constant, variable, arrays, function
} objtyp;

typedef enum types {
    notyp, ints, chars, voids
} types;

typedef struct tabitem {//符号表
    string name;
    int link; //指向同一函数中上一个标识符在tab表中的位置
    objtyp obj; // 标识符种类：常量、变量、数组变量、函数等
    types typ; // 标识符类型：int型、char型、void型等
    int lev; // 标识符所在的静态层次，全局常量、变量或函数为0，局部常量、变量为1
    int ref; //函数名：指向其在btab表中的指针值  
    int arrcnt; //数组元素个数
    int adr; // 变量、形参：在运行栈s中的相对地址；函数名：相应目标代码的入口地址；整型常量：对应数值；
} tabitem;

typedef struct btabitem {
    int last, lastpar, psize, vsize;
    /*
      last指向函数最后一个标识符在tab表中的位置；
      lastpar指向函数的最后一个参数在tab表中的位置；
      psize为参数及内务信息区所占的存储单元数；
      vsize局部常量、变量、参数及内务信息区所占的存储单元总数。
    */
} btabitem;

extern map<string, symbol> ksy; //保留字表
extern map<char, symbol> sps; //特殊字符表

extern vector<tabitem> tab; //符号表
extern int t;

extern vector<btabitem> btab; //分程序表
extern int b;

extern vector<string> stab; //字符串常量表
extern int sx; //字符串常量表索引

extern set<symbol> statbegsys;
extern set<symbol> relationop;

void setup();
int loc(string name);
void tabinit(string name, objtyp obj, types typ, int adr);
void enter(string name, objtyp obj, types typ, int arrcnt);


#endif // !_TABS_H_
