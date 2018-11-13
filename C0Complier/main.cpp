#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include "lexical_analysis.h"
using namespace std;

int main(void)
{
    FILE *in = fopen("16061175_test.c", "r");
    setup(in);
    while (true) {
        insymbol();
    }
    return 0;
}