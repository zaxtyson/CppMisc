#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <iostream>
#include <vector>
#include <map>

int main()
{
    std::string s = fmt::format("I'd rather be {1} than {0}.\n", "right", "happy");
    std::cout << s;
    using namespace std::literals::chrono_literals;
    fmt::print("Default format: {} {}\n", 42s, 100ms);
    fmt::print("strftime-like format: {:%H:%M:%S}\n", 3h + 15min + 30s);
    std::vector<int> v = {1, 2, 3};
    fmt::print("{}\n", v);
    std::vector<std::vector<int>> v2 = {
        {1, 2, 3},
        {4, 5, 6}};
    fmt::print("{}\n", v2);
    std::map<std::string, std::string> m = {
        {"name", "zhangsan"},
        {"age", "19"}};
    fmt::print("{}\n", m);
    m.erase("age");
    m["namex"] = "lisi";
    fmt::print("{}\n", m);
}