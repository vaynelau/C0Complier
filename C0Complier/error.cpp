#include <cstdio>
#include "error.h"
#include "lexical_analysis.h"

using namespace std;

bool noerror = true;

void error(int n)
{
    noerror = false;
    printf("%s:%d:%d: error: ", src_file_name.c_str(), lcnt, chcnt);
    switch (n) {
    case 1:
        printf("不合法的字符常量'%c'\n", ch);
        break;
    case 2:
        puts("缺少单引号终结符");
        break;
    case 3:
        puts("缺少双引号终结符");
        break;
    case 4:
        puts("不合法的符号'!'");
        break;
    case 5:
        printf("无法识别的符号'%c'\n", ch);
        nextch();
        break;
    case 6:
        puts("缺少标识符");
        break;
    case 7:
        puts("缺少赋值符号");
        break;
    case 8:
        puts("缺少整数或字符常量");
        break;
    case 9:
        puts("缺少类型标识符int或char");
        break;
    case 10:
        puts("数组元素个数不大于0");
        break;
    case 11:
        puts("没有指定数组元素个数");
        break;
    case 12:
        puts("缺少右中括号");
        break;
    case 13:
        puts("缺少分号");
        break;
    case 14:
        puts("缺少右小括号");
        break;
    case 15:
        puts("缺少左大括号");
        break;
    case 16:
        puts("缺少右大括号");
        break;
    case 17:
        puts("不应出现的常量标识符");
        break;
    case 18:
        puts("未定义的标识符");
        break;
    case 19:
        puts("标识符出现重定义");
        break;
    case 20:
        puts("不合法的语句");
        break;
    case 21:
        puts("缺少冒号");
        break;
    case 22:
        puts("缺少左中括号");
        break;
    case 23:
        puts("函数无返回值");
        break;
    case 24:
        puts("不合法的表达式（因子）");
        break;
    case 25:
        puts("返回值类型与函数不一致");
        break;
    case 26:
        puts("缺少左小括号");
        break;
    case 27:
        puts("关系运算符左右类型不一致");
        break;
    case 28:
        puts("表达式的值不为整数");
        break;
    case 29:
        puts("case语句的常量类型与switch表达式的值不一致");
        break;
    case 30:
        puts("缺少case语句");
        break;
    case 31:
        puts("数组下标索引不为整型");
        break;
    case 32:
        puts("赋值语句左右类型不一致");
        break;
    case 33:
        puts("标识符类型不为变量");
        break;
    case 34:
        puts("实参个数过多");
        break;
    case 35:
        puts("实参与形参类型不一致");
        break;
    case 36:
        puts("实参个数过少");
        break;
    case 37:
        puts("程序结构错误");
        break;
    case 38:
        puts("没有合法的main函数");
        break;
    case 39:
        puts("函数缺少返回语句");
        break;
    case 40:
        puts("case语句的常量出现重复");
        break;
    default:
        puts("其他错误");
        break;
    }
}