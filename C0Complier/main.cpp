#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include "lexical_analysis.h"
#include "syntax_analysis.h"
#include "tabs.h"

using namespace std;

string sourcefilename; //源代码文件名
FILE *psin; //源代码文件指针


int main(void)
{
    puts("Please input the source file name:");
    getline(cin, sourcefilename);
    if (sourcefilename.empty()) {
        //sourcefilename = "testcode.c";
        sourcefilename = "16061175_test.txt";
    }
    psin = fopen(sourcefilename.c_str(), "r");
    setup();
    lexcial_init();
    syntax_init();
    program();
    return 0;
}