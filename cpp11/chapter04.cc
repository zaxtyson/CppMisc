#include <iostream>
#include <array>
#include <map>
#include <unordered_map>
#include <tuple>

using std::cout;
using std::endl;

// ----------------------------------------------------------------

void func_pass_array_c_style(int array[], int len)
{
    cout << "C style array, len: " << len << endl;
    for (int i = 0; i < len; i++)
        cout << array[i] << ' ';
    cout << endl;
}

template <typename BinIterator>
void func_pass_array_iterator_style(BinIterator begin, BinIterator end)
{
    cout << "Iterator style array" << endl;
    for (BinIterator it = begin; it != end; it++)
        cout << *it << ' ';
    cout << endl;
}

template <typename T, size_t N>
void func_pass_array_cxx_style(std::array<T, N> array)
{
    cout << "CXX style array, len: " << array.size() << endl;
    for (T &e : array)
        cout << e << " ";
    cout << endl;
}

void test_array()
{
    constexpr int len = 10;
    std::array<int, len> list = {12, 22, 3, 66, 58, 0, 111, 23};
    cout << "size:" << list.size() << ", list[4]=" << list[4] << endl;

    std::sort(list.begin(), list.end(), [](int x, int y) { return x < y; }); // 支持 iterator

    func_pass_array_c_style(list.data(), list.size());
    func_pass_array_iterator_style(list.begin(), list.end());
    func_pass_array_cxx_style(list);
}

// ----------------------------------------------------------------

void test_map()
{
    std::map<int, std::string> u = {
        {1, "1"},
        {2, "2"},
        {3, "3"}};

    std::unordered_map<int, std::string> v = {
        {1, "1"},
        {2, "2"},
        {3, "3"}};

    cout << "Map: " << u.size() << endl;
    for (auto &[key, value] : u)
        cout << "key: " << key << ", value: " << value << endl;
    cout << endl;

    cout << "Unordered Map: " << v.size() << endl;
    for (auto &[key, value] : v)
        cout << "key: " << key << ", value: " << value << endl;
    cout << endl;
}

// ----------------------------------------------------------------

void test_tuple()
{
    // 使用 std::get<N>, N 必须是编译器常量
    auto stu = std::make_tuple("Zhangsan", 18, 3.0);
    cout << "name: " << std::get<0>(stu) << ", age: " << std::get<1>(stu) << ",GPA: " << std::get<2>(stu) << endl;

    // 使用 std::tie
    std::string a;
    int b;
    double c;
    std::tie(a,b,c) = stu;  // 解包
    cout << "name: " << a << ", age: " << b << ",GPA: " << c << endl;


    // C++17 结构化绑定
    auto& [name, age, gpa] = stu;
    cout << "name: " << name << ", age: " << age << ",GPA: " << gpa << endl;

    // 通过引用可以修改 tuple 的值
    age = 66;
    cout << "age changed: " << std::get<int>(stu) << endl; // C++14 可以通过类型 get 值, 而非index, 前提是tuple中只有一个该类型的数据

    // 合并元组
    auto other = std::make_tuple("男", "团员", "汉族");
    auto stu2 = std::tuple_cat(std::move(stu), other);

    // 获取元组长度
    // size_t len = std::tuple_size<decltype(stu2)>::value;
    size_t len = std::tuple_size_v<decltype(stu2)>;
    cout << "tuple len: " << len << endl;

    // for(size_t i = 0; i < len; i++)
    //     cout << std::get<i>(stu2) << " ";  // 非法, i 不是编译器常量, 使用 C++17 的 variant
    // cout << endl;
}

int main(int argc, char const *argv[])
{
    // test_array();
    // test_map();
    test_tuple();
}
