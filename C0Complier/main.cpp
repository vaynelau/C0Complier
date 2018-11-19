#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <string>
#include "lexical_analysis.h"
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

int main(int argc, char *argv[])
{
    puts("Please input the source file name:");
    getline(cin, sourcefilename);
    if (sourcefilename.empty()) {
        sourcefilename = "16061175_test.txt";
    }
    psin = fopen(sourcefilename.c_str(), "r");
    setup();
    while (true) {
        insymbol();
    }
    return 0;
}