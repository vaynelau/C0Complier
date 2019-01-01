#ifndef _SYNTAX_ANALYSIS_H_
#define _SYNTAX_ANALYSIS_H_

#include "midcode.h"
#include "table.h"

using namespace std;

void syntax_analysis();
int expression(datatyp *ptyp, operandtyp *potyp);
int funccall(int i);
void statement();


#endif // !_SYNTAX_ANALYSIS_H_