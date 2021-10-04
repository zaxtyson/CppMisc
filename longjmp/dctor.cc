#include <iostream>
#include <csetjmp>
#include <memory>

using namespace std;

// 直接 longjmp 倒在局部对象未析构

static jmp_buf global_env;

class Object
{
public:
    Object()
    {
        cout << "Object()" << endl;
    }

    ~Object()
    {
        cout << "~Object()" << endl;
    }
};

void fun1();

int main(int argc, char const *argv[])
{
    int ret = setjmp(global_env);
    if (ret == 0)
    {
        cout << "Normal flow" << endl;
        fun1();
    }
    else
    {
        cout << "Longjmp flow" << endl;
    }
    return 0;
}

void fun1()
{
    // Object obj;  // 不会析构
    std::unique_ptr<Object> obj(new Object()); // 还是不会析构
    longjmp(global_env, 1);
    // obj dctor not be called
}

// Normal flow
// Object()
// Longjmp flow