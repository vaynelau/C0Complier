#pragma once
#ifndef MY_HEADER

#define MY_HEADER

typedef enum symbol {
    intcon, charcon, stringcon, //ÕûÊý  ×Ö·û  ×Ö·û´®
    _plus, _minus, times, idiv, //+  -  *  /
    eql, neq, gtr, geq, lss, leq, //==  !=  >  >=  <  <=
    lparent, rparent, lbracket, rbracket, lbrace, rbrace, //( ) [ ] { } 
    comma, semicolon, colon, becomes, //, ; : =  
    ident,
    constsy, intsy, charsy, voidsy, mainsy, returnsy,
    ifsy, switchsy, casesy, defaultsy, whilesy

} symbol;

void setup(FILE *in);
void insymbol();

#endif // !MY_HEADER
