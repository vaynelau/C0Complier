#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <string>
#include <map>
#include <vector>
#include "lexical_analysis.h"
using namespace std;

FILE *psin; //源代码文件指针

char ch; //记录当前从源程序中读取的一个字符
string id; //记录标识符的名字或特殊符号
symbol sy; //记录当前单词符号的类型
int inum; //记录整数常量或字符常量的值
int sleng; //记录字符串长度

map<string, symbol> ksy; //保留字表
map<char, symbol> sps; //特殊字符表
vector<string> stab; //字符串常量表
int sx; //字符串常量表索引
int cnt; //统计已分析的单词个数

const char *symstr[] = { //单词类别码对应的助记符
    "intcon", "charcon", "stringcon",
    "_plus", "_minus", "times", "idiv",
    "eql", "neq", "gtr", "geq", "lss", "leq",
    "lparent", "rparent", "lbracket", "rbracket", "lbrace", "rbrace",
    "comma", "semicolon", "colon", "becomes",
    "ident", "mainsy", "scanfsy", "printfsy",
    "constsy", "returnsy", "intsy", "charsy", "voidsy",
    "ifsy", "switchsy", "casesy", "defaultsy", "whilesy" 
};

void setup(FILE *in)
{
    psin = in; 
    sx = -1;
    ch = ' ';
    cnt = 0;

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

void nextch()
{
    do {
        ch = fgetc(psin);
        if (ch == EOF) {
            fclose(psin);
            exit(0);
        }
    } while (ch == '\n');
}

void printsy()
{
    switch (sy) {
    case intcon:
        printf("%-3d %-10s %d\n", ++cnt, symstr[sy], inum);
        break;
    case charcon:
        printf("%-3d %-10s %c\n", ++cnt, symstr[sy], inum);
        break;
    case stringcon:
        printf("%-3d %-10s %s (length: %d)\n", ++cnt, symstr[sy], stab[inum].c_str(), sleng);
        break;
    default:
        printf("%-3d %-10s %s\n", ++cnt, symstr[sy], id.c_str());
        break;
    }
}

void error()
{
    printf("Lexical Analysis Error.\n");
    fclose(psin);
    exit(1);
}

void insymbol()
{
    int k = 0;
    id.clear();

    while (ch==' ' || ch == '\t') {
        nextch();
    }
    
    if (isalpha(ch) || ch == '_') { //标识符或保留字
        do {
            id += ch;
            nextch();
        } while (isalnum(ch) || ch == '_');

        if (ksy.count(id)) {
            sy = ksy[id];
        }
        else {
            sy = ident;
        }
    }
    else if (isdigit(ch)) { //整数常量
        inum = 0;
        sy = intcon;
        do {
            inum = inum * 10 + ch - '0';
            nextch();
        } while (isdigit(ch));
    }
    else if (ch == '\'') { //字符常量
        nextch();
        if (isalnum(ch) || ch == '_' || ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            inum = (int)ch;
            nextch();
            if (ch == '\'') {
                sy = charcon;
                nextch();
            }
            else {
                error();
            }
        }
        else {
            error();
        }
    }
    else if (ch == '\"') { //字符串字面量
        k = 0;
        nextch();
        while (ch != '\"') {
            id += ch;
            k++;
            nextch();
        }
        sy = stringcon;
        stab.push_back(id);
        inum = ++sx;
        sleng = k;
        nextch();
    }
    else if (ch == '>') { //大于号，大于等于号
        id += ch;
        nextch();
        if (ch == '=') {
            id += ch;
            sy = geq;
            nextch();
        }
        else {
            sy = gtr;
        }
    }
    else if (ch == '<') { //小于号，小于等于号
        id += ch;
        nextch();
        if (ch == '=') {
            id += ch;
            sy = leq;
            nextch();
        }
        else {
            sy = lss;
        }
    }
    else if (ch == '=') { //赋值符号，等于号
        id += ch;
        nextch();
        if (ch == '=') {
            id += ch;
            sy = eql;
            nextch();
        }
        else {
            sy = becomes;
        }
    }
    else if (ch == '!') { //不等号
        id += ch;
        nextch();
        if (ch == '=') {
            id += ch;
            sy = neq;
            nextch();
        }
        else {
            error();
        }
    }
    else if (sps.count(ch)) { //其他合法的特殊符号
        id += ch; 
        sy = sps[ch];
        nextch();
    }
    else {
        error();
    }

    printsy();
}


