#include <any>
#include <iostream>
#include <string>
#include <typeinfo>
#include <optional>

using std::cout;
using std::endl;
using std::optional;
using std::string;

struct Student
{
    string name;
    size_t id;
};

optional<Student> getStudent(size_t id)
{
    if (id > 0)
        return Student({string("name_%d", id), id});
    return std::nullopt;
}

optional<string> getInt(bool flag)
{
    if (flag)
        return string("Hello");
    return std::nullopt;
}

int main(int argc, char const *argv[])
{
    // std::any data = 1000;
    // if (data.type() == typeid(string))
    // {
    //     string str = std::any_cast<string>(data);
    //     cout << "string" << str << endl;
    // }
    // else if (data.type() == typeid(const char *))
    // {
    //     const char *str = std::any_cast<const char *>(data);
    //     cout << "const char" << str << endl;
    // }
    // else if (data.type() == typeid(int))
    // {
    //     int a = std::any_cast<int>(data);
    //     cout << "int" << a << endl;
    // }

    string str = getInt(false).value_or("no value");
    cout << str << endl;

    optional<string> opt = getInt(false);
    if (!opt)
        cout << "Not value" << endl;
    else
        cout << opt.value() << endl;

    return 0;
}
