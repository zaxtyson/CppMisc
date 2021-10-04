#include <iostream>

using std::cout;
using std::endl;

class Demo
{
private:
    int a, b, c;

public:
    Demo() : a(1), b(2), c(3)
    {
        cout << "constructor" << endl;
    }
    virtual ~Demo()
    {
        cout << "destructor" << endl;
    }
};

void operator delete(void *p)
{
    cout << "delete address " << p << endl;
    free(p);
}

void operator delete[](void *p)
{
    cout << "delete[] address " << p << endl;
    free(p);
}

template <typename T>
void array_delete(T *p)
{
    if (!p)
        return;
    size_t *ps = reinterpret_cast<size_t *>(p) - 1;
    cout << "start of array: " << ps << endl;
    size_t size = ps[0];
    cout << "array size: " << size << endl;
    T *cur = p + size;
    using Func = void (*)();
    while (cur != p)
    {
        cur--;
        void *vptr = reinterpret_cast<void **>(cur)[0];
        Func dtor = reinterpret_cast<Func *>(vptr)[0];
        dtor();
    }
    ::operator delete[](ps);
}

int main()
{
    Demo *p = new Demo[5];
    cout << p << endl;
    // delete[] p;
    array_delete(p);
    return 0;
}