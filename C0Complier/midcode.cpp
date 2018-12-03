#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <string>
#include <vector>
#include "midcode.h"
#include "table.h"

using namespace std;

vector<midcodeitem> midcode;
int mx = -1;

void midcode_enter(optyp op, int v1, int v2, int v3)
{
    ++mx;
    midcodeitem item;
    item.op = op;
    item.v1 = v1;
    item.v2 = v2;
    item.v3 = v3;
    item.lev = b;
    midcode.push_back(item);
}

void print_midcode()
{
    FILE *out;
    
    out = fopen("midcode.txt", "w");

    for (int i = 0; i <= mx; i++) {
        switch (midcode[i].op) {
        case _const:
            if (midcode[i].v1 == ints) {
                fprintf(out, "const int %s = %d\n", tab[midcode[i].v2].name.c_str(), midcode[i].v3);
            }
            else {
                fprintf(out, "const char %s = '%c'\n", tab[midcode[i].v2].name.c_str(), midcode[i].v3);
            }
            break;
        case _var:
            if (midcode[i].v1 == ints) {
                fprintf(out, "var int %s\n", tab[midcode[i].v2].name.c_str());
            }
            else {
                fprintf(out, "var char %s\n", tab[midcode[i].v2].name.c_str());
            }
            break;

        case _array:
            if (midcode[i].v1 == ints) {
                fprintf(out, "array int %s %d\n", tab[midcode[i].v2].name.c_str(), midcode[i].v3);
            }
            else {
                fprintf(out, "array char %s %d\n", tab[midcode[i].v2].name.c_str(), midcode[i].v3);
            }
            break;
        case _func:
            if (midcode[i].v1 == ints) {
                fprintf(out, "int %s()\n", tab[midcode[i].v2].name.c_str());
            }
            else if (midcode[i].v1 == chars) {
                fprintf(out, "char %s()\n", tab[midcode[i].v2].name.c_str());
            }
            else {
                fprintf(out, "void %s()\n", tab[midcode[i].v2].name.c_str());
            }
            break;
        case _para:
            if (midcode[i].v1 == ints) {
                fprintf(out, "para int %s\n", tab[midcode[i].v2].name.c_str());
            }
            else {
                fprintf(out, "para char %s\n", tab[midcode[i].v2].name.c_str());
            }
            break;
        case _push:
            fprintf(out, "push $t%d\n", midcode[i].v1);
            break;
        case _call:
            fprintf(out, "call %s\n", tab[midcode[i].v1].name.c_str());
            break;
        case _ret:
            if (midcode[i].v1 != -1) {
                fprintf(out, "ret $t%d\n", midcode[i].v1);
            }
            else {
                fprintf(out, "ret\n");
            }
            break;
        case _assign:
            if (midcode[i].v2 == RET) {
                fprintf(out, "$t%d = RET\n", midcode[i].v1);
            }
            else {
                fprintf(out, "%s = $t%d\n", tab[midcode[i].v1].name.c_str(), midcode[i].v2);
            }
            break;
        case _arrassign:
            fprintf(out, "%s[$t%d] = $t%d\n", tab[midcode[i].v1].name.c_str(), midcode[i].v2, midcode[i].v3);
            break;
        case _conload:
            if (midcode[i].v2 == ints) {
                fprintf(out, "$t%d = %d\n", midcode[i].v1, midcode[i].v3);
            }
            else if (midcode[i].v2 == chars) {
                fprintf(out, "$t%d = '%c'\n", midcode[i].v1, midcode[i].v3);
            }
            else {
                fprintf(out, "$t%d = \"%s\"\n", midcode[i].v1, stab[midcode[i].v3].c_str());
            }
            break;
        case _varload:
            fprintf(out, "$t%d = %s\n", midcode[i].v1, tab[midcode[i].v2].name.c_str());
            break;
        case _arrload:
            fprintf(out, "$t%d = %s[$t%d]\n", midcode[i].v1, tab[midcode[i].v2].name.c_str(), midcode[i].v3);
            break;
        case _label:
            fprintf(out, "label_%d:\n", midcode[i].v1);
            break;
        case _goto:
            fprintf(out, "GOTO label_%d\n", midcode[i].v1);
            break;
        case _bz:
            fprintf(out, "BZ label_%d\n", midcode[i].v1);
            break;
        case _neg:
            fprintf(out, "$t%d = -$t%d\n", midcode[i].v1, midcode[i].v2);
            break;
        case _plus:
            fprintf(out, "$t%d = $t%d + $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _minus:
            fprintf(out, "$t%d = $t%d - $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _times:
            fprintf(out, "$t%d = $t%d * $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _idiv:
            fprintf(out, "$t%d = $t%d / $t%d\n", midcode[i].v1, midcode[i].v2, midcode[i].v3);
            break;
        case _eql:
            fprintf(out, "$t%d == $t%d\n", midcode[i].v2, midcode[i].v3);
            break;
        case _neq:
            fprintf(out, "$t%d != $t%d\n", midcode[i].v2, midcode[i].v3);
            break;
        case _gtr:
            fprintf(out, "$t%d > $t%d\n", midcode[i].v2, midcode[i].v3);
            break;
        case _geq:
            fprintf(out, "$t%d >= $t%d\n", midcode[i].v2, midcode[i].v3);
            break;
        case _lss:
            fprintf(out, "$t%d < $t%d\n", midcode[i].v2, midcode[i].v3);
            break;
        case _leq:
            fprintf(out, "$t%d <= $t%d\n", midcode[i].v2, midcode[i].v3);
            break;

        default:
            fprintf(out, "unknown midcode op: %d\n", midcode[i].op);
            printf("unknown midcode op: %d\n", midcode[i].op);
            break;
        }
    }
    fclose(out);
    printf("生成的中间代码已输出到midcode.txt文件中。\n");
}
