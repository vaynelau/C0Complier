#ifndef _MIDCODE_H_
#define _MIDCODE_H_

#include <vector>

using namespace std;

#define RET -5
#define SCANF 1
#define PRINTF 2

typedef enum {
    _const, _var, _array, _func, _para, _push, _call, _ret,
    _assign, _arrassign, _conload, _varload, _arrload, _label, _goto, _bez, _bz,
    _neg, _plus, _minus, _times, _idiv, //+  -  *  /
    _eql, _neq, _gtr, _geq, _lss, _leq, //==  !=  >  >=  <  <=
} optyp;


typedef struct {
    optyp op;
    int v1, v2, v3;
    int lev;
} midcodeitem;

extern vector<midcodeitem> midcode;
extern int mx;

void midcode_enter(optyp op, int v1, int v2, int v3);
void print_midcode();

#endif
