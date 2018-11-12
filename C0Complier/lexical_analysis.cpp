#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <string>
#include <map>
#include <vector>
#include "my_header.h"
using namespace std;

FILE *psin;
char ch;

string id;
symbol sy;
int inum, sleng;

map<string, symbol> ksy;
map<char, symbol> sps;
vector<string> stab;
int sx;

const char *symstr[] = {
    "intcon", "charcon", "stringcon",
    "_plus", "_minus", "times", "idiv",
    "eql", "neq", "gtr", "geq", "lss", "leq",
    "lparent", "rparent", "lbracket", "rbracket", "lbrace", "rbrace",
    "comma", "semicolon", "colon", "becomes",
    "ident",
    "constsy", "intsy", "charsy", "voidsy", "mainsy", "returnsy",
    "ifsy", "switchsy", "casesy", "defaultsy", "whilesy" 
};

void setup(FILE *in)
{
    psin = in; 
    sx = -1;
    ch = ' ';

    ksy["const"] = constsy;
    ksy["if"] = ifsy;
    ksy["switch"] = switchsy;
    ksy["case"] = casesy;
    ksy["default"] = defaultsy;
    ksy["while"] = whilesy;
    ksy["return"] = returnsy;
    ksy["int"] = intsy;
    ksy["char"] = charsy;
    ksy["void"] = voidsy;
    //ksy["main"] = mainsy;

    sps['+'] = _plus;
    sps['-'] = _minus;
    sps['*'] = times;
    sps['/'] = idiv;
    sps['('] = lparent;
    sps[')'] = rparent;
    sps['['] = lbracket;
    sps[']'] = rbracket;
    sps['{'] = lbrace;
    sps['}'] = rbrace;
    sps[','] = comma;
    sps[';'] = semicolon;
    sps[':'] = colon;
}

void error()
{

}

void nextch()
{
    do {
        ch = fgetc(psin);
        if (ch == EOF) {
            fclose(psin);
            exit(0);
        }
    } while (ch == '\n');
}

void insymbol()
{
    int k;

    while (ch==' ' || ch == '\t') {
        nextch();
    }
    
    if (isalpha(ch) || ch == '_') {
        id.clear();
        do {
            id += ch;
            nextch();
        } while (isalnum(ch) || ch == '_');
        
        if (ksy.count(id)) {
            sy = ksy[id];
            
        }
        else {
            sy = ident;
        }
        printf("%s: %s\n", symstr[sy], id.c_str());
        return;
    }
    
    if (isdigit(ch)) {
        inum = 0;
        sy = intcon;
        do {
            inum = inum * 10 + ch - '0';
            nextch();
        } while (isdigit(ch));
        printf("%s: %d\n", symstr[sy], inum);
        return;
    }

    if (ch == '\'') {
        nextch();
        if (isalnum(ch) || ch == '_' || ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            inum = (int)ch;
            nextch();
            if (ch == '\'') {
                sy = charcon;
                nextch();
            }
            else {
                error();
            }
        }
        else {
            error();
        }
        printf("%s: %c\n", symstr[sy], inum);
        return;
    }

    if (ch == '\"') {
        k = 0;
        id.clear();
        nextch();
        while (ch != '\"') {
            id += ch;
            k++;
            nextch();
        }
        
        sy = stringcon;
        stab.push_back(id);
        inum = ++sx;
        sleng = k;
        nextch();
        printf("%s: %s\n", symstr[sy], id.c_str());
        return;
    }

    if (ch == '>') {
        id.clear();
        id += ch;
        nextch();
        if (ch == '=') {
            id += ch;
            sy = geq;
            nextch();
        }
        else {
            sy = gtr;
        }
        printf("%s: %s\n", symstr[sy], id.c_str());
        return;
    }

    if (ch == '<') {
        id.clear();
        id += ch;
        nextch();
        if (ch == '=') {
            id += ch;
            sy = leq;
            nextch();
        }
        else {
            sy = lss;
        }
        printf("%s: %s\n", symstr[sy], id.c_str());
        return;
    }

    if (ch == '=') {
        id.clear();
        id += ch;
        nextch();
        if (ch == '=') {
            id += ch;
            sy = eql;
            nextch();
        }
        else {
            sy = becomes;
        }
        printf("%s: %s\n", symstr[sy], id.c_str());
        return;
    }

    if (ch == '!') {
        id.clear();
        id += ch;
        nextch();
        if (ch == '=') {
            id += ch;
            sy = neq;
            nextch();
        }
        else {
            error();
        }
        printf("%s: %s\n", symstr[sy], id.c_str());
        return;
    }

    if (sps.count(ch)) {
        sy = sps[ch];
        printf("%s: %c\n", symstr[sy], ch);
        nextch();
        return;
    }

    error();
}


