#ifndef _OPTIMIZATION_H_
#define _OPTIMIZATION_H_
#include <iostream>
#include <cstdio>
#include <string>

using namespace std;


typedef struct {
    //int id;
    int start, end; //基本块的开始、结束的中间代码位置
    vector<int> pres, follows;//前驱基本块数组、后继基本块数组
    int n_pre, n_follow; //前驱基本块、后继基本块的数量
    vector<bool> def1, use1, in1, out1;
    vector<bool> def2, use2, in2, out2;
} block_item;

typedef struct {
    int index;
    int kind;
    int num;
} reg;

extern vector<block_item> block_tab;
extern int block_index;

void code_opt();

#endif // !_OPTIMIZATION_H_