#ifndef _TABS_H_
#define _TABS_H_
#include <string>
#include <map>
#include <vector>

using namespace std;

typedef enum symbol {
    intcon, charcon, stringcon, //����  �ַ�  �ַ���
    _plus, _minus, times, idiv, //+  -  *  /
    eql, neq, gtr, geq, lss, leq, //==  !=  >  >=  <  <=
    lparent, rparent, lbracket, rbracket, lbrace, rbrace, //( ) [ ] { } 
    comma, semicolon, colon, becomes, //, ; : =  
    ident, mainsy, scanfsy, printfsy,
    constsy, returnsy, intsy, charsy, voidsy,
    ifsy, switchsy, casesy, defaultsy, whilesy, eofsy
} symbol;

typedef enum objtyp {
    constant, variable, function
} objtyp;

typedef enum types {
    notyp, ints, chars, arrints, arrchars, voids
} types;

typedef struct tabitem {//���ű�
    string name;
    int link; //ָ��ͬһ��������һ����ʶ����tab���е�λ��
    objtyp obj; // ��ʶ�����ࣺ������������������
    types typ; // ��ʶ�����ͣ�int�͡�char�͡�int�����͡�char�����͡�void�͵�
    int lev; // ��ʶ�����ڵľ�̬��Σ�ȫ�ֳ�������������Ϊ0���ֲ���������������0
    int ref; //��������ָ������btab���е�ָ��ֵ  
    int arrcnt; //����Ԫ�ظ���
    int adr; // �������βΣ�������ջs�е���Ե�ַ������������ӦĿ��������ڵ�ַ�����ͳ�������Ӧ��ֵ��
} tabitem;

typedef struct btabitem {
    int last, lastpar, psize, vsize;
    /*
      lastָ�������һ����ʶ����tab���е�λ�ã�
      lastparָ���������һ��������tab���е�λ�ã�
      psizeΪ������������Ϣ����ռ�Ĵ洢��Ԫ����
      vsize�ֲ�������������������������Ϣ����ռ�Ĵ洢��Ԫ������
    */
} btabitem;

extern map<string, symbol> ksy; //�����ֱ�
extern map<char, symbol> sps; //�����ַ���

extern vector<tabitem> tab; //���ű�
extern int t;

extern vector<btabitem> btab; //�ֳ����
extern int b;

extern vector<string> stab; //�ַ���������
extern int sx; //�ַ�������������

#endif // !_TABS_H_