#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <vector>
#include "optimization.h"
#include "midcode.h"
#include "table.h"

using namespace std;
vector<block_item> block_tab;
int block_index = -1;

void block_partition()
{
    int i;
    int b_index = 0;
    for (i = 0; i <= mx && midcode[i].op != _funcdef; i++);
    i++;
    for (; i <= mx; i++) {
        b_index++;
        bool isfirst = true;
        for (; i <= mx && (midcode[i].op == _condef || midcode[i].op == _vardef || midcode[i].op == _arrdef || midcode[i].op == _paradef); i++);
        for (; i <= mx && midcode[i].op != _funcdef; i++) {
            if (isfirst || midcode[i].op == _label || midcode[i].op == _call || midcode[i - 1].op == _call || midcode[i - 1].op == _goto || midcode[i - 1].op == _bz || midcode[i - 1].op == _ret) {
                ++block_index;
                block_item block;
                block.start = i;
                block.n_pre = block.n_follow = 0;
                block_tab.push_back(block);
                btab[b_index].end_blk = block_index;
                if (isfirst) {
                    btab[b_index].start_blk = block_index;
                    isfirst = false;
                }
            }
            block_tab[block_index].end = i;
        } //循环结束时midcode[i].op == _funcdef，经过外层循环的i++，正好抵消
    }
}

void build_flow_graph()
{
    for (int i = 1; i <= b; i++) {
        for (int j = btab[i].start_blk; j <= btab[i].end_blk; j++) {

            int end = block_tab[j].end;

            if (midcode[end].op == _goto || midcode[end].op == _bz) {
                for (int k = btab[i].start_blk; k <= btab[i].end_blk; k++) {
                    int start = block_tab[k].start;
                    if (midcode[start].op == _label && midcode[start].v1 == midcode[end].v1) {
                        block_tab[j].follows.push_back(k);
                        block_tab[j].n_follow++;
                        block_tab[k].pres.push_back(j);
                        block_tab[k].n_pre++;
                        break;
                    }
                }
            }

            if (j < btab[i].end_blk && midcode[end].op != _goto && midcode[end].op != _ret) {
                int k = j + 1;
                block_tab[j].follows.push_back(k);
                block_tab[j].n_follow++;
                block_tab[k].pres.push_back(j);
                block_tab[k].n_pre++;
            }

        }
    }
}


optyp s1[] = {
    _arrload,
    _assign, _arrassign,//赋值四元式
    _neg, _plus, _minus, _times, _idiv, //运算四元式
    _eql, _neq, _gtr, _geq, _lss, _leq,
    _bz, _ret, _push, _push_scanf
};
set<optyp> opset1(s1, s1 + sizeof(s1) / sizeof(s1[0]));


void data_flow_analysis()
{
    for (int i = 1; i <= b; i++) {
        int n_localvar = btab[i].n_localvar;
        int n_tmpvar = btab[i].n_tmpvar;
        for (int j = btab[i].start_blk; j <= btab[i].end_blk; j++) {
            block_tab[j].def1.resize(n_localvar, false);
            block_tab[j].use1.resize(n_localvar, false);
            block_tab[j].in1.resize(n_localvar, false);
            block_tab[j].out1.resize(n_localvar, false);
            block_tab[j].def2.resize(n_tmpvar, false);
            block_tab[j].use2.resize(n_tmpvar, false);
            block_tab[j].in2.resize(n_tmpvar, false);
            block_tab[j].out2.resize(n_tmpvar, false);

            for (int k = block_tab[j].start; k <= block_tab[j].end; k++) {
                if (opset1.count(midcode[k].op)) {
                    if (midcode[k].t2 == _localvar && !block_tab[j].def1[tab[midcode[k].v2].pos]) {
                        block_tab[j].use1[tab[midcode[k].v2].pos] = true;
                    }
                    else if (midcode[k].t2 == _tmpvar && !block_tab[j].def2[midcode[k].v2]) {
                        block_tab[j].use2[midcode[k].v2] = true;
                    }
                    if (midcode[k].t3 == _localvar && !block_tab[j].def1[tab[midcode[k].v3].pos]) {
                        block_tab[j].use1[tab[midcode[k].v3].pos] = true;
                    }
                    else if (midcode[k].t3 == _tmpvar && !block_tab[j].def2[midcode[k].v3]) {
                        block_tab[j].use2[midcode[k].v3] = true;
                    }

                    if (midcode[k].t1 == _localvar && !block_tab[j].use1[tab[midcode[k].v1].pos]) {
                        block_tab[j].def1[tab[midcode[k].v1].pos] = true;
                    }
                    else if (midcode[k].t1 == _tmpvar && !block_tab[j].use2[midcode[k].v1]) {
                        block_tab[j].def2[midcode[k].v1] = true;
                    }
                }

            }
        }

        bool flag = true;
        while (flag) {
            flag = false;
            for (int j = btab[i].start_blk; j <= btab[i].end_blk; j++) {
                for (int k = 0; k < block_tab[j].n_follow; k++) {
                    int follow = block_tab[j].follows[k];
                    for (int l = 0; l < block_tab[j].out1.size(); l++) {
                        block_tab[j].out1[l] = (block_tab[j].out1[l] || block_tab[follow].in1[l]);
                    }
                    for (int l = 0; l < block_tab[j].out2.size(); l++) {
                        block_tab[j].out2[l] = (block_tab[j].out2[l] || block_tab[follow].in2[l]);
                    }
                }
                for (int l = 0; l < block_tab[j].in1.size(); l++) {
                    bool tmp = block_tab[j].use1[l] || (block_tab[j].out1[l] && !block_tab[j].def1[l]);
                    if (block_tab[j].in1[l] != tmp) {
                        block_tab[j].in1[l] = tmp;
                        flag = true;
                    }
                }
                for (int l = 0; l < block_tab[j].in2.size(); l++) {
                    bool tmp = block_tab[j].use2[l] || (block_tab[j].out2[l] && !block_tab[j].def2[l]);
                    if (block_tab[j].in2[l] != tmp) {
                        block_tab[j].in2[l] = tmp;
                        flag = true;
                    }
                }
            }
        }
    }
}


void build_conflict_graph()
{
    for (int i = 1; i <= b; i++) {
        int n_localvar = btab[i].n_localvar;
        int n_tmpvar = btab[i].n_tmpvar;
        btab[i].conflict_graph.resize(n_localvar + n_tmpvar);
        for (int j = 0; j < btab[i].conflict_graph.size(); j++) {
            btab[i].conflict_graph[j].resize(n_localvar + n_tmpvar, false);
        }
        for (int j = btab[i].start_blk; j <= btab[i].end_blk; j++) {
            block_tab[j].def1.insert(block_tab[j].def1.end(), block_tab[j].def2.begin(), block_tab[j].def2.end());
            block_tab[j].use1.insert(block_tab[j].use1.end(), block_tab[j].use2.begin(), block_tab[j].use2.end());
            block_tab[j].in1.insert(block_tab[j].in1.end(), block_tab[j].in2.begin(), block_tab[j].in2.end());
            block_tab[j].out1.insert(block_tab[j].out1.end(), block_tab[j].out2.begin(), block_tab[j].out2.end());
            for (int k = 0; k < block_tab[j].def1.size(); k++) {
                if (block_tab[j].def1[k] || block_tab[j].in1[k]) {
                    for (int l = 0; l < block_tab[j].def1.size(); l++) {
                        if (k != l && (block_tab[j].def1[l] || block_tab[j].in1[l])) {
                            btab[i].conflict_graph[k][l] = btab[i].conflict_graph[l][k] = true;
                        }
                    }
                }
            }
        }


    }
}


void print_block()
{
    FILE *out = fopen("../blocks.txt", "w");
    for (int i = 1; i <= b; i++) {
        fprintf(out, "func_%d:\n", i);
        for (int j = btab[i].start_blk; j <= btab[i].end_blk; j++) {
            fprintf(out, "  block_%d:\n", j);
            for (int k = block_tab[j].start; k <= block_tab[j].end; k++) {
                print_midcode(out, k);
                if (j != btab[i].start_blk && block_tab[j].n_pre == 0) {
                    midcode[k].is_valid = false;
                }
            }

            fprintf(out, "  前驱: ");
            for (int k = 0; k < block_tab[j].n_pre; k++) {
                fprintf(out, "%d, ", block_tab[j].pres[k]);
            }
            fprintf(out, "\n");
            fprintf(out, "  后继: ");
            for (int k = 0; k < block_tab[j].n_follow; k++) {
                fprintf(out, "%d, ", block_tab[j].follows[k]);
            }
            fprintf(out, "\n");

            fprintf(out, "  def1: ");
            for (int k = 0; k < block_tab[j].def1.size(); k++) {
                fprintf(out, "%d, ", (int)block_tab[j].def1[k]);
            }
            fprintf(out, "\n");
            fprintf(out, "  use1: ");
            for (int k = 0; k < block_tab[j].use1.size(); k++) {
                fprintf(out, "%d, ", (int)block_tab[j].use1[k]);
            }
            fprintf(out, "\n");


            fprintf(out, "  in1: ");
            for (int k = 0; k < block_tab[j].in1.size(); k++) {
                fprintf(out, "%d, ", (int)block_tab[j].in1[k]);
            }
            fprintf(out, "\n");
            fprintf(out, "  out1: ");
            for (int k = 0; k < block_tab[j].out1.size(); k++) {
                fprintf(out, "%d, ", (int)block_tab[j].out1[k]);
            }
            fprintf(out, "\n");

            fprintf(out, "\n");
        }
        fprintf(out, "\n");
    }
    fclose(out);
}

void reg_alloc()
{
    int n_reg = 3 + 8 + 8;
    FILE *out = fopen("../regs.txt", "w");
    int reg_index = 0;

    for (int i = 1; i <= b; i++) {
        fprintf(out, "func_%d:\n", i);

        int n_localvar = btab[i].n_localvar;
        //int n_para = btab[i].psize / 4;
        int n_tmpvar = btab[i].n_tmpvar;
        int sum = n_tmpvar + n_localvar;

        if (btab[i].is_leaf) {
            n_reg = 8;
        }
        else {
            n_reg = 15;
        }

        vector<reg> regs;
        btab[i].regs.resize(n_localvar + n_tmpvar);
        vector<vector<bool>> graph;
        graph.resize(n_localvar + n_tmpvar);
        for (int j = 0; j < graph.size(); j++) {
            btab[i].regs[j].kind = 0;
            graph[j].insert(graph[j].end(), btab[i].conflict_graph[j].begin(), btab[i].conflict_graph[j].end());
        }

        bool flag = true;
    label1:
        while (flag && sum > 0) {
            flag = false;
            for (int j = 0; j < graph.size() && sum>0; j++) {
                if (btab[i].regs[j].kind == 0) {
                    int cnt = 0;
                    for (int k = 0; k < graph[j].size(); k++) {
                        if (graph[j][k]) {
                            cnt++;
                        }
                    }
                    //printf("cnt %d \n", cnt);
                    if (cnt < n_reg) {
                        sum--;
                        flag = true;
                        reg r;
                        r.kind = 1;
                        r.index = j;
                        regs.push_back(r);
                        btab[i].regs[j].kind = 1;
                        for (int k = 0; k < graph[j].size(); k++) {
                            graph[j][k] = graph[k][j] = false;
                        }
                    }
                }
            }
        }

        if (sum > 0) {
            for (int j = graph.size() - 1; j >= 0; j--) {
                int cnt = 0;
                for (int k = 0; k < graph[j].size(); k++) {
                    if (graph[j][k]) {
                        cnt++;
                    }
                }
                if (cnt >= n_reg) {
                    sum--;
                    btab[i].regs[j].kind = -1;
                    for (int k = 0; k < graph[j].size(); k++) {
                        graph[j][k] = graph[k][j] = false;
                    }
                    break;
                }
            }
            flag = true;
            goto label1;
        }

        for (int j = regs.size() - 1; j >= 0; j--) {
            int k = regs[j].index;
            set<int> set1;
            for (int l = 0; l < btab[i].regs.size(); l++) {
                if (btab[i].regs[l].kind == 2 && btab[i].conflict_graph[k][l]) {
                    set1.insert(btab[i].regs[l].num);
                }
            }
            if (!btab[i].is_leaf) {
                for (int l = 3; l < 19; l++) {
                    if (l == 4) {
                        continue;
                    }
                    if (!set1.count(l)) {
                        btab[i].regs[k].kind = 2;
                        btab[i].regs[k].num = l;
                        break;
                    }
                }
            }
            else {
                for (int l = 19; l < 29; l++) {
                    if (l == 24 || l == 25) {
                        continue;
                    }
                    if (!set1.count(l)) {
                        btab[i].regs[k].kind = 2;
                        btab[i].regs[k].num = l;
                        break;
                    }
                }
            }
        }

        for (int j = 0; j < btab[i].regs.size(); j++) {
            fprintf(out, "    var_%d kind: %d reg: $%d\n", j, btab[i].regs[j].kind, btab[i].regs[j].num);
        }
        fprintf(out, "\n\n");
    }

    fclose(out);
}


void code_opt()
{
    block_partition();
    build_flow_graph();
    data_flow_analysis();
    build_conflict_graph();
    print_block();
    reg_alloc();
    printf("中间代码优化及寄存器分配已完成。\n");
}