#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <set>
#include "lexical_analysis.h"
#include "syntax_analysis.h"
#include "error.h"
#include "tabs.h"

using namespace std;

int dx, prt;
bool retflag;


void constdef()
{
    int line = lcnt;
    int sign = 1;
    insymbol();
    if (sy == intsy) {
        do {
            insymbol();
            if (sy == ident) {
                tab_enter(id, constant, ints, 0);
                insymbol();
                if (sy == becomes) {
                    insymbol();
                }
                else {
                    error(7);
                }
                if (sy == _plus || sy == _minus) {
                    sign = (sy == _minus) ? -1 : 1;
                    insymbol();
                }
                if (sy == intcon) {
                    tab[t].adr = sign * inum;
                    insymbol();
                }
                else {
                    error(8);
                }
            }
            else {
                error(6);
            }
        } while (sy == comma);
    }
    else if (sy == charsy) {
        do {
            insymbol();
            if (sy == ident) {
                tab_enter(id, constant, chars, 0);
                insymbol();
                if (sy == becomes) {
                    insymbol();
                }
                else {
                    error(7);
                }
                if (sy == charcon) {
                    tab[t].adr = inum;
                    insymbol();
                }
                else {
                    error(8);
                }
            }
            else {
                error(6);
            }
        } while (sy == comma);
    }
    else {
        error(9);
    }

    if (sy == semicolon) {
        insymbol();
    }
    else {
        error(13);
    }
    printf("line %d: 这是一个常量定义\n", line);
}


void vardef(types typ, string name)
{
    int line = lcnt;
    if (sy == lbracket) {
        insymbol();
        if (sy == intcon) {
            if (inum > 0) {
                tab_enter(name, arrays, typ, dx);
                dx += (typ == ints) ? (inum * 4) : inum;
            }
            else {
                error(10);//数组元素个数不大于0
            }
            insymbol();
        }
        else {
            error(8);//没有指定数组元素
        }
        if (sy == rbracket) {
            insymbol();
        }
        else {
            error(12);//缺少右中括号
        }
    }
    else {
        tab_enter(name, variable, typ, dx);
        dx += (typ == ints) ? 4 : 1;
    }

    while (sy == comma) {
        insymbol();
        if (sy == ident) {
            name = id;
            insymbol();
            if (sy == lbracket) {
                insymbol();
                if (sy == intcon) {
                    if (inum > 0) {
                        tab_enter(name, arrays, typ, dx);
                        dx += (typ == ints) ? (inum * 4) : inum;
                    }
                    else {
                        error(10);//数组元素个数不大于0
                    }
                    insymbol();
                }
                else {
                    error(11);//没有指定数组元素个数
                }
                if (sy == rbracket) {
                    insymbol();
                }
                else {
                    error(12);//缺少右中括号
                }
            }
            else {
                tab_enter(name, variable, typ, dx);
                dx += (typ == ints) ? 4 : 1;
            }
        }
        else {
            error(9); //变量无标识符
        }
    }

    if (sy == semicolon) {
        insymbol();
    }
    else {
        error(13);
    }
    printf("line %d: 这是一个变量定义\n", line);
}


types factor(int *value)
{
    types typ = ints;
    int val, sign = 1;
    int i;
    switch (sy) {
    case charcon:
        *value = inum;
        typ = chars;
        insymbol();
        break;
    case _plus:
    case _minus:
        sign = (sy == _plus) ? 1 : -1;
        insymbol();
        if (sy == intcon) {
            *value = inum * sign;
            typ = ints;
            insymbol();
        }
        else {
            error(8);
        }
        break;
    case intcon:
        *value = inum;
        typ = ints;
        insymbol();
        break;
    case lparent:
        insymbol();
        typ = ints;
        expression(&val);
        *value = val;
        if (sy == rparent) {
            insymbol();
        }
        else {
            error(14);
        }
        break;
    case ident:
        i = loc(id);
        if (i != 0) {
            switch (tab[i].obj) {
            case constant:
                *value = tab[i].adr;
                typ = tab[i].typ;
                insymbol();
                break;
            case variable:
                *value = 1;
                typ = tab[i].typ;
                insymbol();
                break;
            case arrays:
                insymbol();
                if (sy == lbracket) {
                    insymbol();
                }
                else {
                    error(22);
                }
                typ = tab[i].typ;
                expression(value);
                if (sy == rbracket) {
                    insymbol();
                }
                else {
                    error(12);
                }
                break;
            case function:
                if (tab[i].typ != voids) {
                    typ = tab[i].typ;
                    *value = 1;
                    call(i);
                }
                else {
                    error(23);
                }
                break;
            default:
                break;
            }
        }
        else {
            error(18);//未定义的标识符

        }
        break;
    default:
        error(24);
        break;
    }
    printf("line %d: 这是一个因子\n", lcnt);
    return typ;
}


types term(int *value)
{
    int val, tmp;
    types typ = ints;
    typ = factor(&val);
    symbol op;
    while (sy == times || sy == idiv) {
        typ = ints;
        op = sy;
        insymbol();
        factor(&tmp);
        if (op == times) {
            val *= tmp;
        }
        else {
            val /= tmp;
        }
    }
    *value = val;
    printf("line %d: 这是一个项\n", lcnt);
    return typ;
}


types expression(int *value)
{
    int sign = 1;
    int val, tmp;
    types typ = chars, typ1 = chars;
    if (sy == _plus || sy == _minus) {
        sign = (sy == _plus) ? 1 : -1;
        insymbol();
        typ = ints;
    }
    typ1 = term(&val);
    val *= sign;
    symbol op;
    while (sy == _plus || sy == _minus) {
        typ = ints;
        op = sy;
        insymbol();
        term(&tmp);
        if (op == _plus) {
            val += tmp;
        }
        else {
            val -= tmp;
        }
    }
    *value = val;
    printf("line %d: 这是一个表达式\n", lcnt);
    if (typ == ints || typ1 == ints) {
        return ints;
    }
    else {
        return chars;
    }
}


void ifstatement()
{
    insymbol();
    int value1, value2;
    types typ1 = ints, typ2 = ints;
    if (sy == lparent) {
        insymbol();
    }
    else {
        error(26);
    }
    typ1 = expression(&value1);
    if (relationop.count(sy)) {
        insymbol();
        typ2 = expression(&value2);
        if (typ2 != typ1) {
            error(27);
        }
    }
    else {
        if (typ1 != ints) {
            error(28);
        }
    }
    printf("line %d: 这是一个条件\n", lcnt);
    if (sy == rparent) {
        insymbol();
    }
    else {
        error(14);
    }
    statement();
    printf("line %d: 这是一个条件语句\n", lcnt - 1);
}


void whilestatement()
{
    insymbol();
    int value1, value2;
    types typ1 = ints, typ2 = ints;
    if (sy == lparent) {
        insymbol();
    }
    else {
        error(26);
    }
    typ1 = expression(&value1);
    if (relationop.count(sy)) {
        insymbol();
        typ2 = expression(&value2);
        if (typ2 != typ1) {
            error(27);
        }
    }
    else {
        if (typ1 != ints) {
            error(28);
        }
    }
    printf("line %d: 这是一个条件\n", lcnt);
    if (sy == rparent) {
        insymbol();
    }
    else {
        error(14);
    }
    statement();
    printf("line %d: 这是一个循环语句\n", lcnt - 1);
}


void onecase(types typ)
{
    insymbol();
    int sign = 1, value;
    if (sy == charcon) {
        value = inum;
        insymbol();
        if (typ != chars) {
            error(29);
        }
    }
    else {
        if (sy == _plus || sy == _minus) {
            sign = (sy == _plus) ? 1 : -1;
            insymbol();
        }
        if (sy == intcon) {
            value = sign * inum;
            insymbol();
            if (typ != ints) {
                error(29);
            }
        }
        else {
            error(8);
        }
    }

    if (sy == colon) {
        insymbol();
    }
    else {
        error(21);
    }
    statement();
    printf("line %d: 这是一个情况子语句\n", lcnt - 1);
}


void switchstatement()
{
    insymbol();
    if (sy == lparent) {
        insymbol();
    }
    else {
        error(26);
    }
    int value;
    types typ = expression(&value);
    if (sy == rparent) {
        insymbol();
    }
    else {
        error(14);
    }

    if (sy == lbrace) {
        insymbol();
    }
    else {
        error(15);
    }

    if (sy == casesy) {
        do {
            onecase(typ);
        } while (sy == casesy);
    }
    else {
        error(30);// 无case语句
    }
    if (sy == defaultsy) {
        insymbol();
        if (sy == colon) {
            insymbol();
        }
        else {
            error(21);
        }
        statement();
        printf("line %d: 这是一个缺省\n", lcnt - 1);
    }
    if (sy == rbrace) {
        insymbol();
    }
    else {
        error(16);
    }
    printf("line %d: 这是一个情况语句\n", lcnt - 1);
}


void returnstatement()
{
    types typ = ints;
    int value;
    retflag = true;
    insymbol();
    if (sy == lparent) {
        insymbol();
        typ = expression(&value);
        if (typ != tab[prt].typ) {
            error(25);//返回值类型不一致
        }
        if (sy == rparent) {
            insymbol();
        }
        else {
            error(14);
        }
    }
    printf("line %d: 这是一个返回语句\n", lcnt);
}


void assignment(int i)
{
    types typ1 = ints, typ2 = ints;
    int value1, value2;
    insymbol();
    if (tab[i].obj == arrays) {
        if (sy == lbracket) {
            insymbol();
        }
        else {
            error(22);
        }
        typ1 = expression(&value1);
        if (typ1 != ints) {
            error(31);
        }
        //else if(value<0 || value>tab[i].arrcnt-1) {
        //    error(0);//数组越界
        //}
        if (sy == rbracket) {
            insymbol();
        }
        else {
            error(12);
        }
    }
    if (sy == becomes) {
        insymbol();
    }
    else {
        error(7);
    }
    typ2 = expression(&value2);
    if (typ2 != tab[i].typ) {
        error(32);
    }
    printf("line %d: 这是一个赋值语句\n", lcnt);
}


void standfunc()
{
    int value;
    types typ = ints;
    if (id == "scanf") {
        insymbol();
        if (sy == lparent) {
            do {
                insymbol();
                if (sy == ident) {
                    int i = loc(id);
                    if (i == 0) {
                        error(18);
                    }
                    else if (tab[i].obj != variable) {
                        error(33);
                    }
                    insymbol();
                }
                else {
                    error(6);
                }
            } while (sy == comma);
        }
        else {
            error(26);
        }
        printf("line %d: 这是一个读语句\n", lcnt);
    }
    else {
        insymbol();
        if (sy == lparent) {
            insymbol();
        }
        else {
            error(26);
        }
        if (sy == stringcon) {
            insymbol();
            if (sy == comma) {
                insymbol();
                typ = expression(&value);
            }
        }
        else {
            typ = expression(&value);
        }
        printf("line %d: 这是一个写语句\n", lcnt);
    }

    if (sy == rparent) {
        insymbol();
    }
    else {
        error(14);
    }
}


void call(int i)
{

    int lastp = btab[tab[i].ref].lastpar;
    int cpos = i;
    int value;
    types typ = ints;

    insymbol();
    if (sy == lparent) {
        insymbol();
    }
    else {
        error(26);
    }

    if (sy != rparent) {
        cpos++;
        typ = expression(&value);
        if (cpos > lastp) {
            error(34); //实参过多
        }
        else {
            if (typ != tab[cpos].typ) {
                error(35);
            }
        }

        while (sy == comma) {
            insymbol();
            cpos++;
            typ = expression(&value);
            if (cpos > lastp) {
                error(34); //实参过多
            }
            else {
                if (typ != tab[cpos].typ) {
                    error(35);
                }
            }
        
        }
    }
    printf("line %d: 这是一个值参数表\n", lcnt);
    
    if (cpos < lastp) {
        error(36); //实参过少
    }
    if (sy == rparent) {
        insymbol();
    }
    else {
        error(14);
    }

    if (tab[i].typ == voids) {
        printf("line %d: 这是一个无返回值的函数调用语句\n", lcnt);
    }
    else {
        printf("line %d: 这是一个有返回值的函数调用语句\n", lcnt);
    }
}


void statement()
{
    int i;
    switch (sy) {
    case ifsy:
        ifstatement();
        break;
    case whilesy:
        whilestatement();
        break;
    case switchsy:
        switchstatement();
        break;
    case returnsy:
        returnstatement();
        if (sy == semicolon) {
            insymbol();
        }
        else {
            error(13);
        }
        break;
    case semicolon: insymbol(); printf("line %d: 这是一个空语句\n", lcnt); break;
    case lbrace: insymbol();
        while (statbegsys.count(sy)) {
            statement();//语句处理函数遇到分号读下一个字符
        }
        if (sy == rbrace) {
            insymbol();
        }
        else {
            error(16);
        }
        break;
    case ident:
        i = loc(id);
        if (i != 0) {
            switch (tab[i].obj) {
            case constant: error(17); break;
            case variable:
            case arrays:
                assignment(i);
                if (sy == semicolon) {
                    insymbol();
                }
                else {
                    error(13);
                }
                break;
            case function:
                if (id == "scanf" || id == "printf") {
                    standfunc();
                }
                else {
                    call(i);
                }
                if (sy == semicolon) {
                    insymbol();
                }
                else {
                    error(13);
                }
                break;
            default:
                break;
            }
        }
        else {
            error(18);//未定义的标识符
        }
        break;
    default:
        error(20); //不合法语句
    }
}


void compoundstatement()
{
    string name;
    types typ = ints;

    while (sy == constsy) {
        constdef();
    }
    while (sy == intsy || sy == charsy) {
        typ = (sy == intsy) ? ints : chars;
        insymbol();
        if (sy == ident) {
            name = id;
            insymbol();
            vardef(typ, name);
        }
        else {
            error(6); //缺变量名标识符
        }
    }
    btab[b].vsize = dx;

    while (statbegsys.count(sy)) {
        statement();//语句处理函数遇到分号读下一个字符
    }
    printf("line %d: 这是一个语句列\n", lcnt - 1);
    printf("line %d: 这是一个复合语句\n", lcnt - 1);
}


int paralist()
{
    int paracnt = 0;
    types typ = ints;

    insymbol();

    if (sy != rparent) {
        if (sy == intsy || sy == charsy) {
            typ = (sy == intsy) ? ints : chars;
            insymbol();
            if (sy == ident) {
                tab_enter(id, variable, typ, dx);
                dx += (typ == ints) ? 4 : 1;
                paracnt++;
                insymbol();
            }
            else {
                error(6); //缺少参数名标识符
            }
        }
        else{
            error(9); //缺少参数类型标识符
        }

        while (sy == comma) {
            insymbol();
            if (sy == intsy || sy == charsy) {
                typ = (sy == intsy) ? ints : chars;
                insymbol();
                if (sy == ident) {
                    tab_enter(id, variable, typ, dx);
                    dx += (typ == ints) ? 4 : 1;
                    paracnt++;
                    insymbol();
                }
                else {
                    error(6); //缺少参数名标识符
                }
            }
            else if (sy != rparent) {
                error(9); //缺少参数类型标识符
            }
        }

    }

    if (sy == rparent) {
        insymbol();
    }
    else {
        error(14);
    }
    printf("line %d: 这是一个参数表\n", lcnt - 1);
    return paracnt;
}


bool funcdef(types typ, string name)
{
    int paracnt;
    
    btab_enter();
    tab_enter(name, function, typ, 0);
    tab[t].ref = b;
    prt = t;

    dx = 0;
    paracnt = paralist();
    btab[b].lastpar = t;
    btab[b].psize = dx;
    
    retflag = false;
    if (sy == lbrace) {
        insymbol();
        compoundstatement();
    }
    else {
        error(15);//缺复合语句的左大括号
    }

    if (typ != voids && !retflag) {
        error(39);//缺返回语句
    }

    if (sy == rbrace) {
        insymbol();
    }
    else {
        error(16);//缺复合语句的右大括号
    }

    if (name == "main" && typ == voids && paracnt == 0) {
        printf("line %d: 这是一个主函数的定义\n", lcnt);
        return true;
    }
    else if (typ == voids) {
        printf("line %d: 这是一个无返回值函数的定义\n", lcnt - 1);
    }
    else {
        printf("line %d: 这是一个有返回值函数的定义\n", lcnt - 1);
    }
    return false;
}


void syntax_analysis()
{
    bool funcflag = false;
    bool mainflag = false;
    string name;
    types typ = ints;
    
    btab_enter();
    tab_enter("scanf", function, voids, 1);
    tab_enter("printf", function, voids, 2);

    dx = 0;
    insymbol();
    while (sy == constsy) {
        constdef();
    }
    while ((sy == intsy || sy == charsy || sy == voidsy) && !mainflag) {
        if (sy == intsy || sy == charsy) {
            typ = (sy == intsy) ? ints : chars;
            insymbol();
            if (sy == ident) {
                name = id;
                insymbol();
                if (sy == lparent) {
                    if (!funcflag) {
                        btab[0].vsize = dx;
                    }
                    funcflag = true;
                    funcdef(typ, name);
                }
                else if (!funcflag) {
                    vardef(typ, name);
                }
                else {
                    error(37); //程序结构错误
                }
            }
            else {
                error(6);
            }
        }
        else { //sy == voidsy
            insymbol();
            if (sy == ident) {
                name = id;
                insymbol();
                if (sy == lparent) {
                    if (!funcflag) {
                        btab[0].vsize = dx;
                    }
                    funcflag = true;
                    mainflag = funcdef(voids, name);
                }
                else {
                    error(37); //程序结构错误
                }
            }
            else {
                error(6);
            }
        }
    }

    if (!mainflag) {
        error(38);
    }
    if (sy != eofsy) {
        error(37);
    }
}
