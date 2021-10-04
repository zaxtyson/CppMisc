#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void mem_leak1()
{
    char *p = (char *)malloc(100);
}

static void mem_leak2()
{
    FILE *fp = fopen("leak.c", "r");
}

static void mem_leak3()
{
    char *p = (char *)malloc(1);
    *(int *)p = 100;
    free(p);
}

static void mem_leak4()
{
    char array[5];
    strcpy(array, "Hello");
}

static void mem_leak5()
{
    char *p = (char *)malloc(10);
    free(p);
    p = NULL;
    free(p);
}

static void mem_leak6()
{
    char *p;
    free(p);
}

int main(int argc, char const *argv[])
{
    mem_leak1();
    mem_leak2();
    mem_leak3();
    mem_leak4();
    mem_leak5();
    mem_leak6();
    return 0;
}
