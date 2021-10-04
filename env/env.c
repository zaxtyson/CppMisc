#include <stdlib.h>
#include <stdio.h>

static void set_env_str()
{
    char env[] = "mykey=myvalue"; // err!
    // char env = "mykey=myvalue";  // 可以， 全局的或者静态的没问题
    // static char env[] = "mykey=myvalue"; // 可以
    // 建议使用 int setenv(const char* name, const char* value, int overwrite)

    // if (setenv("mykey", "myvalue2", 1) != 0)
    if (putenv(env) != 0)
    {
        printf("failed!\n");
        return;
    }
    printf("now, mykey=%s\n", getenv("mykey"));
}

static void get_env_str()
{
    printf("\nmykey=%s\n", getenv("mykey"));
}

int main(int argc, char const *argv[])
{
    set_env_str(); // now, mykey=myvalue
    get_env_str(); // mykey=(null), 因为 env[] 内存已经释放
    return 0;
}
