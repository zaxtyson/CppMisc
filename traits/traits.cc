#include <iostream>

using namespace std;

// traits 类型推导， 迭代器， std 算法 distance
// 不同类型的迭代器， 算法实现的流程不同

template <typename T>
struct isPointer
{
    static const bool value = false;
};

template <typename T>
struct isPointer<T *>
{
    static const bool value = true;
};

int main(int argc, char const *argv[])
{
    int a = 100;
    if (isPointer<int *>::value)
    {
        cout << "yes" << endl;
    }
    else
    {
        cout << "no" << endl;
    }
    return 0;
}
