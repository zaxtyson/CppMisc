#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char *p = (char *)malloc(10);
    memset(p, 1, 10);
    printf("addr=%p, size=%d\n", p, strlen(p));

    // 尽量别用 realloc
    // realloc(NULL, size) = malloc(size);
    // realloc(ptr, 0) = free(ptr)
    // 如果要用， 必须先检查错误情况
    char *new_p = (char *)realloc(p, sizeof(100));
    if (!new_p)
    {
        printf("err!\n");
        return -1;
    }
    p = new_p; // 然后才能覆盖原来的指针， 防止内存泄露
    memset(p, 1, 100);
    printf("addr=%p, size=%d\n", p, strlen(p));

    // 关于内存拷贝
    char src[] = "hello";
    char dest[100] = {0};
    memcpy(dest, src, sizeof(dest));

    // 注意 '\0'
    strncat(dest, src, sizeof(dest) - strlen(dest) - 1);
    strncpy(dest, src, sizeof(dest) - 1);
    snprintf(dest, sizeof(dest), "%s-%s", src, "world"); // '\0'已经计算在内

    // 输入
    char input[100] = {0};
    // 不要不要不要使用 gets 它不检查缓冲区
    fgets(input, sizeof(input), stdin); // 最多读取 sizeof(input) -1, 自动添加 '\0'
    return 0;
}
