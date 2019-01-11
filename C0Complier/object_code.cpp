#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <string>
#include <vector>
#include "table.h"
#include "midcode.h"
#include "object_code.h"

using namespace std;


optyp s5[] = {
    _neg, _plus, _minus, _times, _idiv, //运算四元式
    _eql, _neq, _gtr, _geq, _lss, _leq
};
set<optyp> opset5(s5, s5 + sizeof(s5) / sizeof(s5[0]));


int get_v2(FILE *out, int i, int func_x)
{
    int adr;
    int vsize = btab[func_x].vsize;

    if (midcode[i].t2 == _intcon || midcode[i].t2 == _charcon) {
        fprintf(out, "    li $24, %d\n", midcode[i].v2);
    }
    else if (midcode[i].t2 != -1) {
        if (midcode[i].t2 == _localvar) {
            adr = tab[midcode[i].v2].adr;
        }
        else {
            adr = vsize + midcode[i].v2 * 4;
        }
        fprintf(out, "    lw $24, -%d($fp)\n", adr);
    }
    return 24;
}


int get_v3(FILE *out, int i, int func_x)
{
    int adr;
    int vsize = btab[func_x].vsize;

    if (midcode[i].t3 == _intcon || midcode[i].t3 == _charcon) {
        fprintf(out, "    li $25, %d\n", midcode[i].v3);

    }
    else if (midcode[i].t3 != -1) {
        if (midcode[i].t3 == _localvar) {
            adr = tab[midcode[i].v3].adr;
        }
        else {
            adr = vsize + midcode[i].v3 * 4;
        }
        fprintf(out, "    lw $25, -%d($fp)\n", adr);
    }
    return 25;
}

void gen_mips(FILE *out, int i, int func_x)
{
    int dsp, dfp;
    int v1;
    int v2, v3;
    int vsize = btab[func_x].vsize;
    int adr;
    string str;

    if (opset5.count(midcode[i].op)) {
        str = "ERROR";
        switch (midcode[i].op) {
        case _neg:
            str = "negu";
            break;
        case _plus:
            str = "addu";
            break;
        case _minus:
            str = "subu";
            break;
        case _times:
            str = "mul";
            break;
        case _idiv:
            str = "div";
            break;
        case _eql:
            str = "seq";
            break;
        case _neq:
            str = "sne";
            break;
        case _gtr:
            str = "sgt";
            break;
        case _geq:
            str = "sge";
            break;
        case _lss:
            str = "slt";
            break;
        case _leq:
            str = "sle";
            break;
        default:
            str = "ERROR";
            break;
        }

        v2 = get_v2(out, i, func_x);
        v3 = get_v3(out, i, func_x);

        if (midcode[i].t1 == _globalvar) {
            v1 = 24;
        }
        else {
            if (midcode[i].t1 == _localvar) {
                adr = tab[midcode[i].v1].adr;
            }
            else {
                adr = vsize + midcode[i].v1 * 4;
            }
            v1 = 24;
        }

        if (midcode[i].op == _neg) {
            fprintf(out, "    negu $%d, $%d\n", v1, v2);
        }
        else {
            fprintf(out, "    %s $%d, $%d, $%d\n", str.c_str(), v1, v2, v3);
        }

        if (midcode[i].t1 == _globalvar) {
            fprintf(out, "    sw $24, global_var+%d\n", tab[midcode[i].v1].adr);
        }
        else {
            fprintf(out, "    sw $24, -%d($fp)\n", adr);
        }
        return;
    }



    switch (midcode[i].op) {
    case _condef:
    case _vardef:
    case _arrdef:
    case _funcdef:
    case _paradef:
        break;

    case _push_scanf:
        if (tab[midcode[i].v1].lev == 0) {
            if (tab[midcode[i].v1].typ == ints) {
                fprintf(out, "    li $v0, 5\n");
                fprintf(out, "    syscall\n");
                fprintf(out, "    sw $v0, global_var+%d\n", tab[midcode[i].v1].adr);
            }
            else {
                fprintf(out, "    li $v0, 12\n");
                fprintf(out, "    syscall\n");
                fprintf(out, "    sb $v0, global_var+%d\n", tab[midcode[i].v1].adr);
            }
        }
        else {
            if (tab[midcode[i].v1].typ == ints) {
                fprintf(out, "    li $v0, 5\n");
                fprintf(out, "    syscall\n");
            }
            else {
                fprintf(out, "    li $v0, 12\n");
                fprintf(out, "    syscall\n");
            }
            fprintf(out, "    sw $v0, -%d($fp)\n", tab[midcode[i].v1].adr);
        }
        break;

    case _push:
        if (midcode[i].v1 != PRINTF) {
            v2 = get_v2(out, i, func_x);
            fprintf(out, "    addiu $sp, $sp, -4\n");
            fprintf(out, "    sw $%d, 0($sp)\n", v2);
        }
        break;
    case _std_call:
        if (tab[midcode[i].v1].name == "printf") {
            if (midcode[i].v2 == 1) {
                if (i > 0 && midcode[i - 1].v3 == strs) {
                    fprintf(out, "    la $a0, str_%d\n", midcode[i].v3);
                    fprintf(out, "    li $v0, 4\n");
                    fprintf(out, "    syscall\n");
                }
                else if (i > 0 && midcode[i - 1].v3 == ints) {
                    v2 = get_v2(out, i - 1, func_x);
                    fprintf(out, "    move $a0, $%d\n", v2);
                    fprintf(out, "    li $v0, 1\n");
                    fprintf(out, "    syscall\n");
                }
                else if (i > 0 && midcode[i - 1].v3 == chars) {
                    v2 = get_v2(out, i - 1, func_x);
                    fprintf(out, "    move $a0, $%d\n", v2);
                    fprintf(out, "    li $v0, 11\n");
                    fprintf(out, "    syscall\n");
                }
            }
            else if (midcode[i].v2 == 2) {
                fprintf(out, "    la $a0, str_%d\n", midcode[i].v3);
                fprintf(out, "    li $v0, 4\n");
                fprintf(out, "    syscall\n");

                if (i > 0 && midcode[i - 1].v3 == ints) {
                    v2 = get_v2(out, i - 1, func_x);
                    fprintf(out, "    move $a0, $%d\n", v2);
                    fprintf(out, "    li $v0, 1\n");
                    fprintf(out, "    syscall\n");
                }
                else if (i > 0 && midcode[i - 1].v3 == chars) {
                    v2 = get_v2(out, i - 1, func_x);
                    fprintf(out, "    move $a0, $%d\n", v2);
                    fprintf(out, "    li $v0, 11\n");
                    fprintf(out, "    syscall\n");
                }
            }

            fprintf(out, "    li $a0, %d\n", '\n');
            fprintf(out, "    li $v0, 11\n");
            fprintf(out, "    syscall\n");
        }

        break;

    case _call:
        dsp = btab[tab[midcode[i].v1].ref].vsize - btab[tab[midcode[i].v1].ref].psize + btab[tab[midcode[i].v1].ref].n_tmpvar * 4 + 8;
        dfp = btab[tab[midcode[i].v1].ref].vsize + btab[tab[midcode[i].v1].ref].n_tmpvar * 4 + 8 - 4;
        fprintf(out, "    addiu $sp, $sp, -%d\n", dsp);
        fprintf(out, "    sw $fp, 4($sp)\n");
        fprintf(out, "    sw $ra, 0($sp)\n");

        fprintf(out, "    addiu $fp, $sp, %d\n", dfp);
        fprintf(out, "    jal func_%d\n", tab[midcode[i].v1].ref);
        fprintf(out, "    lw $fp, 4($sp)\n");
        fprintf(out, "    lw $ra, 0($sp)\n");

        dsp = btab[tab[midcode[i].v1].ref].vsize + btab[tab[midcode[i].v1].ref].n_tmpvar * 4 + 8;
        fprintf(out, "    addiu $sp, $sp, %d\n", dsp);
        break;
    case _ret:
        if (midcode[i].t2 != -1) {
            v2 = get_v2(out, i, func_x);
            fprintf(out, "    move $2, $%d\n", v2);
        }
        fprintf(out, "    jr $ra\n");
        break;
    case _assign:
        if (midcode[i].t2 == _retval) { //v1一定为临时变量
            v1 = 24;
            fprintf(out, "    move $%d, $2\n", v1);
            fprintf(out, "    sw $24, -%d($fp)\n", vsize + midcode[i].v1 * 4);
        }
        else if (midcode[i].t2 == _globalvar) {//v1一定为临时变量
            v1 = 24;
            fprintf(out, "    lw $%d, global_var+%d\n", v1, tab[midcode[i].v2].adr);
            fprintf(out, "    sw $24, -%d($fp)\n", vsize + midcode[i].v1 * 4);
        }
        else { //v1一定为局部或全局变量
            v2 = get_v2(out, i, func_x);
            if (midcode[i].t1 == _globalvar) {
                fprintf(out, "    sw $%d, global_var+%d\n", v2, tab[midcode[i].v1].adr);
            }
            else if (midcode[i].t1 == _localvar) {
                fprintf(out, "    sw $%d, -%d($fp)\n", v2, tab[midcode[i].v1].adr);
            }
        }
        break;
    case _arrassign:
        v2 = get_v2(out, i, func_x);
        v3 = get_v3(out, i, func_x);

        fprintf(out, "    sll $24, $%d, 2\n", v2);
        if (tab[midcode[i].v1].lev == 0) {
            if (tab[midcode[i].v1].typ == ints) {
                fprintf(out, "    sw $%d, global_var+%d($24)\n", v3, tab[midcode[i].v1].adr);
            }
            else {
                fprintf(out, "    sb $%d, global_var+%d($24)\n", v3, tab[midcode[i].v1].adr);
            }
        }
        else {
            fprintf(out, "    subu $24, $fp, $24\n");
            if (tab[midcode[i].v1].typ == ints) {
                fprintf(out, "    sw $%d, -%d($24)\n", v3, tab[midcode[i].v1].adr);
            }
            else {
                fprintf(out, "    sb $%d, -%d($24)\n", v3, tab[midcode[i].v1].adr);
            }
        }
        break;

    case _arrload:
        v3 = get_v3(out, i, func_x);
        v1 = 24;
        fprintf(out, "    sll $24, $%d, 2\n", v3);
        if (tab[midcode[i].v2].lev == 0) {
            if (tab[midcode[i].v2].typ == ints) {
                fprintf(out, "    lw $%d, global_var+%d($24)\n", v1, tab[midcode[i].v2].adr);
            }
            else {
                fprintf(out, "    lb $%d, global_var+%d($24)\n", v1, tab[midcode[i].v2].adr);
            }
        }
        else {
            fprintf(out, "    subu $24, $fp, $24\n");
            if (tab[midcode[i].v2].typ == ints) {
                fprintf(out, "    lw $%d, -%d($24)\n", v1, tab[midcode[i].v2].adr);
            }
            else {
                fprintf(out, "    lb $%d, -%d($24)\n", v1, tab[midcode[i].v2].adr);
            }
        }
        fprintf(out, "    sw $24, -%d($fp)\n", vsize + midcode[i].v1 * 4);
        break;


    case _label:
        fprintf(out, "    label_%d:\n", midcode[i].v1);
        break;
    case _goto:
        fprintf(out, "    j label_%d\n", midcode[i].v1);
        break;
    case _bz:
        v2 = get_v2(out, i, func_x);
        fprintf(out, "    beq $%d, $0, label_%d\n", v2, midcode[i].v1);
        break;
    default:
        fprintf(out, "ERROR: unknown midcode op: %d!\n", midcode[i].op);
        printf("ERROR: unknown midcode op: %d!\n", midcode[i].op);
        break;
    }
}




void gen_obj_code()
{
    int dsp;
    FILE *out;

    out = fopen("..\\16061175_刘卫_优化前目标代码.TXT", "w");
    fprintf(out, ".data\n");
    fprintf(out, "    global_var: .space %d\n", btab[0].vsize);
    for (int i = 0; i <= sx; i++) {
        fprintf(out, "    str_%d: .asciiz \"%s\"\n", i, stab[i].c_str());
    }

    fprintf(out, "\n\n.text\n");
    int main_ref = tab[loc("main")].ref;
    dsp = btab[main_ref].vsize + btab[main_ref].n_tmpvar * 4;
    fprintf(out, "    addiu $fp, $sp, -4\n");
    fprintf(out, "    addiu $sp, $sp, %d\n", -dsp);
    fprintf(out, "    jal func_%d\n", main_ref);
    fprintf(out, "    li $v0, 10\n");
    fprintf(out, "    syscall\n\n\n");
    vector<reg> regs;


    for (int i = 1; i <= b; i++) {
        fprintf(out, "\n\nfunc_%d:\n", i);
        for (int j = btab[i].start_blk; j <= btab[i].end_blk; j++) {
            for (int k = block_tab[j].start; k <= block_tab[j].end; k++) {
                gen_mips(out, k, i);
            }
        }
    }

    fclose(out);
    printf("优化前的目标代码已输出到文件中。\n");
}