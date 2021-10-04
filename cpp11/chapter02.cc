#include <iostream>
#include <vector>
#include <tuple>
#include <map>

using std::cout;
using std::endl;

void test_NULL()
{
    if (std::is_same<decltype(NULL), decltype(0)>::value)
        cout << "NULL == 0" << endl;

    if (std::is_same<decltype(NULL), decltype(nullptr)>::value)
        cout << "NULL == nullptr" << endl;

    if (std::is_same<decltype(NULL), decltype((void *)0)>::value)
        cout << "NULL == (void*)0" << endl;
}

// ----------------------------------------------------------------

constexpr int pow3(int x)
{
    return x * x * x;
}

int pow2(int x)
{
    return x * x;
}

void test_constexpr()
{
    int array[pow3(3) + 1] = {1, 2, 3}; // thats ok
    for (int i : array)
        cout << i << " ";
    cout << endl;

    // invalid, but still works
    int len = 10;
    int array2[len] = {1};
    for (int i : array2)
        cout << i << " ";
    cout << endl;

    // invalid, but still works
    int array3[pow2(2)] = {1, 1, 1, 1};
    for (int i : array3)
        cout << i << " ";
    cout << endl;
}

// ----------------------------------------------------------------

void test_enhanced_if_switch()
{
    std::vector<int> array = {1, 2, 3, 4};
    if (auto it = std::find(array.begin(), array.end(), 2); it != array.end()) // C++17
        *it = 22;

    for (int i : array)
        cout << i << " ";
    cout << endl;
}

// ----------------------------------------------------------------
class Foo1
{
public:
    std::vector<int> data;

public:
    Foo1(std::initializer_list<int> list) // C++ 11
    {
        for (auto i : list)
            data.push_back(i);
    }

    void addMoreData(std::initializer_list<int> list) // can be use as arguments
    {
        for (auto i : list)
            data.push_back(i);
    }

    void showData()
    {
        for (auto i : data)
            cout << i << " ";
        cout << endl;
    }
};

void test_initalizer_list()
{
    Foo1 f1({1, 2, 3, 4});
    f1.showData();
    f1.addMoreData({8, 9, 10});
    f1.showData();

    Foo1 f2{100, 200, 300, 400}; // C++11 数组 POD 类对象统一初始化语法
    f2.showData();
}

// ----------------------------------------------------------------

std::tuple<std::string, int> func1() // C++17
{
    std::string name = "Zhangsan";
    return std::make_tuple(name, 18);
}

void test_tuple()
{
    auto [name, age] = func1();
    cout << name << " " << age << endl;
}

// ----------------------------------------------------------------

// template <typename T, typename U>
// decltype(x+y) func_add_bad(T x, U y)  // 一开始是不知道x,y类型的
// {
//     return x+ y;
// }

template <typename T, typename U>
auto func_add_11(T x, U y) -> decltype(x + y) // C++11 尾返回类型(trailing return type)
{
    return x + y;
}

template <typename T, typename U>
auto func_add_14(T x, U y) // C++14, 自动推导返回类型
{
    return x + y;
}

void test_auto_decltype()
{
    auto i = 100;                                                // int
    cout << std::is_same<decltype(i), int>::value << endl;       // 1
    auto array = new auto(10);                                   // int*
    cout << std::is_same<decltype(array), int *>::value << endl; // 1
    // void foo(auto x, auto y){}  // 函数参数推断应该使用 template
    // auto array2[10] = {1, 2, 3};  // 也不能推断数组类型

    auto w11 = func_add_11(1, 2);
    std::cout << std::is_same<decltype(w11), int>::value << std::endl; // 1
    auto w14 = func_add_14(1, "Hello");
    std::cout << std::is_same<decltype(w14), const char *>::value << std::endl; // 1
}

// ----------------------------------------------------------------

template <typename T>
auto func_add_by_condition(T x) // 如果参数类型是整形, +100, 否则 -100
{
    if constexpr (std::is_integral<T>::value)
        return x + 100;
    else
        return x - 100;
}

void test_if_constexpr()
{
    long x = 1024;
    cout << func_add_by_condition(x) << endl; // 1124
}

// -------------------------------------------------------------

template <typename T = int, typename U = double> // C++11 默认模板参数
auto func_add_with_default_arg_type(T x, U y)
{
    return x + y;
}

template <typename R, typename... Args> // C++17 变参模板展开
void func_print(R x, Args... args)
{
    cout << "x is: [" << x << "], args num: " << sizeof...(args) << endl;
    if constexpr (sizeof...(args) > 0)
        func_print(args...);
}

// C++17 还有一个黑魔法, 初始化列表展开

template <typename... T> // C++17 折叠表达式
auto func_sum_flod_expr(T... x)
{
    return (x + ...);
}

template <int buffSize> // 非类型模板参数推导, 将字面量作为模板参数传递
void func_no_type_template()
{
    cout << "template arg: " << buffSize << endl;
    int array[buffSize] = {1, 2, 3};
}

template <auto someInfo> // C++17 自动字面量参数类型, 功能有限
void func_auto_no_type_template()
{
    cout << "auto template arg: " << someInfo.second << endl;
}

void test_templates()
{
    cout << func_add_with_default_arg_type(1, 12.3) << endl;
    cout << func_add_with_default_arg_type<int, int>(1, 12) << endl;
    func_print("Hello", "--", "World");
    cout << "sum: " << func_sum_flod_expr(1, 2, 3, 4, 5, 6) << endl;
    func_no_type_template<6666>();
    func_auto_no_type_template<std::make_pair(111, 222)>();
}

// ----------------------------------------------------------------
class BaseClass
{
public:
    int value1;
    int value2;

public:
    BaseClass()
    {
        value1 = 100;
    }
    BaseClass(int v2) : BaseClass() // C++11 委托构造
    {
        value2 = v2;
    }
};

class SubClass : public BaseClass
{
public:
    using BaseClass::BaseClass; // C++11 继承构造
};

void test_constructor_function()
{
    BaseClass bc(200);
    cout << bc.value1 << " " << bc.value2 << endl;
    SubClass sc(400);
    cout << sc.value1 << " " << sc.value2 << endl;
}

// ----------------------------------------------------------------

enum class Color : unsigned int
{
    RED,
    BLUE,
    YELLOW,
    GREEN
};

void test_strong_enum_class()
{
    Color c1 = Color::RED;
    // cout << c1 << endl; // error
    cout << static_cast<unsigned int>(c1) << endl;
}

// ----------------------------------------------------------------

void test_struct_bind()
{
    std::map<std::string, int> info = {
        {"A", 100},
        {"B", 200},
        {"C", 300}};

    for (auto&& [name, value] : info)
        if (name.compare("A") == 0)
            value = -value;

    for (auto &&[name, value] : info)
        cout << name << ":" << value << endl;
}

int main(int argc, char const *argv[])
{
    // test_NULL();
    // test_constexpr();
    // test_enhanced_if_switch();
    // test_initalizer_list();
    // test_tuple();
    // test_auto_decltype();
    // test_if_constexpr();
    // test_templates();
    // test_constructor_function();
    // test_strong_enum_class();
    test_struct_bind();
}
