#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <string>
#include <vector>
#include "table.h"
#include "midcode.h"
#include "mips.h"

using namespace std;

void gen_mips()
{
    int dsp, dfp;
    FILE *out;
    //int v1;
    int v2, v3;
    int printf_para_cnt = 0;


    out = fopen("..\\a.asm", "w");
    fprintf(out, ".data\n");
    fprintf(out, "    global_var: .space %d\n", btab[0].vsize);
    for (int i = 0; i <= sx; i++) {
        fprintf(out, "    str_%d: .asciiz \"%s\"\n", i, stab[i].c_str());
    }

    fprintf(out, "\n\n.text\n");
    int mainref = tab[loc("main")].ref;
    dsp = btab[mainref].vsize;
    fprintf(out, "    addiu $fp, $sp, -4\n");
    fprintf(out, "    addiu $sp, $sp, %d\n", -dsp);
    fprintf(out, "    jal func_%d\n", mainref);
    fprintf(out, "    li $v0, 10\n");
    fprintf(out, "    syscall\n");

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
                        fprintf(out, "    sw $v0, -%d($fp)\n", tab[midcode[i].v1].adr);
                    }
                    else {
                        fprintf(out, "    li $v0, 12\n");
                        fprintf(out, "    syscall\n");
                        fprintf(out, "    sb $v0, -%d($fp)\n", tab[midcode[i].v1].adr);
                    }
                }
            }
            else if (midcode[i].v2 == PRINTF) {
                printf_para_cnt++;
                //if (midcode[i].v1 >= 8) { //正常来说不会执行
                //    fprintf(out, "    lw $t9, 0($sp)\n");
                //    fprintf(out, "    addiu $sp, $sp, 4\n");
                //}
            }
            else {
                if (midcode[i].v1 >= 8) {
                    fprintf(out, "    lw $t9, 0($sp)\n");
                    fprintf(out, "    addiu $sp, $sp, 4\n");
                }
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t%d, 0($sp)\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1));
            }
            break;
        case _call:
            if (tab[midcode[i].v1].name != "scanf" && tab[midcode[i].v1].name != "printf") {
                dsp = btab[tab[midcode[i].v1].ref].vsize - btab[tab[midcode[i].v1].ref].psize + 40 + 8;
                dfp = btab[tab[midcode[i].v1].ref].vsize + 40 + 8 - 4;
                fprintf(out, "    addiu $sp, $sp, -%d\n", dsp);
                fprintf(out, "    sw $fp, 4($sp)\n");
                fprintf(out, "    sw $ra, 0($sp)\n");
                for (int j = 0; j < 10; j++) {
                    fprintf(out, "    sw $t%d, %d($sp)\n", j, 8 + 4 * j);
                }
                fprintf(out, "    addiu $fp, $sp, %d\n", dfp);
                fprintf(out, "    jal func_%d\n", tab[midcode[i].v1].ref);
                fprintf(out, "    lw $fp, 4($sp)\n");
                fprintf(out, "    lw $ra, 0($sp)\n");
                for (int j = 0; j < 10; j++) {
                    fprintf(out, "    lw $t%d, %d($sp)\n", j, 8 + 4 * j);
                }
                dsp = btab[tab[midcode[i].v1].ref].vsize + 40 + 8;
                fprintf(out, "    addiu $sp, $sp, %d\n", dsp);
            }
            else if (tab[midcode[i].v1].name == "printf") {
                if (printf_para_cnt == 1) {
                    if (i>0 && midcode[i-1].v3 == strs) {
                        fprintf(out, "    move $a0, $t0\n");
                        fprintf(out, "    li $v0, 4\n");
                        fprintf(out, "    syscall\n");
                    }
                    else if (i > 0 && midcode[i-1].v3 == ints) {
                        fprintf(out, "    move $a0, $t0\n");
                        fprintf(out, "    li $v0, 1\n");
                        fprintf(out, "    syscall\n");
                    }
                    else if (i > 0 && midcode[i - 1].v3 == chars) {
                        fprintf(out, "    move $a0, $t0\n");
                        fprintf(out, "    li $v0, 11\n");
                        fprintf(out, "    syscall\n");
                    }
                }
                else if (printf_para_cnt == 2) {
                    fprintf(out, "    move $a0, $t0\n");
                    fprintf(out, "    li $v0, 4\n");
                    fprintf(out, "    syscall\n");

                    if (i > 0 && midcode[i - 1].v3 == ints) {
                        fprintf(out, "    move $a0, $t1\n");
                        fprintf(out, "    li $v0, 1\n");
                        fprintf(out, "    syscall\n");
                    }
                    else if (i > 0 && midcode[i - 1].v3 == chars) {
                        fprintf(out, "    move $a0, $t1\n");
                        fprintf(out, "    li $v0, 11\n");
                        fprintf(out, "    syscall\n");
                    }
                }
                
                fprintf(out, "    li $a0, %d\n", '\n');
                fprintf(out, "    li $v0, 11\n");
                fprintf(out, "    syscall\n");
                printf_para_cnt = 0;
            }
            break;
        case _ret:
            if (midcode[i].v1 != -1) {
                if (midcode[i].v1 >= 8) {
                    fprintf(out, "    lw $t9, 0($sp)\n");
                    fprintf(out, "    addiu $sp, $sp, 4\n");
                }
                fprintf(out, "    move $v0, $t%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1));
            }
            fprintf(out, "    jr $ra\n");
            break;
        case _assign:
            if (midcode[i].v2 == RET) {
                fprintf(out, "    move $t%d, $v0\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1));
                if (midcode[i].v1 >= 8) {
                    fprintf(out, "    addiu $sp, $sp, -4\n");
                    fprintf(out, "    sw $t9, 0($sp)\n");
                }
            }
            else {
                if (midcode[i].v2 >= 8) {
                    fprintf(out, "    lw $t9, 0($sp)\n");
                    fprintf(out, "    addiu $sp, $sp, 4\n");
                }
                if (tab[midcode[i].v1].lev == 0) {
                    if (tab[midcode[i].v1].typ == ints) {
                        fprintf(out, "    sw $t%d, global_var+%d\n", (midcode[i].v2 >= 8 ? 9: midcode[i].v2), tab[midcode[i].v1].adr);
                    }
                    else {
                        fprintf(out, "    sb $t%d, global_var+%d\n", (midcode[i].v2 >= 8 ? 9 : midcode[i].v2), tab[midcode[i].v1].adr);
                    }
                }
                else {
                    if (tab[midcode[i].v1].typ == ints) {
                        fprintf(out, "    sw $t%d, -%d($fp)\n", (midcode[i].v2 >= 8 ? 9 : midcode[i].v2), tab[midcode[i].v1].adr);
                    }
                    else {
                        fprintf(out, "    sb $t%d, -%d($fp)\n", (midcode[i].v2 >= 8 ? 9 : midcode[i].v2), tab[midcode[i].v1].adr);
                    }
                }
            }
            break;
        case _arrassign:
            if (midcode[i].v3 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            v3 = (midcode[i].v3 >= 8 ? 9 : midcode[i].v3);
            if (midcode[i].v2 >= 8) {
                fprintf(out, "    lw $t8, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            v2 = (midcode[i].v2 >= 8 ? 8 : midcode[i].v2);
            fprintf(out, "    sll $t%d, $t%d, 2\n", v2, v2);
            if (tab[midcode[i].v1].lev == 0) {
                if (tab[midcode[i].v1].typ == ints) {
                    fprintf(out, "    sw $t%d, global_var+%d($t%d)\n", v3, tab[midcode[i].v1].adr, v2);
                }
                else {
                    fprintf(out, "    sb $t%d, global_var+%d($t%d)\n", v3, tab[midcode[i].v1].adr, v2);
                }
            }
            else {
                fprintf(out, "    subu $t%d, $fp, $t%d\n", v2, v2);
                if (tab[midcode[i].v1].typ == ints) {
                    fprintf(out, "    sw $t%d, -%d($t%d)\n", v3, tab[midcode[i].v1].adr, v2);
                }
                else {
                    fprintf(out, "    sb $t%d, -%d($t%d)\n", v3, tab[midcode[i].v1].adr, v2);
                }
            }
            break;
        case _conload: //tmp, ints, inum * sign
            if (midcode[i].v2 == ints || midcode[i].v2 == chars) {
                if (midcode[i].v1 < 8) {
                    fprintf(out, "    li $t%d, %d\n", midcode[i].v1, midcode[i].v3);
                }
                else {
                    fprintf(out, "    li $t9, %d\n", midcode[i].v3);
                    fprintf(out, "    addiu $sp, $sp, -4\n");
                    fprintf(out, "    sw $t9, 0($sp)\n");
                }
            }
            else {
                fprintf(out, "    la $t%d, str_%d\n", midcode[i].v1, midcode[i].v3);
            }
            break;
        case _varload:
            if (tab[midcode[i].v2].obj == constant) {
                fprintf(out, "    li $t%d, %d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), tab[midcode[i].v2].adr);
            }
            else if (tab[midcode[i].v2].lev == 0) {
                if (tab[midcode[i].v2].typ == ints) {
                    fprintf(out, "    lw $t%d, global_var+%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), tab[midcode[i].v2].adr);
                }
                else {
                    fprintf(out, "    lb $t%d, global_var+%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), tab[midcode[i].v2].adr);
                }
            }
            else {
                if (tab[midcode[i].v2].typ == ints) {
                    fprintf(out, "    lw $t%d, -%d($fp)\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), tab[midcode[i].v2].adr);
                }
                else {
                    fprintf(out, "    lb $t%d, -%d($fp)\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), tab[midcode[i].v2].adr);
                }
            }
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        case _arrload:
            if (midcode[i].v3 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            v3 = (midcode[i].v3 >= 8 ? 9 : midcode[i].v3);
            
            fprintf(out, "    sll $t%d, $t%d, 2\n", v3, v3);
            if (tab[midcode[i].v2].lev == 0) {
                if (tab[midcode[i].v2].typ == ints) {
                    fprintf(out, "    lw $t%d, global_var+%d($t%d)\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), tab[midcode[i].v2].adr, v3);
                }
                else {
                    fprintf(out, "    lb $t%d, global_var+%d($t%d)\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), tab[midcode[i].v2].adr, v3);
                }
            }
            else {
                fprintf(out, "    subu $t%d, $fp, $t%d\n", v3, v3);
                if (tab[midcode[i].v2].typ == ints) {
                    fprintf(out, "    lw $t%d, -%d($t%d)\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), tab[midcode[i].v2].adr, v3);
                }
                else {
                    fprintf(out, "    lb $t%d, -%d($t%d)\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), tab[midcode[i].v2].adr, v3);
                }
            }
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        case _label:
            fprintf(out, "    label_%d:\n", midcode[i].v1);
            break;
        case _goto:
            fprintf(out, "    j label_%d\n", midcode[i].v1);
            break;
        case _bz:
            //
            fprintf(out, "    beq $t%d, $0, label_%d\n", midcode[i].v2, midcode[i].v1);
            break;
        case _neg:
            if (midcode[i].v2 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            fprintf(out, "    neg $t%d, $t%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), (midcode[i].v2 >= 8 ? 9 : midcode[i].v2));
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        case _plus:
            if (midcode[i].v3 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            if (midcode[i].v2 >= 8) {
                fprintf(out, "    lw $t8, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            fprintf(out, "    addu $t%d, $t%d, $t%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), (midcode[i].v2 >= 8 ? 8 : midcode[i].v2), (midcode[i].v3 >= 8 ? 9 : midcode[i].v3));
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        case _minus:
            if (midcode[i].v3 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            if (midcode[i].v2 >= 8) {
                fprintf(out, "    lw $t8, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            fprintf(out, "    subu $t%d, $t%d, $t%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), (midcode[i].v2 >= 8 ? 8 : midcode[i].v2), (midcode[i].v3 >= 8 ? 9 : midcode[i].v3));
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        case _times:
            if (midcode[i].v3 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            if (midcode[i].v2 >= 8) {
                fprintf(out, "    lw $t8, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            fprintf(out, "    mul $t%d, $t%d, $t%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), (midcode[i].v2 >= 8 ? 8 : midcode[i].v2), (midcode[i].v3 >= 8 ? 9 : midcode[i].v3));
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        case _idiv:
            if (midcode[i].v3 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            if (midcode[i].v2 >= 8) {
                fprintf(out, "    lw $t8, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            fprintf(out, "    div $t%d, $t%d, $t%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), (midcode[i].v2 >= 8 ? 8 : midcode[i].v2), (midcode[i].v3 >= 8 ? 9 : midcode[i].v3));
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        case _eql:
            if (midcode[i].v3 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            if (midcode[i].v2 >= 8) {
                fprintf(out, "    lw $t8, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            fprintf(out, "    seq $t%d, $t%d, $t%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), (midcode[i].v2 >= 8 ? 8 : midcode[i].v2), (midcode[i].v3 >= 8 ? 9 : midcode[i].v3));
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        case _neq:
            if (midcode[i].v3 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            if (midcode[i].v2 >= 8) {
                fprintf(out, "    lw $t8, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            fprintf(out, "    sne $t%d, $t%d, $t%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), (midcode[i].v2 >= 8 ? 8 : midcode[i].v2), (midcode[i].v3 >= 8 ? 9 : midcode[i].v3));
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        case _gtr:
            if (midcode[i].v3 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            if (midcode[i].v2 >= 8) {
                fprintf(out, "    lw $t8, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            fprintf(out, "    sgt $t%d, $t%d, $t%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), (midcode[i].v2 >= 8 ? 8 : midcode[i].v2), (midcode[i].v3 >= 8 ? 9 : midcode[i].v3));
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        case _geq:
            if (midcode[i].v3 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            if (midcode[i].v2 >= 8) {
                fprintf(out, "    lw $t8, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            fprintf(out, "    sge $t%d, $t%d, $t%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), (midcode[i].v2 >= 8 ? 8 : midcode[i].v2), (midcode[i].v3 >= 8 ? 9 : midcode[i].v3));
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        case _lss:
            if (midcode[i].v3 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            if (midcode[i].v2 >= 8) {
                fprintf(out, "    lw $t8, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            fprintf(out, "    slt $t%d, $t%d, $t%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), (midcode[i].v2 >= 8 ? 8 : midcode[i].v2), (midcode[i].v3 >= 8 ? 9 : midcode[i].v3));
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        case _leq:
            if (midcode[i].v3 >= 8) {
                fprintf(out, "    lw $t9, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            if (midcode[i].v2 >= 8) {
                fprintf(out, "    lw $t8, 0($sp)\n");
                fprintf(out, "    addiu $sp, $sp, 4\n");
            }
            fprintf(out, "    sle $t%d, $t%d, $t%d\n", (midcode[i].v1 >= 8 ? 9 : midcode[i].v1), (midcode[i].v2 >= 8 ? 8 : midcode[i].v2), (midcode[i].v3 >= 8 ? 9 : midcode[i].v3));
            if (midcode[i].v1 >= 8) {
                fprintf(out, "    addiu $sp, $sp, -4\n");
                fprintf(out, "    sw $t9, 0($sp)\n");
            }
            break;
        default:
            fprintf(out, "unknown midcode op: %d\n", midcode[i].op);
            printf("unknown midcode op: %d\n", midcode[i].op);
            break;
        }
    }

    fclose(out);
    printf("生成的mips目标代码已输出到a.asm文件中。\n");
}