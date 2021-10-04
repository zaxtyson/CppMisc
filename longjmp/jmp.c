#include <setjmp.h>
#include <stdio.h>

// setjmp longjmp 的实现平台相关
// glibc/sysdeps/i386/setjmp.S
// setjmp -> __sigsetjmp
// 保存 6 个寄存器: ebx esi edi esp pc ebp
// 注意， 函数局部变量的值并不会恢复

// longjmp 恢复寄存器内容， 把 val 放到 eax， 跳转到 pc 寄存器处(setjmp下一条指令)

static jmp_buf global_env;

void func1();
void func2();

int main(int argc, char const *argv[])
{
    int n = setjmp(global_env); // longjmp 会再次回到这里
    if (n == 0)                 // 直接调用返回 0, 跳转来的话， n 为指定值
    {
        printf("Normal flow\n");
        func1();
    }
    else
    {
        printf("Longjmp flow, n=%d\n", n);
    }
    printf("main ended\n");
    return 0;
}

void func1()
{
    printf("this is func1\n");
    func2();
    printf("func1 ended\n"); // 没有执行
}

void func2()
{
    printf("this is func2\n");
    longjmp(global_env, 666); // 跳转到 setjmp 处
    printf("func2 ended\n");  // 没有执行
}