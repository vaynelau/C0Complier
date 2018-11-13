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

char ch; //记录从源程序中读取的上一个字符
string id; //记录标识符的名字或特殊符号
symbol sy; //记录当前单词符号的类型
int inum; //记录整数常量或字符常量的值
int sleng; //记录字符串长度

map<string, symbol> ksy; //保留字表
map<char, symbol> sps; //特殊字符表
vector<string> stab; //字符串常量表
int sx; //字符串常量表索引
int sycnt; //统计已分析的单词个数
int lcnt, chcnt; // 记录当前字符所在的行数和列数
bool errflag;

const char *symstr[] = { //单词类别码对应的助记符，顺序应该与symbol成员定义的顺序一致
    "intcon", "charcon", "stringcon",
    "plus", "minus", "times", "idiv",
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
    sycnt = 0;
    lcnt = 1;
    chcnt = 0;

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
    static bool eofflag = false, eolnflag = false;

    if (eofflag) {
        exit(0);
    }
    else if (eolnflag) {
        lcnt++;
        chcnt = 0;
        eolnflag = false;
    }

    ch = fgetc(psin);
    chcnt++;
    if (ch == EOF) {
        eofflag = true;
    }
    else if (ch == '\n') {
        eolnflag = true;
    }
}

void error(int n)
{
    errflag = true;
    printf("%s:%d:%d: error: ", sourcefile.c_str(), lcnt, chcnt);
    switch (n) {
    case 0:
        puts("illegal integer constant with a leading '0'");
        break;
    case 1:
        printf("illegal character constant '%c'\n", ch);
        nextch();
        break;
    case 2:
        puts("missing terminating ' character");
        nextch();
        break;
    case 3:
        puts("missing terminating \" character");
        nextch();
        break;
    case 4:
        puts("illegal symbol '!' in program");
        break;
    case 5:
        printf("illegal symbol '%c' in program\n", ch);
        nextch();
        break;
    default:
        break;
    }
}

void printsymbol()
{
    switch (sy) {
    case intcon:
        printf("%-3d %-10s %d\n", ++sycnt, symstr[sy], inum);
        break;
    case charcon:
        printf("%-3d %-10s \'%c\'(%d)\n", ++sycnt, symstr[sy], inum, inum);
        break;
    case stringcon:
        printf("%-3d %-10s \"%s\"(length: %d)\n", ++sycnt, symstr[sy], stab[inum].c_str(), sleng);
        break;
    default:
        printf("%-3d %-10s %s\n", ++sycnt, symstr[sy], id.c_str());
        break;
    }
}

void insymbol()
{
    int k = 0;
    id.clear();
    errflag = false;

    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == EOF) {
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
    else if (isdigit(ch)) { //无符号整数常量
        if (ch != '0') {
            inum = 0;
            sy = intcon;
            do {
                inum = inum * 10 + ch - '0';
                nextch();
            } while (isdigit(ch));
        }
        else {
            nextch();
            if (isdigit(ch)) {
                error(0); //整数前导零错误
            }
            else {
                inum = 0;
                sy = intcon;
            }
        }
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
                error(2); //字符常量后无单引号
            }
        }
        else {
            error(1);  //不合法的字符常量错误
        }
    }
    else if (ch == '\"') { //字符串字面量
        k = 0;
        nextch();
        while (ch != '\"') {
            if (ch >= 32 && ch <= 126) {
                id += ch;
                k++;
                nextch();
            }
            else {
                errflag = true;
                break;
            }
        }
        if (!errflag) {
            sy = stringcon;
            stab.push_back(id);
            inum = ++sx;
            sleng = k;
            nextch();
        }
        else {
            error(3); //字符串中含有非法字符
        }
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
            error(4); //不合法的特殊符号
        }
    }
    else if (sps.count(ch)) { //其他合法的特殊符号
        id += ch;
        sy = sps[ch];
        nextch();
    }
    else {
        error(5); //不合法的特殊符号
    }

    if (!errflag) {
        //printsymbol();
    }
}