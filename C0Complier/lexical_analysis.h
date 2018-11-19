#ifndef _LEXICAL_ANALYSIS_H_
#define _LEXICAL_ANALYSIS_H_
#include <iostream>
#include <cstdio>
#include <string>
#include <map>
#include <vector>

using namespace std;

extern string sourcefilename;
extern FILE *psin;
extern char ch;
extern int lcnt, chcnt; // 记录当前字符所在的行数和列数

void setup();
void nextch();
void insymbol();
void printsymbol();

#endif // !_LEXICAL_ANALYSIS_H_
