#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include "lexical_analysis.h"
#include "error.h"
#include "table.h"

using namespace std;

int ch; //记录从源程序中读取的上一个字符
string id; //记录标识符的名字或特殊符号
symbol sy; //记录当前单词符号的类型
int inum; //记录整数常量或字符常量的值
int sleng; //记录字符串长度

int sycnt; //统计已分析的单词个数
int lcnt, chcnt; // 记录当前字符所在的行数和列数
bool errflag; //标记是否发生错误


const char *symstr[] = { //单词类别码对应的助记符，顺序应该与symbol成员定义的顺序一致
    "intcon", "charcon", "stringcon",
    "plus", "minus", "times", "idiv",
    "eql", "neq", "gtr", "geq", "lss", "leq",
    "lparent", "rparent", "lbracket", "rbracket", "lbrace", "rbrace",
    "comma", "semicolon", "colon", "becomes",
    "ident", //"mainsy", "scanfsy", "printfsy",
    "constsy", "returnsy", "intsy", "charsy", "voidsy",
    "ifsy", "switchsy", "casesy", "defaultsy", "whilesy","eofsy"
};

void lexcial_init()
{
    ch = ' ';
    sycnt = 0;
    lcnt = 1;
    chcnt = 0;
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

    ch = fgetc(src_file);
    chcnt++;
    if (ch == EOF) {
        eofflag = true;
    }
    else if (ch == '\n') {
        eolnflag = true;
    }
}



void print_symbol()
{
    switch (sy) {
    case intcon:
        printf("%-4d %-10s %d\n", ++sycnt, symstr[sy], inum);
        break;
    case charcon:
        printf("%-4d %-10s \'%c\'\n", ++sycnt, symstr[sy], inum);
        break;
    case stringcon:
        printf("%-4d %-10s \"%s\"\n", ++sycnt, symstr[sy], stab[inum].c_str());
        break;
    default:
        printf("%-4d %-10s %s\n", ++sycnt, symstr[sy], id.c_str());
        break;
    }
}

void insymbol()
{
    int k = 0;
    id.clear();
    errflag = false;

    //while (ch == ' ' || ch == '\t' || ch == '\n') {
    while (isspace(ch)) {
        nextch();
    }

    if (isalpha(ch) || ch == '_') { //标识符或保留字
        do {
            id += (char)ch;
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
        inum = 0;
        sy = intcon;
        if (ch == '0') {
            nextch();
        }
        else {
            do {
                inum = inum * 10 + ch - '0';
                nextch();
            } while (isdigit(ch));
        }
    }
    else if (ch == '\'') { //字符常量
        sy = charcon;
        nextch();
        if (isalnum(ch) || ch == '_' || ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            inum = (int)ch;

        }
        else {
            inum = 0;
            error(1);  //不合法的字符常量
        }
        nextch();
        if (ch == '\'') {
            nextch();
        }
        else {
            error(2); //字符常量后无单引号
        }
    }
    else if (ch == '\"') { //字符串字面量
        k = 0;
        nextch();
        while (ch != '\"') {
            if (ch >= 32 && ch <= 126) {
                id += (char)ch;
                if (ch == '\\') {
                    id += (char)ch;
                }
                k++;
                nextch();
            }
            else {
                errflag = true;
                break;
            }
        }
        sy = stringcon;
        stab.push_back(id);
        inum = ++sx;
        sleng = k;
        if (!errflag) {
            nextch();
        }
        else {
            error(3); //字符串不以双引号结束
        }
    }
    else if (ch == '>') { //大于号，大于等于号
        id += (char)ch;
        nextch();
        if (ch == '=') {
            id += (char)ch;
            sy = geq;
            nextch();
        }
        else {
            sy = gtr;
        }
    }
    else if (ch == '<') { //小于号，小于等于号
        id += (char)ch;
        nextch();
        if (ch == '=') {
            id += (char)ch;
            sy = leq;
            nextch();
        }
        else {
            sy = lss;
        }
    }
    else if (ch == '=') { //赋值符号，等于号
        id += (char)ch;
        nextch();
        if (ch == '=') {
            id += (char)ch;
            sy = eql;
            nextch();
        }
        else {
            sy = becomes;
        }
    }
    else if (ch == '!') { //不等号
        id += (char)ch;
        nextch();
        if (ch == '=') {
            id += (char)ch;
            sy = neq;
            nextch();
        }
        else {
            error(4); //不合法的符号'!'
            insymbol();
        }
    }
    else if (sps.count((char)ch)) { //其他合法的特殊符号
        id += (char)ch;
        sy = sps[(char)ch];
        nextch();
    }
    else if (ch == EOF) {
        sy = eofsy;
    }
    else {
        error(5); //不合法的特殊符号
        nextch();
        insymbol();
    }
    //print_symbol();
}