#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
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
    item.t1 = -1;
    item.t2 = -1;
    item.t3 = -1;
    item.lev = b;
    midcode.push_back(item);
}

void midcode_enter2(optyp op, int v1, int v2, int v3, int t1, int t2, int t3)
{
    ++mx;
    midcodeitem item;
    item.op = op;
    item.v1 = v1;
    item.v2 = v2;
    item.v3 = v3;
    item.t1 = t1;
    item.t2 = t2;
    item.t3 = t3;

    item.lev = b;
    midcode.push_back(item);
}

string int_to_str(int n)
{
    ostringstream stream;
    stream << n;
    return stream.str();
}


string to_str(int val, int typ)
{
    string str;
    switch (typ) {
    case _intcon:
        str = int_to_str(val);
        break;
    case _charcon:
        str = "'c'";
        str[1] = (char)val;
        break;
    case _strcon:
        str = "\"";
        str += stab[val];
        str += "\"";
        break;
    case _localvar:
    case _globalvar:
    case _arrvar:
        str = tab[val].name;
        break;
    case _tmpvar:
        str = "$t";
        str += int_to_str(val);
        break;
    case _retval:
        str = "RET";
        break;
    default:
        str = "ERROR";
        break;
    }
    return str;
}


void print_midcode(FILE *out, int index)
{
    midcodeitem item = midcode[index];

    fprintf(out, "    ");
    switch (item.op) {
    case _condef:
        if (item.v1 == ints) {
            fprintf(out, "const int %s = %d\n", tab[item.v2].name.c_str(), item.v3);
        }
        else {
            fprintf(out, "const char %s = '%c'\n", tab[item.v2].name.c_str(), item.v3);
        }
        break;
    case _vardef:
        if (item.v1 == ints) {
            fprintf(out, "var int %s\n", tab[item.v2].name.c_str());
        }
        else {
            fprintf(out, "var char %s\n", tab[item.v2].name.c_str());
        }
        break;
    case _arrdef:
        if (item.v1 == ints) {
            fprintf(out, "array int %s %d\n", tab[item.v2].name.c_str(), item.v3);
        }
        else {
            fprintf(out, "array char %s %d\n", tab[item.v2].name.c_str(), item.v3);
        }
        break;
    case _funcdef:
        fprintf(out, "\n\n");
        if (item.v1 == ints) {
            fprintf(out, "int %s()\n", tab[item.v2].name.c_str());
        }
        else if (item.v1 == chars) {
            fprintf(out, "char %s()\n", tab[item.v2].name.c_str());
        }
        else {
            fprintf(out, "void %s()\n", tab[item.v2].name.c_str());
        }
        break;
    case _paradef:
        if (item.v1 == ints) {
            fprintf(out, "para int %s\n", tab[item.v2].name.c_str());
        }
        else {
            fprintf(out, "para char %s\n", tab[item.v2].name.c_str());
        }
        break;

    case _push:
        fprintf(out, "push %s\n", to_str(item.v2, item.t2).c_str());
        break;
    case _push_scanf:
        fprintf(out, "push %s\n", to_str(item.v1, item.t1).c_str());
        break;
    case _call:
    case _std_call:
        fprintf(out, "call %s\n", tab[item.v1].name.c_str());
        break;

    case _ret:
        if (item.t1 != -1) {
            fprintf(out, "ret %s\n", to_str(item.v2, item.t2).c_str());
        }
        else {
            fprintf(out, "ret\n");
        }
        break;

    case _assign:
        fprintf(out, "%s = %s\n", to_str(item.v1, item.t1).c_str(), to_str(item.v2, item.t2).c_str());
        break;
    case _arrassign:
        fprintf(out, "%s[%s] = %s\n", tab[item.v1].name.c_str(), to_str(item.v2, item.t2).c_str(), to_str(item.v3, item.t3).c_str());
        break;
        /*
        case _conload:
            if (item.v2 == ints) {
                fprintf(out, "$t%d = %d\n", item.v1, item.v3);
            }
            else if (item.v2 == chars) {
                fprintf(out, "$t%d = '%c'\n", item.v1, item.v3);
            }
            else {
                fprintf(out, "$t%d = \"%s\"\n", item.v1, stab[item.v3].c_str());
            }
            break;
        case _varload:
            fprintf(out, "$t%d = %s\n", item.v1, tab[item.v2].name.c_str());
            break;
        */
    case _arrload:
        fprintf(out, "%s = %s[%s]\n", to_str(item.v1, item.t1).c_str(), tab[item.v2].name.c_str(), to_str(item.v3, item.t3).c_str());
        break;

    case _label:
        fprintf(out, "label_%d:\n", item.v1);
        break;
    case _goto:
        fprintf(out, "GOTO label_%d\n", item.v1);
        break;
    case _bz:
        fprintf(out, "BZ label_%d\n", item.v1);
        break;

    case _neg:
        fprintf(out, "%s = -%s\n", to_str(item.v1, item.t1).c_str(), to_str(item.v2, item.t2).c_str());
        break;
    case _plus:
        fprintf(out, "%s = %s + %s\n", to_str(item.v1, item.t1).c_str(), to_str(item.v2, item.t2).c_str(), to_str(item.v3, item.t3).c_str());
        break;
    case _minus:
        fprintf(out, "%s = %s - %s\n", to_str(item.v1, item.t1).c_str(), to_str(item.v2, item.t2).c_str(), to_str(item.v3, item.t3).c_str());
        break;
    case _times:
        fprintf(out, "%s = %s * %s\n", to_str(item.v1, item.t1).c_str(), to_str(item.v2, item.t2).c_str(), to_str(item.v3, item.t3).c_str());
        break;
    case _idiv:
        fprintf(out, "%s = %s / %s\n", to_str(item.v1, item.t1).c_str(), to_str(item.v2, item.t2).c_str(), to_str(item.v3, item.t3).c_str());
        break;
    case _eql:
        fprintf(out, "%s == %s\n", to_str(item.v2, item.t2).c_str(), to_str(item.v3, item.t3).c_str());
        break;
    case _neq:
        fprintf(out, "%s != %s\n", to_str(item.v2, item.t2).c_str(), to_str(item.v3, item.t3).c_str());
        break;
    case _gtr:
        fprintf(out, "%s > %s\n", to_str(item.v2, item.t2).c_str(), to_str(item.v3, item.t3).c_str());
        break;
    case _geq:
        fprintf(out, "%s >= %s\n", to_str(item.v2, item.t2).c_str(), to_str(item.v3, item.t3).c_str());
        break;
    case _lss:
        fprintf(out, "%s < %s\n", to_str(item.v2, item.t2).c_str(), to_str(item.v3, item.t3).c_str());
        break;
    case _leq:
        fprintf(out, "%s <= %s\n", to_str(item.v2, item.t2).c_str(), to_str(item.v3, item.t3).c_str());
        break;

    default:
        fprintf(out, "ERROR: unknown midcode op: %d!\n", item.op);
        printf("ERROR: unknown midcode op: %d\n!", item.op);
        break;
    }
}


void print_midcodes()
{
    FILE *out;

    out = fopen("..\\16061175_刘卫_优化前中间代码.TXT", "w");
    for (int i = 0; i <= mx; i++) {
        print_midcode(out, i);
    }
    fclose(out);
    printf("优化前的中间代码已输出到文件中。\n");
}

void print_midcodes_opt()
{
    FILE *out;

    out = fopen("..\\16061175_刘卫_优化后中间代码.TXT", "w");
    for (int i = 0; i <= mx; i++) {
        print_midcode(out, i);
    }
    fclose(out);
    printf("优化后的中间代码已输出到文件中。\n");
}
