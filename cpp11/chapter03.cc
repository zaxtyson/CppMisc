#include <iostream>
#include <functional>
#include <memory>

using std::cout;
using std::endl;

// ----------------------------------------------------------------

void lambda_value_capture()
{
    int value = 100;
    auto func = [value] { return value; }; // lambda 创建时捕获, 拷贝一份
    value = 200;
    cout << "capture by value: " << func() << endl; // 100
}

void lambda_reference_capture()
{
    int value = 100;
    auto func = [&value] { return value; }; // lambda 引用捕获
    value = 200;
    cout << "capture by reference: " << func() << endl; // 200
}

void lambda_expression_capture() // C++14 表达式捕获, 允许捕获右值
{
    auto value = std::make_unique<int>(100);
    auto add = [v1 = 1, v2 = std::move(value)](int x) -> int { return v1 + (*v2) + x; };
    cout << "capture by expression: " << add(66) << endl; // 167
}

void lambda_generic()
{
    auto add = [](auto x, auto y) { return x + y; }; // C++14 泛型 lambda, 相当于函数的模板功能
    cout << add(std::string("Hello"), std::string(" World")) << endl;
    cout << add(100, 200) << endl;
}

void test_lambda()
{
    lambda_value_capture();
    lambda_reference_capture();
    lambda_expression_capture();
    lambda_generic();
}

// ----------------------------------------------------------------

using func_ptr1 = int (*)(int);            // 函数指针类型
using func_ptr2 = std::function<int(int)>; // std::function 是callable实体的类型安全的wrapper(函数的容器), 更加安全

void func_with_function_arg(func_ptr1 func)
{
    cout << "result 1: " << func(100) << endl;
}

void func_with_function_arg_safe(func_ptr2 func)
{
    cout << "result 2: " << func(100) << endl;
}

void test_functional()
{
    func_ptr1 foo = [](int x) -> int { return x + 100; }; // lambda表达式是闭包类型, **捕获列表为空时**, 可转换为函数指针
    func_with_function_arg(foo);

    int value = 6;
    func_ptr2 bar = [value](int x) -> int { return x + value; }; // 同时允许 lambda 捕获
    func_with_function_arg_safe(bar);
}

// ----------------------------------------------------------------

int func_add_use_3_arg(int x, int y, int z)
{
    return x + y + z;
}

void test_function_bind()
{
    std::function<int(int)> bind_add = std::bind(func_add_use_3_arg, std::placeholders::_1, 100, 200); // 绑定两个确定的参数, 产生一个单参函数
    // bind_add 类型复杂的时候使用 auto 自动推导来规避
    cout << bind_add(666) << endl;
}

// -------------------------------------------------------------

int main(int argc, char const *argv[])
{
    // test_lambda();
    // test_functional();
    test_function_bind();
}
