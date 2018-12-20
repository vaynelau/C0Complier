#ifndef _LEXICAL_ANALYSIS_H_
#define _LEXICAL_ANALYSIS_H_
#include <cstdio>
#include <string>
#include <set>
#include "table.h"

using namespace std;

extern string src_file_name;
extern FILE *src_file;
extern int ch;
extern int lcnt, chcnt; // 记录当前字符所在的行数和列数
extern string id; //记录标识符的名字或特殊符号
extern symbol sy; //记录当前单词符号的类型
extern int inum; //记录整数常量或字符常量的值
extern int sleng; //记录字符串长度

void lexcial_init();
void nextch();
void insymbol();
void print_symbol();

#endif // !_LEXICAL_ANALYSIS_H_
