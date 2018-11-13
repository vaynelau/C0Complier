#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <string>
#include "lexical_analysis.h"
using namespace std;

string sourcefile;

int main(int argc, char *argv[])
{
    putchar(127);
    puts("Please input the source file name:");
    getline(cin, sourcefile);
    if (sourcefile.empty()) {
        sourcefile = "test.c";
    }
    FILE *psin = fopen(sourcefile.c_str(), "r");
    setup(psin);
    while (true) {
        insymbol();
        
    }
    return 0;
}