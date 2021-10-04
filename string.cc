#include <iostream>
#include <cassert>
#include <string>
#include <regex>
using std::cout;
using std::endl;
using std::string;
using namespace std::literals::string_literals;

// using string = std::basic_string<char>;
// using wstring = std::basic_string<wchar_t>;
// using u16string = std::basic_string<char16_t>;
// using u32string = std::basic_string<char32_t>;

int main(int argc, char const *argv[])
{
    auto str = "this is std string, not c string"s;

    auto str2 = R"xx(raw string\n)xx"; // R"[0,16](xxx)[0,16]"
    cout << str2 << endl;

    assert(std::stoi("123") == 123);
    assert(std::stol("999") == 999L);
    assert(std::stod("12.35") == 12.35);
    assert(std::to_string(1234) == "1234");

    auto str_new = std::regex_replace(str, std::regex(R"(is(.+?),)"), "xxxx");
    cout << str_new << endl;
    return 0;
}
