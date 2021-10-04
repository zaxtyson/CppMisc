#include <iostream>
#include <functional>

using namespace std;

template <typename Callable, typename... Args>
auto decorate(Callable &&func, Args &&...args)
{
    cout << "start" << endl;
    auto ret = func(args...);
    cout << "end" << endl;
    return ret;
}

// decorate

template <typename>
struct Decorator;

template <typename Ret, typename... Args>
struct Decorator<Ret(Args...)>
{
    std::function<Ret(Args...)> m_func;

    Decorator(std::function<Ret(Args...)> func) : m_func(func) {}

    Ret operator()(Args... args)
    {
        cout << "start" << endl;
        return m_func(args...);
    }
};

template <typename Ret, typename... Args>
Decorator<Ret(Args...)> make_decorator(Ret (*func)(Args...))
{
    return std::function<Ret(Args...)>(func);
};

int test(int a, int b)
{
    return a + b + 100;
}

int main(int argc, char const *argv[])
{
    auto f = [](int a, int b)
    { return a * b; };

    decorate(f, 10, 20);

    auto f2 = make_decorator(test);
    cout << f2(10, 20) << endl;
    return 0;
}
