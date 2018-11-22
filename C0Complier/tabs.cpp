#include <string>
#include <map>
#include <vector>
#include <set>
#include "tabs.h"
#include "error.h"

using namespace std;

map<string, symbol> ksy; //保留字表
map<char, symbol> sps; //特殊字符表
vector<tabitem> tab; //符号表
int t;
vector<btabitem> btab; //分程序表
int b;
vector<string> stab; //字符串常量表
int sx; //字符串常量表索引

symbol s1[] = {
    ifsy, whilesy, lbrace, ident, semicolon, switchsy, returnsy
};
set<symbol> statbegsys(s1, s1 + sizeof(s1) / sizeof(s1[0]));

symbol s2[] = {
    eql, neq, gtr, geq, lss, leq
};
set<symbol> relationop(s2, s2 + sizeof(s2) / sizeof(s2[0]));

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
    //ksy["main"] = mainsy;
    //ksy["scanf"] = scanfsy;
    //ksy["printf"] = printfsy;

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

int loc(string name)
{
    tab[0].name = name;
    int j = btab[b].last;
    while (tab[j].name != name) {
        j = tab[j].link;
    }
    if (j != 0) {
        return j;
    }
    j = btab[0].last;
    while (tab[j].name != name) {
        j = tab[j].link;
    }
    return j;
}



void tabinit(string name, objtyp obj, types typ, int adr)
{
    ++t;
    tabitem item;
    item.name = name;
    item.link = (t > 0) ? (t - 1) : 0;
    item.obj = obj;
    item.typ = typ;
    item.adr = adr;
    item.ref = 0;
    item.lev = 0;
    item.arrcnt = 0;
    tab.push_back(item);
}


void enter(string name, objtyp obj, types typ, int arrcnt)
{
    tab[0].name = name;
    int i, j;
    int btmp = (obj == function) ? 0 : b;
    i = j = btab[btmp].last;
    while (tab[j].name != name) {
        j = tab[j].link;
    }
    if (j != 0) {
        error(19); //标识符重定义错误
    }
    else {
        ++t;
        tabitem item;
        item.name = name;
        item.link = i;
        item.obj = obj;
        item.typ = typ;
        item.arrcnt = arrcnt;
        item.lev = btmp;
        item.adr = 0;
        item.ref = 0;
        tab.push_back(item);
        btab[btmp].last = t;
    }

}
