#ifndef D3CE0DA5_5E4D_4E40_83CF_938F799F84EE
#define D3CE0DA5_5E4D_4E40_83CF_938F799F84EE
#include <cstring>
#include <iostream>

template <typename T>
int compare(const T a, const T b)
{
    if (a > b)
        return 1;
    else if (a < b)
        return -1;
    else
        return 0;
}

template <>
int compare(const char *str1, const char *str2)
{
    return strcmp(str1, str2);
}

template <typename T>
class Foo
{
public:
    Foo() = default;
    void bar(T arg);
};

template <typename T>
void Foo<T>::bar(T arg)
{
    std::cout << arg << std::endl;
}

template <>
void Foo<const char *>::bar(const char *str)
{
    std::cout << str << std::endl;
}

#endif /* D3CE0DA5_5E4D_4E40_83CF_938F799F84EE */
