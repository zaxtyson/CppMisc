#include <iostream>

using namespace std;

template <typename T>
struct TraitsHelper
{
    using arg_type = T;
    using ret_type = T;
};

template <>
struct TraitsHelper<int>
{
    using arg_type = int;
    using ret_type = double;
};

template <typename T>
class Test
{
public:
    TraitsHelper<T>::ret_type foo(TraitsHelper<T>::arg_type arg)
    {
        cout << "testing" << endl;
    };

private:
    T m_data;
};

// template <typename T>
// TraitsHelper<T>::ret_type Test<T>::foo(TraitsHelper<T>::arg_type arg)
// {
//     cout << "testing" << endl;
// }

int main(int argc, char const *argv[])
{
    Test<int> test;
    test.foo(111);
    return 0;
}
