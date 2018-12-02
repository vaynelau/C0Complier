#define _CRT_SECURE_NO_WARNINGS
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

vector<midcodeitem> midcode;
int mx;

symbol s1[] = {
    ifsy, whilesy, lbrace, ident, semicolon, switchsy, returnsy
};
set<symbol> statbegsys(s1, s1 + sizeof(s1) / sizeof(s1[0]));

symbol s2[] = {
    eql, neq, gtr, geq, lss, leq
};
set<symbol> relationop(s2, s2 + sizeof(s2) / sizeof(s2[0]));

void tabs_init()
{
    t = -1;
    b = -1;
    sx = -1;
    mx = -1;

    ++t;
    tabitem item;
    item.name = "";
    item.link = 0;
    item.obj = function;
    item.typ = voids;
    item.adr = 0;
    item.ref = 0;
    item.lev = 0;
    item.arrcnt = 0;
    tab.push_back(item);

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

    sps['+'] = _plus_;
    sps['-'] = _minus_;
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
    int i;

    tab[0].name = name;
    i = btab[b].last;
    while (tab[i].name != name) {
        i = tab[i].link;
    }
    if (i != 0) {
        return i;
    }
    i = btab[0].last;
    while (tab[i].name != name) {
        i = tab[i].link;
    }
    return i;
}


void tab_enter(string name, objtyp obj, types typ, int adr)
{
    int i, prb;

    prb = (obj == function) ? 0 : b;
    tab[0].name = name;
    i = btab[prb].last;
    while (tab[i].name != name) {
        i = tab[i].link;
    }

    if (i == 0) {
        ++t;
        tabitem item;
        item.name = name;
        item.obj = obj;
        item.typ = typ;
        item.adr = adr;
        item.ref = 0;
        item.arrcnt = 0;
        item.lev = prb;
        item.link = btab[prb].last;
        btab[prb].last = t;
        tab.push_back(item);
    }
    else {
        error(19); //标识符重定义错误
    }
}

void btab_enter()
{
    ++b;
    btabitem item;
    item.last = 0;
    item.lastpar = 0;
    item.psize = 0;
    item.vsize = 0;
    btab.push_back(item);
}



void midcode_enter(optyp op, int v1, int v2, int v3)
{
    ++mx;
    midcodeitem item;
    item.op = op;
    item.v1 = v1;
    item.v2 = v2;
    item.v3 = v3;
    midcode.push_back(item);
}

void print_midcode()
{
    for (int i = 0; i <= mx; i++) {
        switch (midcode[i].op) {
        case _const:
            if (midcode[i].v1 == ints) {
                printf("const int %s = %d\n", tab[midcode[i].v2].name.c_str(), midcode[i].v3);
            }
            else {
                printf("const char %s = '%c'\n", tab[midcode[i].v2].name.c_str(), midcode[i].v3);
            }
            break;
        case _var:
            if (midcode[i].v1 == ints) {
                printf("var int %s\n", tab[midcode[i].v2].name.c_str());
            }
            else {
                printf("var char %s\n", tab[midcode[i].v2].name.c_str());
            }
            break;

        case _array:
            if (midcode[i].v1 == ints) {
                printf("array int %s %d\n", tab[midcode[i].v2].name.c_str(), midcode[i].v3);
            }
            else {
                printf("array char %s %d\n", tab[midcode[i].v2].name.c_str(), midcode[i].v3);
            }
            break;
        case _func:
            if (midcode[i].v1 == ints) {
                printf("int %s()\n", tab[midcode[i].v2].name.c_str());
            }
            else if (midcode[i].v1 == chars) {
                printf("char %s()\n", tab[midcode[i].v2].name.c_str());
            }
            else {
                printf("void %s()\n", tab[midcode[i].v2].name.c_str());
            }
            break;
        case _para:
            if (midcode[i].v1 == ints) {
                printf("para int %s\n", tab[midcode[i].v2].name.c_str());
            }
            else {
                printf("para char %s\n", tab[midcode[i].v2].name.c_str());
            }
            break;
        case _push:
            printf("push $t%d\n", midcode[i].v1);
            break;
        case _call:
            printf("call %s\n", tab[midcode[i].v1].name.c_str());
            break;
        case _ret:
            if (midcode[i].v1 != -1) {
                printf("ret $t%d\n", midcode[i].v1);
            }
            else {
                printf("ret\n");
            }
            break;
        case _assign:
            if (midcode[i].v2 == RET) {
                printf("$t%d = RET\n", midcode[i].v1);
            }
            else {
                printf("%s = $t%d\n", tab[midcode[i].v1].name.c_str(), midcode[i].v2);
            }
            break;
        case _arrassign:
            printf("%s[$t%d] = $t%d\n", tab[midcode[i].v1].name.c_str(), midcode[i].v2, midcode[i].v3);
            break;
        case _conload:
            if (midcode[i].v2 == ints) {
                printf("$t%d = %d\n", midcode[i].v1, midcode[i].v3);
            }
            else if (midcode[i].v2 == chars) {
                printf("$t%d = '%c'\n", midcode[i].v1, midcode[i].v3);
            }
            else {
                printf("$t%d = \"%s\"\n", midcode[i].v1, stab[midcode[i].v3].c_str());
            }
            break;
        case _varload:
            printf("$t%d = %s\n", midcode[i].v1, tab[midcode[i].v2].name.c_str());
            break;
        case _arrload:
            printf("$t%d = %s[$t%d]\n", midcode[i].v1, tab[midcode[i].v2].name.c_str(), midcode[i].v3);
            break;
        case _label:
            printf("label_%d:\n", midcode[i].v1);
            break;
        case _goto:
            printf("GOTO label_%d\n", midcode[i].v1);
            break;
        case _bz:
            printf("BZ label_%d\n", midcode[i].v1);
            break;
        case _neg:
            printf("$t%d = -$t%d\n", midcode[i].v1, midcode[i].v2);
            break;
        case _plus:
            printf("$t%d = $t%d + $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _minus:
            printf("$t%d = $t%d - $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _times:
            printf("$t%d = $t%d * $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _idiv:
            printf("$t%d = $t%d / $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _eql:
            printf("$t%d == $t%d\n", midcode[i].v2, midcode[i].v3);
            break;
        case _neq:
            printf("$t%d != $t%d\n", midcode[i].v2, midcode[i].v3);
            break;
        case _gtr:
            printf("$t%d > $t%d\n", midcode[i].v2, midcode[i].v3);
            break;
        case _geq:
            printf("$t%d >= $t%d\n", midcode[i].v2, midcode[i].v3);
            break;
        case _lss:
            printf("$t%d < $t%d\n", midcode[i].v2, midcode[i].v3);
            break;
        case _leq:
            printf("$t%d <= $t%d\n", midcode[i].v2, midcode[i].v3);
            break;

        default:
            printf("mmmmmmmmmp%d\n", midcode[i].op);
            break;
        }
    }
}

FILE *out;

void gen_mips()
{
    int dsp, dfp;
    out = fopen("out.asm", "w");
    fprintf(out, ".data\n");
    fprintf(out, "global_var: .space %d\n", btab[0].vsize);
    for (int i = 0; i <= sx; i++) {
        fprintf(out, "str_%d: .asciiz \"%s\"\n", i, stab[i].c_str());
    }

    fprintf(out, "\n\n.text\n");
    int mainref = tab[loc("main")].ref;
    dsp = btab[mainref].vsize;
    fprintf(out, "addiu $fp, $sp, -4\n");
    fprintf(out, "addiu $sp, $sp, %d\n", -dsp);
    fprintf(out, "jal func_%d\n", mainref);
    fprintf(out, "li $v0, 10\n");
    fprintf(out, "syscall\n");

    for (int i = 0; i <= mx; i++) {
        switch (midcode[i].op) {
        case _const:
        case _var:
        case _array:
            break;
        case _func:
            fprintf(out, "\n\nfunc_%d:\n", tab[midcode[i].v2].ref);
            break;
        case _para:
            break;
        case _push:
            if (midcode[i].v2 == SCANF) {
                if (tab[midcode[i].v1].lev == 0) {
                    if (tab[midcode[i].v1].typ == ints) {
                        fprintf(out, "li $v0, 5\n");
                        fprintf(out, "syscall\n");
                        fprintf(out, "sw $v0, global_var+%d\n", tab[midcode[i].v1].adr);
                    }
                    else {
                        fprintf(out, "li $v0, 12\n");
                        fprintf(out, "syscall\n");
                        fprintf(out, "sb $v0, global_var+%d\n", tab[midcode[i].v1].adr);
                    }
                }
                else {
                    if (tab[midcode[i].v1].typ == ints) {
                        fprintf(out, "li $v0, 5\n");
                        fprintf(out, "syscall\n");
                        fprintf(out, "sw $v0, -%d($fp)\n", tab[midcode[i].v1].adr);
                    }
                    else {
                        fprintf(out, "li $v0, 12\n");
                        fprintf(out, "syscall\n");
                        fprintf(out, "sb $v0, -%d($fp)\n", tab[midcode[i].v1].adr);
                    }
                }
            }
            else if (midcode[i].v2 == PRINTF) {
                fprintf(out, "move $a0, $t%d\n", midcode[i].v1);
                if (midcode[i].v3 == strs) {
                    fprintf(out, "li $v0, 4\n");
                }
                else if (midcode[i].v3 == ints) {
                    fprintf(out, "li $v0, 1\n");
                }
                else {
                    fprintf(out, "li $v0, 11\n");
                }
                fprintf(out, "syscall\n");
            }
            else {
                fprintf(out, "addiu $sp, $sp, -4\n");
                fprintf(out, "sw $t%d, 0($sp)\n", midcode[i].v1);
            }
            break;
        case _call:
            if (tab[midcode[i].v1].name != "scanf" && tab[midcode[i].v1].name != "printf") {
                dsp = btab[tab[midcode[i].v1].ref].vsize - btab[tab[midcode[i].v1].ref].psize + 40 + 8;
                dfp = btab[tab[midcode[i].v1].ref].vsize + 40 + 8 - 4;
                fprintf(out, "addiu $sp, $sp, -%d\n", dsp);
                fprintf(out, "sw $fp, 4($sp)\n");
                fprintf(out, "sw $ra, 0($sp)\n");
                for (int j = 0; j < 10; j++) {
                    fprintf(out, "sw $t%d, %d($sp)\n", j, 8 + 4 * j);
                }
                fprintf(out, "addiu $fp, $sp, %d\n", dfp);
                fprintf(out, "jal func_%d\n", tab[midcode[i].v1].ref);
                fprintf(out, "lw $fp, 4($sp)\n");
                fprintf(out, "lw $ra, 0($sp)\n");
                for (int j = 0; j < 10; j++) {
                    fprintf(out, "lw $t%d, %d($sp)\n", j, 8 + 4 * j);
                }
                dsp = btab[tab[midcode[i].v1].ref].vsize + 40 + 8;
                fprintf(out, "addiu $sp, $sp, %d\n", dsp);
            }
            else if (tab[midcode[i].v1].name == "printf") {
                fprintf(out, "li $a0, %d\n", '\n');
                fprintf(out, "li $v0, 11\n");
                fprintf(out, "syscall\n");
            }
            break;
        case _ret:
            if (midcode[i].v1 != -1) {
                fprintf(out, "move $v0, $t%d\n", midcode[i].v1);
            }
            fprintf(out, "jr $ra\n");
            break;
        case _assign:
            if (midcode[i].v2 == RET) {
                fprintf(out, "move $t%d, $v0\n", midcode[i].v1);
            }
            else {
                if (tab[midcode[i].v1].lev == 0) {
                    if (tab[midcode[i].v1].typ == ints) {
                        fprintf(out, "sw $t%d, global_var+%d\n", midcode[i].v2, tab[midcode[i].v1].adr);
                    }
                    else {
                        fprintf(out, "sb $t%d, global_var+%d\n", midcode[i].v2, tab[midcode[i].v1].adr);
                    }
                }
                else {
                    if (tab[midcode[i].v1].typ == ints) {
                        fprintf(out, "sw $t%d, -%d($fp)\n", midcode[i].v2, tab[midcode[i].v1].adr);
                    }
                    else {
                        fprintf(out, "sb $t%d, -%d($fp)\n", midcode[i].v2, tab[midcode[i].v1].adr);
                    }
                }
            }
            break;
        case _arrassign:
            fprintf(out, "sll $t%d, $t%d, 2\n", midcode[i].v2, midcode[i].v2);
            if (tab[midcode[i].v1].lev == 0) {
                if (tab[midcode[i].v1].typ == ints) {
                    fprintf(out, "sw $t%d, global_var+%d($t%d)\n", midcode[i].v3, tab[midcode[i].v1].adr, midcode[i].v2);
                }
                else {
                    fprintf(out, "sb $t%d, global_var+%d($t%d)\n", midcode[i].v3, tab[midcode[i].v1].adr, midcode[i].v2);
                }
            }
            else {
                fprintf(out, "subu $t%d, $fp, $t%d\n", midcode[i].v2, midcode[i].v2);
                if (tab[midcode[i].v1].typ == ints) {
                    fprintf(out, "sw $t%d, -%d($t%d)\n", midcode[i].v3, tab[midcode[i].v1].adr, midcode[i].v2);
                }
                else {
                    fprintf(out, "sb $t%d, -%d($t%d)\n", midcode[i].v3, tab[midcode[i].v1].adr, midcode[i].v2);
                }
            }
            break;
        case _conload:
            if (midcode[i].v2 == ints || midcode[i].v2 == chars) {
                fprintf(out, "li $t%d, %d\n", midcode[i].v1, midcode[i].v3);
            }
            else {
                fprintf(out, "la $t%d,str_%d\n", midcode[i].v1, midcode[i].v3);
            }
            break;
        case _varload:
            if (tab[midcode[i].v2].obj == constant) {
                fprintf(out, "li $t%d, %d\n", midcode[i].v1, tab[midcode[i].v2].adr);
            }
            else if (tab[midcode[i].v2].lev == 0) {
                if (tab[midcode[i].v2].typ == ints) {
                    fprintf(out, "lw $t%d, global_var+%d\n", midcode[i].v1, tab[midcode[i].v2].adr);
                }
                else {
                    fprintf(out, "lb $t%d, global_var+%d\n", midcode[i].v1, tab[midcode[i].v2].adr);
                }
            }
            else {
                if (tab[midcode[i].v2].typ == ints) {
                    fprintf(out, "lw $t%d, -%d($fp)\n", midcode[i].v1, tab[midcode[i].v2].adr);
                }
                else {
                    fprintf(out, "lb $t%d, -%d($fp)\n", midcode[i].v1, tab[midcode[i].v2].adr);
                }
            }
            break;
        case _arrload:
            fprintf(out, "sll $t%d, $t%d, 2\n", midcode[i].v3, midcode[i].v3);
            if (tab[midcode[i].v2].lev == 0) {
                if (tab[midcode[i].v2].typ == ints) {
                    fprintf(out, "lw $t%d, global_var+%d($t%d)\n", midcode[i].v1, tab[midcode[i].v2].adr, midcode[i].v3);
                }
                else {
                    fprintf(out, "lb $t%d, global_var+%d($t%d)\n", midcode[i].v1, tab[midcode[i].v2].adr, midcode[i].v3);
                }
            }
            else {
                fprintf(out, "subu $t%d, $fp, $t%d\n", midcode[i].v3, midcode[i].v3);
                if (tab[midcode[i].v2].typ == ints) {
                    fprintf(out, "lw $t%d, -%d($t%d)\n", midcode[i].v1, tab[midcode[i].v2].adr, midcode[i].v3);
                }
                else {
                    fprintf(out, "lb $t%d, -%d($t%d)\n", midcode[i].v1, tab[midcode[i].v2].adr, midcode[i].v3);
                }
            }
            break;
        case _label:
            fprintf(out, "label_%d:\n", midcode[i].v1);
            break;
        case _goto:
            fprintf(out, "j label_%d\n", midcode[i].v1);
            break;
        case _bz:
            fprintf(out, "beq $t%d, $0, label_%d\n", midcode[i].v2, midcode[i].v1);
            break;
        case _neg:
            fprintf(out, "neg $t%d, $t%d\n", midcode[i].v1, midcode[i].v2);
            break;
        case _plus:
            fprintf(out, "addu $t%d, $t%d, $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _minus:
            fprintf(out, "subu $t%d, $t%d, $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _times:
            fprintf(out, "mul $t%d, $t%d, $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _idiv:
            fprintf(out, "div $t%d, $t%d, $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _eql:
            fprintf(out, "seq $t%d, $t%d, $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _neq:
            fprintf(out, "sne $t%d, $t%d, $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _gtr:
            fprintf(out, "sgt $t%d, $t%d, $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _geq:
            fprintf(out, "sge $t%d, $t%d, $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _lss:
            fprintf(out, "slt $t%d, $t%d, $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _leq:
            fprintf(out, "sle $t%d, $t%d, $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        default:
            fprintf(out, "unknown midcode op: %d\n", midcode[i].op);
            break;
        }
    }

    fclose(out);
}