#include <iostream>
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include "lexical_analysis.h"
#include "syntax_analysis.h"
#include "error.h"
#include "tabs.h"

using namespace std;

void enters(string x0, objtyp x1, types x2, int x3)
{
    ++t;
    tabitem item;
    item.name = x0;
    item.link = (t > 0) ? (t - 1) : 0;
    item.obj = x1;
    item.typ = x2;
    item.adr = x3;
    item.ref = 0;
    item.lev = 0;
    item.arrcnt = 0;
    tab.push_back(item);
}


int dx, prt, prd, level;

void enter(string id, objtyp obj, types typ)
{
    tab[0].name = id;
    int i, j;
    i = j = btab[b].last;
    while (tab[j].name != id) {
        j = tab[j].link;
    }
    if (j != 0) {
        error(0); //标识符重定义错误
    }
    else {
        ++t;
        tabitem item;
        item.name = id;
        item.link = i;
        item.obj = obj;
        item.typ = typ;
        item.lev = b;
        item.adr = 0;
        item.ref = 0;
        item.arrcnt = 0;
        tab.push_back(item);
    }
}


void constdef()
{

    int sign = 1;
    insymbol();
    if (sy == intsy) {
        insymbol();
        while (true) {
            if (sy == ident) {
                enter(id, constant, ints);
                insymbol();
                if (sy = becomes) {
                    insymbol;
                }
                else {
                    error(0);
                }
                test(); //直到读到一个整数常量开始符号，逗号，分号
                if (sy == _plus || sy == _minus) {
                    sign = (sy == _minus) ? -1 : 1;
                    insymbol;
                }
                if (sy == intcon) {
                    tab[t].adr = sign * inum;
                    insymbol();
                }
                else {
                    error(0);
                }
            }
            else {
                error(0);
            }

            test();//直到读到逗号，分号
            if (sy == semicolon) {
                break;
            }
            else {
                insymbol();
            }
        }
    }
    else if (sy == charsy) {
        insymbol();
        while (true) {
            if (sy == ident) {
                enter(id, constant, chars);
                insymbol();
                if (sy = becomes) {
                    insymbol;
                }
                else {
                    error(0);
                }
                test(); //直到读到一个字符常量，逗号，分号
                if (sy == charcon) {
                    tab[t].adr = inum;
                    insymbol();
                }
                else {
                    error(0);
                }
            }
            else {
                error(0);
            }
            
            test();//直到读到逗号，分号
            if (sy == semicolon) {
                break;
            }
            else {
                insymbol();
            }
        }
    }
    else {
        error(0);
    }
    test();//直到遇到分号
    insymbol();
}
void vardef()
{

}
bool funcdef()
{

}

void test()
{

}

void syntax_analysis()
{
    enters("", variable, notyp, 0);
    enters("scanf", function, voids, 1);
    enters("printf", function, voids, 2);

    ++b;
    btabitem item;
    item.last = t;
    item.lastpar = 0;
    item.psize = 0;
    item.vsize = 0;
    btab.push_back(item);

    dx = 0;
    prt = t;
    level = 0;
    bool funcflag = false;
    bool mainflag = false;
    insymbol();

    string name;
    test();

    if (sy == constsy) {
        do {
            constdef();
        } while (sy == constsy);
    }
    else if (sy == intsy || sy == charsy || sy == voidsy) {
        do {
            insymbol();
            if (sy == ident) {
                name = id;
                insymbol();
                if (sy == lparent) {
                    funcflag = true;
                    mainflag = funcdef();
                }
                else if (!funcflag) {
                    vardef();
                }
                else {
                    error(0);
                }
            }
            else {
                error(0);
            }
        } while ((sy == intsy || sy == charsy || sy == voidsy) && !mainflag);
    }

    if (!mainflag) {
        error(0);
    }
    if (sy != eofsy) {
        error(0);
    }
}


