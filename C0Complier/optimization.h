#ifndef _OPTIMIZATION_H_
#define _OPTIMIZATION_H_
#include <iostream>
#include <cstdio>
#include <string>

using namespace std;


typedef struct {
    //int id;
    int start, end; //基本块的开始、结束位置
    vector<int> pres, follows;//前驱基本块数组、后继基本块数组
    int n_pre, n_follow; //前驱基本块、后继基本块的数量
} block_item;



void code_opt();

#endif // !_OPTIMIZATION_H_