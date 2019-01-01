#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <vector>
#include "optimization.h"
#include "midcode.h"
#include "mips.h"
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

void print_block()
{
    FILE *out = fopen("../blocks.txt", "w");
    for (int i = 1; i <= b; i++) {
        fprintf(out, "func_%d:\n\n", i);
        for (int j = btab[i].start_blk; j <= btab[i].end_blk; j++) {
            fprintf(out, "  block_%d:\n", j);
            for (int k = block_tab[j].start; k <= block_tab[j].end; k++) {
                print_midcode(out, k);
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

            fprintf(out, "\n");
        }
        fprintf(out, "\n");
    }
}




void code_opt()
{
    block_partition();
    build_flow_graph();
    print_block();
}