#include <iostream>
#include <string>

using namespace std;

template <typename T>
using ptr = T *;

class Student
{
private:
    string name = "test"; // 就地声明
    int age = 18;

public:
    Student(){};
    Student(const string &name_) : name(name_){};        // 初始化列表效率高
    Student(const string &name, int age) : Student(name) // 委托构造,不能同时使用初始化列表
    {
        this->age = age;
    }
    friend ostream &operator<<(ostream &os, const Student s)
    {
        os << "Student{name=" << s.name << ",age=" << s.age << "}";
        return os;
    }
};

// 自定义字面量 string literal operator
// 不能用 friend 写到类里面, "" 后面有空格, _开头
Student operator"" _Stu(const char *name, size_t len)
{
    Student s(name);
    return s;
}

size_t operator"" _len(const char *, size_t len)
{
    return len;
}

int main(int argc, char const *argv[])
{
    atexit([] { cout << "Exiting." << endl; });
    at_quick_exit([] { cout << "quick exit!" << endl; });

    Student s1;
    ptr<Student> s2 = new Student("zhangsan", 22);
    ptr<Student> s3 = new Student("lisi");
    cout << s1 << ' ' << *s2 << ' ' << *s3 << endl;
    Student s4 = "李华"_Stu;
    cout << s4 << endl;
    cout << "我的老天爷"_len << endl;
    quick_exit(0);
    return 0;
}
