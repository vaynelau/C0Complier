#ifndef _SYNTAX_ANALYSIS_H_
#define _SYNTAX_ANALYSIS_H_
#define RET -5

using namespace std;

void syntax_analysis();

int expression(types *ptyp, int tmp);
int funccall(int i, int tmp);
void statement();

#endif // !_SYNTAX_ANALYSIS_H_