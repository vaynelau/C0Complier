#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <set>
#include "lexical_analysis.h"
#include "syntax_analysis.h"
#include "error.h"
#include "table.h"
#include "midcode.h"

using namespace std;

int dx, prt;
//int prmx;
bool retflag;
int labelx;
int conindexflag;
int arrindex;
int tmpindex;
int n_localvar;

void constdef(bool isglobal)
{
    int sign = 1;

    symbol s[] = { comma, semicolon };
    set<symbol> sepsys(s, s + sizeof(s) / sizeof(s[0]));
    set<symbol> sys(procbegsys.begin(), procbegsys.end());
    if (!isglobal) {
        sys.insert(statbegsys.begin(), statbegsys.end());
        sys.insert(rbrace);
        sys.erase(ident);
        //sys.erase(semicolon);
    }

    insymbol();
    if (sy == intsy) {
        do {
            insymbol();
            if (sy == ident) {
                tab_enter(id, constant, ints, 0);
                insymbol();
                if (sy == becomes) {
                    insymbol();
                    if (sy == plus_sy || sy == minus_sy) {
                        sign = (sy == minus_sy) ? -1 : 1;
                        insymbol();
                    }
                    if (sy == intcon) {
                        tab[t].adr = sign * inum;
                        midcode_enter(_condef, ints, t, sign * inum);
                        insymbol();
                        if (!sepsys.count(sy) && !sys.count(sy)) {
                            error(49);
                            skip2(sepsys, sys);
                        }
                    }
                    else {
                        error(8);
                        skip2(sepsys, sys);
                    }
                }
                else {
                    error(7);
                    skip2(sepsys, sys);
                }
            }
            else {
                error(6);
                skip2(sepsys, sys);
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
                    if (sy == charcon) {
                        tab[t].adr = inum;
                        midcode_enter(_condef, chars, t, inum);
                        insymbol();
                        if (!sepsys.count(sy) && !sys.count(sy)) {
                            error(49);
                            skip2(sepsys, sys);
                        }
                    }
                    else {
                        error(8);
                        skip2(sepsys, sys);
                    }
                }
                else {
                    error(7);
                    skip2(sepsys, sys);
                }
            }
            else {
                error(6);
                skip2(sepsys, sys);
            }
        } while (sy == comma);

    }
    else {
        error(9);
        sepsys.erase(comma);
        skip2(sepsys, sys);
    }

    if (sy == semicolon) {
        insymbol();
    }
    else {
        error(13);
    }
}


void vardef(datatyp typ, string name, bool isglobal)
{

    symbol s[] = { comma, semicolon };
    set<symbol> sepsys(s, s + sizeof(s) / sizeof(s[0]));
    set<symbol> sys(procbegsys.begin(), procbegsys.end());
    if (!isglobal) {
        sys.insert(statbegsys.begin(), statbegsys.end());
        sys.insert(rbrace);
        sys.erase(ident);
        //sys.erase(semicolon);
    }

    if (!name.empty() && sy == lbracket) {
        insymbol();
        if (sy == intcon) {
            if (inum > 0) {
                tab_enter(name, arrays, typ, dx);
                tab[t].arrcnt = inum;
                midcode_enter(_arrdef, typ, t, inum);
                //dx += (dtyp == ints) ? (inum * 4) : inum;
                dx += inum * 4;
            }
            else {
                error(10);//数组元素个数不大于0
            }
            insymbol();
            if (sy == rbracket) {
                insymbol();
                if (!sepsys.count(sy) && !sys.count(sy)) {
                    error(49);
                    skip2(sepsys, sys);
                }
            }
            else {
                error(12);//缺少右中括号
                skip2(sepsys, sys);
            }
        }
        else {
            error(8);//没有指定数组元素
            skip2(sepsys, sys);
        }

    }
    else if (!name.empty()) {
        tab_enter(name, variable, typ, dx);
        tab[t].pos = n_localvar;
        midcode_enter(_vardef, typ, t, -1);
        //dx += (dtyp == ints) ? 4 : 1;
        dx += 4;
        n_localvar += 1;
        if (!sepsys.count(sy) && !sys.count(sy)) {
            error(49);
            skip2(sepsys, sys);
        }
    }
    else {
        error(6); //变量无标识符
        skip2(sepsys, sys);
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
                        tab[t].arrcnt = inum;
                        midcode_enter(_arrdef, typ, t, inum);
                        //dx += (dtyp == ints) ? (inum * 4) : inum;
                        dx += inum * 4;
                    }
                    else {
                        error(10);//数组元素个数不大于0
                    }
                    insymbol();
                    if (sy == rbracket) {
                        insymbol();
                        if (!sepsys.count(sy) && !sys.count(sy)) {
                            error(49);
                            skip2(sepsys, sys);
                        }
                    }
                    else {
                        error(12);//缺少右中括号
                        skip2(sepsys, sys);
                    }
                }
                else {
                    error(11);//没有指定数组元素个数
                    skip2(sepsys, sys);
                }

            }
            else {
                tab_enter(name, variable, typ, dx);
                tab[t].pos = n_localvar;
                midcode_enter(_vardef, typ, t, -1);
                //dx += (dtyp == ints) ? 4 : 1;
                dx += 4;
                n_localvar += 1;
                if (!sepsys.count(sy) && !sys.count(sy)) {
                    error(49);
                    skip2(sepsys, sys);
                }
            }
        }
        else {
            error(6); //变量无标识符
            skip2(sepsys, sys);
        }
    }

    if (sy == semicolon) {
        insymbol();
    }
    else {
        error(13);
    }
}


int factor(datatyp *pdtyp, operandtyp *potyp)
{
    datatyp dtyp = ints, dtyp2;
    operandtyp otyp = _reterr, otyp2;

    int sign, ret = -1, ret2;
    int i;

    switch (sy) {
    case charcon:
        dtyp = chars;
        otyp = _charcon;
        ret = inum;
        insymbol();
        break;
    case plus_sy:
    case minus_sy:
        sign = (sy == plus_sy) ? 1 : -1;
        insymbol();
        if (sy == intcon) {
            dtyp = ints;
            otyp = _intcon;
            ret = inum * sign;
            insymbol();
        }
        else {
            error(8);
            skip(statskipsys);
            *potyp = _reterr;
            return -1;
        }
        break;
    case intcon:
        dtyp = ints;
        otyp = _intcon;
        ret = inum;
        insymbol();
        break;
    case lparent:
        insymbol();
        ret = expression(&dtyp, &otyp);
        dtyp = ints;
        if (otyp == _reterr) {
            *potyp = _reterr;
            return -1;
        }
        if (sy == rparent) {
            insymbol();
        }
        else {
            error(14);
            skip(statskipsys);
            *potyp = _reterr;
            return -1;
        }
        break;
    case ident:
        i = loc(id);
        if (i != 0) {
            switch (tab[i].obj) {
            case constant:
                dtyp = tab[i].typ;
                otyp = (dtyp == ints) ? _intcon : _charcon;
                ret = tab[i].adr; //常量替换
                insymbol();
                break;
            case variable:
                dtyp = tab[i].typ;
                if (tab[i].lev == 0) {
                    midcode_enter2(_assign, ++tmpindex, i, -1, _tmpvar, _globalvar, -1);
                    otyp = _tmpvar;
                    ret = tmpindex;
                }
                else {
                    otyp = _localvar;
                    ret = i;
                }
                insymbol();
                break;
            case arrays:
                insymbol();
                if (sy == lbracket) {
                    insymbol();
                    conindexflag = 0;
                    ret2 = expression(&dtyp2, &otyp2);
                    if (otyp2 == _reterr) {
                        *potyp = _reterr;
                        return -1;
                    }
                    if (dtyp2 != ints) {
                        error(31);
                    }
                    else if (otyp2 == _intcon && !(ret2 >= 0 && ret2 < tab[i].arrcnt)) {
                        error(41);  //数组下标越界
                    }
                    midcode_enter2(_arrload, ++tmpindex, i, ret2, _tmpvar, _arrvar, otyp2);
                    dtyp = tab[i].typ;
                    otyp = _tmpvar;
                    ret = tmpindex;
                    if (sy == rbracket) {
                        insymbol();
                    }
                    else {
                        error(12);
                        skip(statskipsys);
                        *potyp = _reterr;
                        return -1;
                    }
                }
                else {
                    error(22);
                    skip(statskipsys);
                    *potyp = _reterr;
                    return -1;
                }
                break;
            case function:
                if (tab[i].typ == voids) {
                    error(23);
                    dtyp = ints;
                }
                ret = funccall(i);
                if (ret == -1) {
                    *potyp = _reterr;
                    return -1;
                }
                midcode_enter2(_assign, ++tmpindex, ret, -1, _tmpvar, _retval, -1);
                dtyp = tab[i].typ;
                otyp = _tmpvar;
                ret = tmpindex;
                break;
            default:
                break;
            }
        }
        else {
            error(18);//未定义的标识符
            skip(statskipsys);
            *potyp = _reterr;
            return -1;
        }
        break;
    default:
        error(24);
        skip(statskipsys);
        *potyp = _reterr;
        return -1;
        break;
    }

    *pdtyp = dtyp;
    *potyp = otyp;
    return ret;
}


int term(datatyp *pdtyp, operandtyp *potyp)
{
    datatyp dtyp, dtyp2;
    operandtyp otyp, otyp2;
    int ret, ret2;
    optyp op;

    ret = factor(&dtyp, &otyp);
    if (otyp == _reterr) {
        *potyp = _reterr;
        return -1;
    }

    while (sy == times_sy || sy == idiv_sy) {
        dtyp = ints;
        op = (sy == times_sy) ? _times : _idiv;
        insymbol();
        ret2 = factor(&dtyp2, &otyp2);
        if (otyp2 == _reterr) {
            *potyp = _reterr;
            return -1;
        }
        if ((otyp == _intcon || otyp == _charcon) && (otyp2 == _intcon || otyp2 == _charcon)) { //常数合并
            ret = (op == _times) ? (ret * ret2) : (ret / ret2);
            otyp = _intcon;
        }
        else {
            midcode_enter2(op, ++tmpindex, ret, ret2, _tmpvar, otyp, otyp2);
            otyp = _tmpvar;
            ret = tmpindex;
        }
    }
    *pdtyp = dtyp;
    *potyp = otyp;
    return ret;
}


int expression(datatyp *ptyp, operandtyp *potyp)
{
    int sign = 1;
    datatyp dtyp, dtyp1, dtyp2;
    operandtyp otyp, otyp2;
    int ret, ret2;
    optyp op;

    dtyp = chars;
    if (sy == plus_sy || sy == minus_sy) {
        sign = (sy == plus_sy) ? 1 : -1;
        insymbol();
        dtyp = ints;
    }

    ret = term(&dtyp1, &otyp);
    if (otyp == _reterr) {
        *potyp = _reterr;
        return -1;
    }
    dtyp = (dtyp1 == ints) ? ints : dtyp;

    if (sign == -1) {
        if (otyp == _intcon || otyp == _charcon) {
            ret = -ret;
            otyp = _intcon;
        }
        else {
            midcode_enter2(_neg, ++tmpindex, ret, -1, _tmpvar, otyp, -1);
            otyp = _tmpvar;
            ret = tmpindex;
        }
    }

    while (sy == plus_sy || sy == minus_sy) {
        dtyp = ints;
        op = (sy == plus_sy) ? _plus : _minus;
        insymbol();
        ret2 = term(&dtyp2, &otyp2);
        if (otyp2 == _reterr) {
            *potyp = _reterr;
            return -1;
        }
        if ((otyp == _intcon || otyp == _charcon) && (otyp2 == _intcon || otyp2 == _charcon)) { //常数合并
            ret = (op == _plus) ? (ret + ret2) : (ret - ret2);
            otyp = _intcon;
        }
        else {
            midcode_enter2(op, ++tmpindex, ret, ret2, _tmpvar, otyp, otyp2);
            otyp = _tmpvar;
            ret = tmpindex;
        }
    }
    *ptyp = dtyp;
    *potyp = otyp;
    return ret;
}


void assignment(int i)
{
    datatyp dtyp1, dtyp2;
    operandtyp otyp1 = _reterr, otyp2, otyp;
    int ret1 = 0, ret2;

    insymbol();
    if (tab[i].obj == arrays) {
        if (sy == lbracket) {
            insymbol();
            ret1 = expression(&dtyp1, &otyp1);
            if (otyp1 == _reterr) {
                return;
            }
            if (dtyp1 != ints) {
                error(31);  //数组下标只能为整型
            }
            if (otyp1 == _condef && !(ret1 >= 0 && ret1 < tab[i].arrcnt)) {
                error(41);  //数组下标越界
            }
            if (sy == rbracket) {
                insymbol();
                if (sy == becomes) {
                    insymbol();
                    ret2 = expression(&dtyp2, &otyp2);
                    if (otyp2 == _reterr) {
                        return;
                    }
                    if (dtyp2 != tab[i].typ) {
                        error(32); //赋值符号左右类型不一致
                    }
                    midcode_enter2(_arrassign, i, ret1, ret2, _arrvar, otyp1, otyp2);
                }
                else {
                    error(7);
                    skip(statskipsys);
                }
            }
            else {
                error(12);
                skip(statskipsys);
            }
        }
        else {
            error(22);
            skip(statskipsys);
        }
    }
    else {
        if (sy == becomes) {
            insymbol();
            ret2 = expression(&dtyp2, &otyp2);
            if (otyp2 == _reterr) {
                return;
            }
            if (dtyp2 != tab[i].typ) {
                error(32); //赋值符号左右类型不一致
            }
            otyp = (tab[i].lev == 0) ? _globalvar : _localvar;
            midcode_enter2(_assign, i, ret2, -1, otyp, otyp2, -1);
        }
        else {
            error(7);
            skip(statskipsys);
        }
    }
}


void returnstatement()
{
    datatyp typ;
    operandtyp otyp;
    int ret;

    retflag = true;
    insymbol();
    if (sy == lparent) {
        insymbol();
        ret = expression(&typ, &otyp);
        if (otyp == _reterr) {
            return;
        }
        if (typ != tab[prt].typ) {
            error(25); //返回值类型不一致
        }
        midcode_enter2(_ret, -1, ret, -1, -1, otyp, -1);
        if (sy == rparent) {
            insymbol();
        }
        else {
            error(14);
            skip(statskipsys);
        }
    }
    else {
        midcode_enter(_ret, -1, -1, -1);
    }
}


void ifstatement()
{
    int ret1, ret2;
    datatyp dtyp1, dtyp2;
    operandtyp otyp1, otyp2;
    optyp op;

    int prlabelx = ++labelx;

    insymbol();
    if (sy == lparent) {
        insymbol();
        ret1 = expression(&dtyp1, &otyp1);
        if (otyp1 == _reterr) {
            goto stat;
        }
        if (relationops.count(sy)) {
            if (dtyp1 != ints) {
                error(46); //关系运算符左侧表达式类型不为整型
            }
            op = (optyp)(sy - eql + _eql);
            insymbol();
            ret2 = expression(&dtyp2, &otyp2);
            if (otyp2 == _reterr) {
                goto stat;
            }
            if (dtyp2 != ints) {
                error(47); //关系运算符右侧表达式类型不为整型
            }
            if (otyp1 == _intcon && otyp2 == _intcon) {
                switch (op) {
                case _eql:ret1 = (ret1 == ret2); break;
                case _neq:ret1 = (ret1 != ret2); break;
                case _gtr:ret1 = (ret1 > ret2); break;
                case _geq:ret1 = (ret1 >= ret2); break;
                case _lss:ret1 = (ret1 < ret2); break;
                case _leq:ret1 = (ret1 <= ret2); break;
                default:
                    break;
                }
            }
            else {
                midcode_enter2(op, ++tmpindex, ret1, ret2, _tmpvar, otyp1, otyp2);
                otyp1 = _tmpvar;
                ret1 = tmpindex;
            }

        }
        else {
            if (dtyp1 != ints) {
                error(28); //条件表达式的类型不为整型
            }
        }

        if (sy == rparent) {
            insymbol();
        }
        else {
            error(14);
            skip(statskipsys);
        }
        midcode_enter2(_bz, prlabelx, ret1, -1, -1, otyp1, -1);
    }
    else {
        error(26);
        skip(statskipsys);
    }

stat:
    statement();
    midcode_enter(_label, prlabelx, -1, -1);
}


void whilestatement()
{
    int ret1, ret2;
    datatyp dtyp1 = ints, dtyp2 = ints;
    operandtyp otyp1, otyp2;
    int prlabelx1 = ++labelx;
    int prlabelx2 = ++labelx;
    optyp op;

    midcode_enter(_label, prlabelx1, -1, -1);

    insymbol();
    if (sy == lparent) {
        insymbol();
        ret1 = expression(&dtyp1, &otyp1);
        if (otyp1 == _reterr) {
            goto stat2;
        }
        if (relationops.count(sy)) {
            if (dtyp1 != ints) {
                error(46); //关系运算符左侧表达式类型不为整型
            }
            op = (optyp)(sy - eql + _eql);
            insymbol();
            ret2 = expression(&dtyp2, &otyp2);
            if (otyp2 == _reterr) {
                goto stat2;
            }
            if (dtyp2 != ints) {
                error(47); //关系运算符右侧表达式类型不为整型
            }
            if (otyp1 == _intcon && otyp2 == _intcon) {
                switch (op) {
                case _eql:ret1 = (ret1 == ret2); break;
                case _neq:ret1 = (ret1 != ret2); break;
                case _gtr:ret1 = (ret1 > ret2); break;
                case _geq:ret1 = (ret1 >= ret2); break;
                case _lss:ret1 = (ret1 < ret2); break;
                case _leq:ret1 = (ret1 <= ret2); break;
                default:
                    break;
                }
            }
            else {
                midcode_enter2(op, ++tmpindex, ret1, ret2, _tmpvar, otyp1, otyp2);
                otyp1 = _tmpvar;
                ret1 = tmpindex;
            }
        }
        else {
            if (dtyp1 != ints) {
                error(28); //条件表达式的类型不为整型
            }
        }
        if (sy == rparent) {
            insymbol();
        }
        else {
            error(14);
            skip(statskipsys);
        }
        midcode_enter2(_bz, prlabelx2, ret1, -1, -1, otyp1, -1);
    }
    else {
        error(26);
        skip(statskipsys);
    }

stat2:
    statement();
    midcode_enter(_goto, prlabelx1, -1, -1);
    midcode_enter(_label, prlabelx2, -1, -1);
}

set<int> casecon;

void onecase(datatyp dtyp, int ret, int otyp, int labelxend)
{
    int sign;
    int labelx1 = ++labelx;
    int ret2;
    operandtyp otyp2;

    insymbol();
    if (sy == charcon) {
        ret2 = inum;
        otyp2 = _charcon;
        if (dtyp != chars) {
            error(29);
        }
        else if (casecon.count(inum)) {
            error(40);
        }
        else {
            casecon.insert(inum);
        }
        insymbol();
        if (sy == colon) {
            insymbol();
        }
        else {
            error(21);
            skip(statskipsys);
        }
        if (otyp == _charcon) {
            ret = (ret == ret2);
            otyp = _intcon;
        }
        else {
            midcode_enter2(_eql, ++tmpindex, ret, ret2, _tmpvar, otyp, otyp2);
            ret = tmpindex;
            otyp = _tmpvar;
        }
        midcode_enter2(_bz, labelx1, ret, -1, -1, otyp, -1);
    }
    else {
        sign = 1;
        if (sy == plus_sy || sy == minus_sy) {
            sign = (sy == plus_sy) ? 1 : -1;
            insymbol();
        }
        if (sy == intcon) {
            ret2 = sign * inum;
            otyp2 = _intcon;
            if (dtyp != ints) {
                error(29);
            }
            else if (casecon.count(sign * inum)) {
                error(40);
            }
            else {
                casecon.insert(sign * inum);
            }
            insymbol();
            if (sy == colon) {
                insymbol();
            }
            else {
                error(21);
                skip(statskipsys);
            }
            if (otyp == _intcon) {
                ret = (ret == ret2);
                otyp = _intcon;
            }
            else {
                midcode_enter2(_eql, ++tmpindex, ret, ret2, _tmpvar, otyp, otyp2);
                ret = tmpindex;
                otyp = _tmpvar;
            }
            midcode_enter2(_bz, labelx1, ret, -1, -1, otyp, -1);
        }
        else {
            error(8);
            skip(statskipsys);
        }
    }

    statement();
    midcode_enter(_goto, labelxend, -1, -1);
    midcode_enter(_label, labelx1, -1, -1);
}


void switchstatement()
{
    datatyp dtyp = ints;
    operandtyp otyp = _intcon;
    int ret = 0;
    int labelxend = ++labelx;

    insymbol();
    if (sy == lparent) {
        insymbol();
        ret = expression(&dtyp, &otyp);
        if (otyp != _reterr) {
            if (sy == rparent) {
                insymbol();
            }
            else {
                error(14);
                skip(statskipsys);
            }
        }
    }
    else {
        error(26);
        skip(statskipsys);
    }

    if (sy == lbrace) {
        insymbol();
        casecon.clear();
        if (sy == casesy) {
            do {
                onecase(dtyp, ret, otyp, labelxend);
            } while (sy == casesy);

            if (sy == defaultsy) {
                insymbol();
                if (sy == colon) {
                    insymbol();
                }
                else {
                    error(21);
                    skip(statskipsys);
                }
                statement();
            }
        }
        else {
            error(30);// 无case语句
            skip(statskipsys);
        }

        if (sy == rbrace) {
            insymbol();
        }
        else {
            error(16);
            skip(statskipsys);
        }
    }
    else {
        error(15);
        skip(statskipsys);
    }

    midcode_enter(_label, labelxend, -1, -1);
}


void stdfunccall()
{
    int ret;
    datatyp dtyp = ints;
    operandtyp otyp;
    symbol s[] = { comma, rparent };
    set<symbol> sys(s, s + sizeof(s) / sizeof(s[0]));

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
                    otyp = (tab[i].lev == 0) ? _globalvar : _localvar;
                    midcode_enter2(_push_scanf, i, -1, -1, otyp, -1, -1);
                    insymbol();
                    if (!sys.count(sy) && !statskipsys.count(sy)) {
                        error(49);
                        skip2(sys, statskipsys);
                    }
                }
                else {
                    error(6);
                    skip2(sys, statskipsys);
                }
            } while (sy == comma);

            if (sy == rparent) {
                insymbol();
            }
            else {
                error(14);
            }
            midcode_enter(_std_call, loc("scanf"), -1, -1);
        }
        else {
            error(26);
            skip(statskipsys);
        }

    }
    else if (id == "printf") {
        insymbol();
        if (sy == lparent) {
            insymbol();
            if (sy == stringcon) {
                ret = inum;
                int str_addr = inum;
                dtyp = strs;
                otyp = _strcon;
                midcode_enter2(_push, PRINTF, ret, dtyp, -1, otyp, -1);
                insymbol();
                if (sy == comma) {
                    insymbol();
                    ret = expression(&dtyp, &otyp);
                    if (otyp == _reterr) {
                        return;
                    }
                    midcode_enter2(_push, PRINTF, ret, dtyp, -1, otyp, -1);
                    midcode_enter(_std_call, loc("printf"), 2, str_addr);
                }
                else {
                    midcode_enter(_std_call, loc("printf"), 1, str_addr);
                }
                if (sy == rparent) {
                    insymbol();
                }
                else {
                    error(14);
                    skip(statskipsys);
                }
            }
            else {
                ret = expression(&dtyp, &otyp);
                if (otyp == _reterr) {
                    return;
                }
                midcode_enter2(_push, PRINTF, ret, dtyp, -1, otyp, -1);
                if (sy == rparent) {
                    insymbol();
                }
                else {
                    error(14);
                    skip(statskipsys);
                }
                midcode_enter(_std_call, loc("printf"), 1, -1);
            }
        }
        else {
            error(26);
            skip(statskipsys);
        }
    }
}


int funccall(int i)
{
    int lastp = btab[tab[i].ref].lastpar;
    int curpos = i;
    int ret;
    datatyp dtyp;
    operandtyp otyp;
    symbol s[] = { comma, rparent };
    set<symbol> sys(s, s + sizeof(s) / sizeof(s[0]));

    insymbol();
    if (sy == lparent) {
        insymbol();
        if (sy != rparent) {
            curpos++;
            ret = expression(&dtyp, &otyp);
            if (otyp == _reterr) {
                return -1;
            }
            if (curpos > lastp) {
                error(34); //实参过多
            }
            else {
                if (dtyp != tab[curpos].typ) {
                    error(35); //实参与形参类型不一致
                }
                midcode_enter2(_push, -1, ret, -1, -1, otyp, -1);
            }
            if (!sys.count(sy) && !statskipsys.count(sy)) {
                error(49);
                skip2(sys, statskipsys);
            }

            while (sy == comma) {
                insymbol();
                curpos++;
                ret = expression(&dtyp, &otyp);
                if (otyp == _reterr) {
                    return -1;
                }
                if (curpos > lastp) {
                    error(34); //实参过多
                }
                else {
                    if (dtyp != tab[curpos].typ) {
                        error(35); //实参与形参类型不一致
                    }
                    midcode_enter2(_push, -1, ret, -1, -1, otyp, -1);
                }
                if (!sys.count(sy) && !statskipsys.count(sy)) {
                    error(49);
                    skip2(sys, statskipsys);
                }
            }
        }

        if (curpos < lastp) {
            error(36); //实参过少
        }

        if (sy == rparent) {
            insymbol();
        }
        else {
            error(14);
            skip(statskipsys);
            return -1;
        }
        midcode_enter(_call, i, -1, -1);
    }
    else {
        error(26);
        skip(statskipsys);
        return -1;
    }

    return RET;
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
            skip(statskipsys);
        }
        break;
    case semicolon:
        insymbol();
        break;
    case lbrace:
        insymbol();
        while (statbegsys.count(sy)) {
            statement();//语句处理函数遇到分号读下一个字符
        }
        if (sy == rbrace) {
            insymbol();
        }
        else {
            error(16);
            skip(statskipsys);
        }
        break;
    case ident:
        i = loc(id);
        if (i != 0) {
            switch (tab[i].obj) {
            case constant:
                error(17);
                skip(statskipsys);
                break;
            case variable:
            case arrays:
                assignment(i);
                if (sy == semicolon) {
                    insymbol();
                }
                else {
                    error(13);
                    skip(statskipsys);
                }
                break;
            case function:
                if (id == "scanf" || id == "printf") {
                    stdfunccall();
                }
                else {
                    funccall(i);
                }
                if (sy == semicolon) {
                    insymbol();
                }
                else {
                    error(13);
                    skip(statskipsys);
                }
                break;
            default:
                break;
            }
        }
        else {
            error(18);//未定义的标识符
            skip(statskipsys);
        }
        break;
    default:
        error(20); //不合法语句
        skip(statskipsys);
    }
}


void compoundstatement()
{
    string name;
    datatyp typ = ints;
    bool varflag = false;

    while (sy == constsy || sy == intsy || sy == charsy) {
        if (sy == constsy) {
            if (varflag) {
                error(43);  //常量定义出现的位置不当
            }
            constdef(false);
        }
        else if (sy == intsy || sy == charsy) {
            if (!varflag) {
                varflag = true;
            }
            typ = (sy == intsy) ? ints : chars;
            insymbol();
            if (sy == ident) {
                name = id;
                insymbol();
                vardef(typ, name, false);
            }
            else {
                name = "";
                vardef(typ, name, false);
            }
        }
    }
    btab[b].vsize = dx;
    while (statbegsys.count(sy)) {
        statement();//语句处理函数遇到分号读下一个字符
    }
}


int paralist()
{
    int paracnt = 0;
    datatyp typ;
    symbol s[] = { comma, rparent, lbrace };
    set<symbol> sys(s, s + sizeof(s) / sizeof(s[0]));
    bool first = true;

    do {
        insymbol();
        if (first) {
            first = false;
            if (sy == rparent) {
                insymbol(); //参数表为空
                return 0;
            }
        }
        if (sy == intsy || sy == charsy) {
            typ = (sy == intsy) ? ints : chars;
            insymbol();
            if (sy == ident) {
                tab_enter(id, variable, typ, dx);
                tab[t].pos = n_localvar;
                midcode_enter(_paradef, typ, t, -1);
                dx += 4;
                n_localvar += 1;
                paracnt++;
                insymbol();
                if (!procbegsys.count(sy) && !sys.count(sy)) {
                    error(49);
                    skip2(sys, procbegsys); // , )  {
                }
            }
            else {
                error(6); //缺少参数名标识符
                skip2(sys, procbegsys); // , )  {
            }
        }
        else {
            error(9); //缺少参数类型标识符
            skip2(sys, procbegsys); // , )  { 
        }
    } while (sy == comma);

    if (sy == rparent) {
        insymbol();
    }
    else {
        error(14);
    }
    return paracnt;
}


bool funcdef(datatyp typ, string name)
{
    int n_paras;

    n_localvar = 0;

    tmpindex = -1;
    btab_enter();
    tab_enter(name, function, typ, 0);
    midcode_enter(_funcdef, typ, t, -1);
    //prmx = mx;
    tab[t].ref = b;
    prt = t;

    dx = 0;
    n_paras = paralist();
    btab[b].lastpar = t;
    btab[b].psize = dx;
    //midcode[prmx].v3 = n_paras; //无用
    //n_localvar = n_paras;

    if (name == "main" && (typ != voids || n_paras != 0)) {
        error(48);  //main函数定义不规范
    }

    retflag = false;
    if (sy == lbrace) {
        insymbol();
        compoundstatement();
        if (typ != voids && !retflag) {
            error(39);//函数缺少返回语句
        }
        if (sy == rbrace) {
            insymbol();
        }
        else {
            error(16);//缺复合语句的右大括号
            skip(procbegsys);
        }
    }
    else {
        error(15);//缺复合语句的左大括号
        skip(procbegsys);
    }
    midcode_enter(_ret, -1, -1, -1);
    btab[b].n_tmpvar = tmpindex + 1;
    btab[b].n_localvar = n_localvar;

    if (name == "main") {
        return true;
    }
    return false;
}


void syntax_analysis()
{
    bool varflag = false, funcflag = false, mainflag = false;
    string name;
    datatyp typ = ints;
    labelx = -1;
    n_localvar = 0;
    btab_enter();
    tab_enter("scanf", function, voids, 1);
    tab_enter("printf", function, voids, 2);

    dx = 0;
    insymbol();
    while (sy != eofsy) {
        if (sy == constsy) {
            if (varflag || funcflag) {
                error(43);  //常量定义出现的位置不当
            }
            constdef(true);
        }
        else if (sy == intsy || sy == charsy) {
            typ = (sy == intsy) ? ints : chars;
            insymbol();
            if (sy == ident) {
                name = id;
                insymbol();
                if (sy == lparent) {
                    if (!funcflag) {
                        btab[0].vsize = dx;
                        funcflag = true;
                    }
                    if (mainflag) {
                        error(37); //main函数后含有其他函数定义
                    }
                    mainflag = funcdef(typ, name);
                }
                else {
                    if (funcflag) {
                        error(42); //变量定义出现的位置不当
                    }
                    if (!varflag) {
                        varflag = true;
                    }
                    vardef(typ, name, true);
                }
            }
            else {
                error(6); //缺少标识符
                skip(procbegsys);
            }
        }
        else if (sy == voidsy) {
            insymbol();
            if (sy == ident) {
                name = id;
                insymbol();
                if (sy == lparent) {
                    if (!funcflag) {
                        btab[0].vsize = dx;
                        funcflag = true;
                    }
                    if (mainflag) {
                        error(37); //main函数后含有其他函数定义
                    }
                    mainflag = funcdef(voids, name);
                }
                else {
                    error(44); //void类型函数定义错误
                    skip(procbegsys);
                }
            }
            else {
                error(6); //缺少标识符
                skip(procbegsys);
            }
        }
        else {
            error(45); //无法识别的程序开始符号
            skip(procbegsys);
        }
    }

    if (!mainflag) {
        error(38); //程序缺少main函数
    }
}
