#ifndef _LEXICAL_ANALYSIS_H_
#define _LEXICAL_ANALYSIS_H_

#include <cstdio>


typedef enum symbol {
    intcon, charcon, stringcon, //ÕûÊý  ×Ö·û  ×Ö·û´®
    _plus, _minus, times, idiv, //+  -  *  /
    eql, neq, gtr, geq, lss, leq, //==  !=  >  >=  <  <=
    lparent, rparent, lbracket, rbracket, lbrace, rbrace, //( ) [ ] { } 
    comma, semicolon, colon, becomes, //, ; : =  
    ident, mainsy, scanfsy, printfsy,
    constsy, returnsy, intsy, charsy, voidsy, 
    ifsy, switchsy, casesy, defaultsy, whilesy
} symbol;

void setup(FILE *in);
void insymbol();
void printsymbol();

#endif // !_LEXICAL_ANALYSIS_H_
