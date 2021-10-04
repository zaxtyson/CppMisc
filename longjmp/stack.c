#include <stdio.h>
#include <setjmp.h>

static jmp_buf global_env;

void func1(int *a);

int main(int argc, char const *argv[])
{
    int a = 100;
    int ret = setjmp(global_env);
    if (ret == 0)
    {
        printf("Normal flow, a=%d\n", a);
        func1(&a);
    }
    else
    {
        printf("Longjmp flow, a=%d\n", a);
    }
    return 0;
}

void func1(int *a)
{
    (*a)++;
    printf("func1, a = %d\n", *a);
    longjmp(global_env, 1);
    printf("func1 ended\n");
}

// 不开优化
// Normal flow, a=100
// func1, a = 101
// Longjmp flow, a=101

// 开了优化
// Normal flow, a=100
// func1, a = 101
// Longjmp flow, a=100