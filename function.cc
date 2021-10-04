#include <iostream>
#include <string>
#include <locale>
#include <functional>
#include <fcntl.h>
using namespace std;
using namespace std::placeholders;

namespace space
{
    template <typename T>
    using ptr = T *;
}

int add(int a, int b)
{
    return a + b;
}

typedef int (*ADD)(int, int);

using add_func = int (*)(int, int);

void test_ptr(int a)
{
    cout << "int a" << endl;
}

void test_ptr(int *ptr)
{
    cout << "int* ptr" << endl;
}

class MyFuncClass
{
    int sum = 0;

public:
    int add(int a)
    {
        sum += a;
        return sum;
    }

    int add2(int a, int b)
    {
        return a + b;
    }
};

class MyOpClass
{
private:
    int value;

public:
    explicit MyOpClass(int x) : value(x) {}
    operator double()
    {
        return static_cast<double>(value);
    }
};

int main(int argc, char const *argv[])
{
    // ADD add1 = add;
    // add_func add2 = add;
    // cout << add1(1, 40) << endl;
    // cout << add2(1, 33) << endl;
    // space::ptr<int>myint = new int(100);
    // cout << *myint << "\t" << myint;
    // setlocale(LC_ALL, "chs");

    // cout << __cplusplus << endl;
    // cout << __FILE__ << endl;
    // cout << __FUNCTION__ << endl;
    // cout << __TIME__ << endl;
    // cout << __LINE__ << endl;

    // int a = 100;
    // static_assert(sizeof(a) <1, "失败");
    // const wchar_t* p1 = L"hhhhh";
    // const wchar_t* p2 = L"你好";
    // wcout << p1 << endl;
    // wcout << p2 << endl;

    // int* ptr = NULL;
    // test_ptr(ptr);
    // // test_ptr(NULL);
    // test_ptr(nullptr);

    // int a = 100;
    // int* b = &a;
    // auto c = b;
    // cout << typeid(a).name() << endl;
    // cout << typeid(b).name() << endl;
    // cout << typeid(c).name() << endl;

    // MyFuncClass s;

    // auto func = bind(&MyFuncClass::add, &s, _1);
    // auto func2 = bind(&MyFuncClass::add2, &s, _1, 100);
    // cout << func(2) << endl;
    // cout << func(4) << endl;
    // cout << func2(1) << endl;

    // MyOpClass op(1);
    // double x = 5 + op;
    // cout << x << endl;
    // MyOpClass op2(x);
    // cout << op2 << endl;

    // int a = 0;
    // auto func = [=]() mutable -> int {return ++a;};
    // cout << func() << endl;
    // double x = 1, y = 2, z = 3;
    // z = z / (double)(x / y);
    // cout << z << endl;
}
