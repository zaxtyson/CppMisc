#include <stdio.h>

int main(int argc, char const *argv[])
{
    setbuf(stdout, NULL);
    const char str[] = "hello world";
    FILE *fp = fopen("./tmp.txt", "w");
    size_t n = fwrite(str, sizeof(str), 1, fp); // n=1
    // size_t n = fwrite(str, 1, sizeof(str), fp); // n=12
    printf("write %d members\n", n);
    fclose(fp);
    return 0;
}
