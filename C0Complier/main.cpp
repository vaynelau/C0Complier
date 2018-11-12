#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include "my_header.h"
using namespace std;

int main(void)
{
    FILE *in = fopen("test.cpp", "r");
    setup(in);
    while (true) {
        insymbol();
    }

}