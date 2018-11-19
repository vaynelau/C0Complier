#include "error.h"
#include "lexical_analysis.h"



void error(int n)
{
    printf("%s:%d:%d: error: ", sourcefilename.c_str(), lcnt, chcnt);
    switch (n) {
        //case 0:
            //puts("illegal integer constant with a leading '0'");
        //    break;
    case 1:
        printf("illegal character constant '%c'\n", ch);
        break;
    case 2:
        puts("missing terminating ' character");
        break;
    case 3:
        puts("missing terminating \" character");
        break;
    case 4:
        puts("illegal '!' in program");
        break;
    case 5:
        printf("illegal '%c' in program\n", ch);
        nextch();
        break;
    default:
        break;
    }
}