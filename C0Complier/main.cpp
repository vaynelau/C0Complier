#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include "lexical_analysis.h"
#include "syntax_analysis.h"
#include "tabs.h"

using namespace std;


string sourcefilename; //源代码文件名
FILE *psin; //源代码文件指针

map<string, symbol> ksy; //保留字表
map<char, symbol> sps; //特殊字符表
vector<tabitem> tab; //符号表
int t;
vector<btabitem> btab; //分程序表
int b;
vector<string> stab; //字符串常量表
int sx; //字符串常量表索引

void setup()
{
    t = -1;
    b = -1;
    sx = -1;

    ksy["const"] = constsy;
    ksy["if"] = ifsy;
    ksy["switch"] = switchsy;
    ksy["case"] = casesy;
    ksy["default"] = defaultsy;
    ksy["while"] = whilesy;
    ksy["return"] = returnsy;
    ksy["int"] = intsy;
    ksy["char"] = charsy;
    ksy["void"] = voidsy;
    ksy["main"] = mainsy;
    ksy["scanf"] = scanfsy;
    ksy["printf"] = printfsy;

    sps['+'] = _plus;
    sps['-'] = _minus;
    sps['*'] = times;
    sps['/'] = idiv;
    sps['('] = lparent;
    sps[')'] = rparent;
    sps['['] = lbracket;
    sps[']'] = rbracket;
    sps['{'] = lbrace;
    sps['}'] = rbrace;
    sps[','] = comma;
    sps[';'] = semicolon;
    sps[':'] = colon;
}



int main(int argc, char *argv[])
{
    puts("Please input the source file name:");
    getline(cin, sourcefilename);
    if (sourcefilename.empty()) {
        sourcefilename = "16061175_test.txt";
    }
    psin = fopen(sourcefilename.c_str(), "r");
    setup();
    lexcial_init();
    syntax_analysis();
    

    return 0;
}