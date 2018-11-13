const@ int _ = 1;
const int __2 = 2, __3 = 003;
const char add = '@', sub = '-', mul = '*', _div = '/';
const int MAX_FIBO = 100;

int n, m;
int fibo[100], num[100];
char str[1000];

void init_fibo()
{
    int i;
    i = 0;
    while (i < MAX_FIBO) {
        fibo[i] = 0;
        i = i + 1;
    }
}

int fibonacci(int n)
{
    switch (n) {
        case 0: return (0);
        case 1: return (1);
        default: {
            if (fibo[n] == 0)
                fibo[n] = fibonacci(n - 1) + fibonacci(n - 2);
            return (fibo[n]);
        }
    }
}

int factorial(int n)
{
    if (n == 0)
        return (1);
    return (n * factorial(n - 1));
}



void swap(int i, int j)
{
    int tmp;
    tmp = num[i];
    num[i] = num[j];
    num[j] = tmp;
    return;
}

void sort(int left, int right)
{
    if (left >= right) {
        return;
    }
    int i, j;
    i = left;
    j = left + 1;
    while (j <= right) {
        if (num[j] < num[left]) {
            i = i + 1;
            swap(i, j);
        }
        j = j + 1;
    }
    swap(left, i);
    sort(left, i - 1);
    sort(i + 1, right);
}

int calculate(int a, int b, char op)
{
    switch (op) {
        case '+': return (a + b);
        case '-': return (a - b);
        case '*': return (a * b);
        case '/': return (a / b);
    }
    return (0);
}

void strrev(int i, int j)
{
    char c;
    while (i < j) {
        c = str[i];
        str[i] = str[j];
        str[j] = c;

        i = i + 1;
        j = j - 1;
    }
}

void main()
{
    int result;
    int a, b;
    int i, tmp;

    printf("test start:");
    printf("test calculate:");
    a = -123 * 456 + -987 / +321 * (2 - 1);
    b = (-a + -0 * 156) / 123;
    printf("a: ", a);
    printf("b: ", b);
    result = calculate(a, b, add);
    printf("a b add: ", result);
    result = calculate(a, b, sub);
    printf("a b sub: ", result);
    result = calculate(a, b, mul);
    printf("a b mul: ", result);
    result = calculate(a, b, _div);
    printf("a b div: ", result);

    printf("test strrev:");
    str[0] = 'H';
    str[1] = 'e';
    str[2] = 'l';
    str[3] = 'l';
    str[4] = 'o';
    str[5] = '_';
    str[6] = '1';
    str[7] = '2';
    str[8] = '3';
    strrev(0, 8);
    i = 0;
    while (i < 9) {
        printf(str[i]);
        i = i + 1;
    }

    printf("Please input n, m:");
    scanf(n, m);
    printf("test factorial n: ", factorial(n));

    init_fibo();
    printf("test fibonacci m: ", fibonacci(m));


    printf("Please input 10 numbers:");
    i = 0;
    while (i < 10) {
        scanf(tmp);
        num[i] = tmp;
        i = i + 1;
    }
    sort(0, 9);
    printf("test sort:");
    i = 0;
    while (i < 10) {
        printf(num[i]);
        i = i + 1;
    };

    printf("test\finished.");

    return;
}