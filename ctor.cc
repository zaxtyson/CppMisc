#include <iostream>

using namespace std;

class A
{
public:
    A() = delete;

    explicit A(int a) : _a(a)
    {
        cout << "ctor with: " << a << endl;
    }

    A(const A &other)
    {
        _a = other._a;
        cout << "copy ctor" << endl;
    }

    A(const A &&other)
    {
        _a = move(other._a);
        cout << "move ctor" << endl;
    }

    void show() const
    {
        cout << "value: " << _a << endl;
    }

private:
    int _a;
};

A make_a()
{
    A tmp(10);
    cout << "make A(10)" << endl;
    return tmp;
}

int main(int argc, char const *argv[])
{
    A a1(20);
    A a2(a1);
    a2.show();
    A a3(move(a2));
    a2.show();
    return 0;
}
