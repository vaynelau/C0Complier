#ifndef _SYNTAX_ANALYSIS_H_
#define _SYNTAX_ANALYSIS_H_


using namespace std;

void syntax_analysis();
int expression(types *ptyp, int tmp);
int funccall(int i, int tmp);
void statement();
void test(set<symbol> s);
void test2(set<symbol> s1, set<symbol> s2);

#endif // !_SYNTAX_ANALYSIS_H_