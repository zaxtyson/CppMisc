#include <iostream>
#include <vector>

using std::cout;
using std::endl;

class Obj
{
public:
    Obj()
    {
        cout << "Obj()" << endl;
    }
    ~Obj()
    {
        cout << "~Obj()" << endl;
    }
    Obj(const Obj &obj)
    {
        cout << "Obj(const Obj&)" << endl;
    }
    Obj(Obj &&obj) noexcept // noexcept is important!!
    {
        cout << "Obj(Obj&&)" << endl;
    }
};

int main(int argc, char const *argv[])
{
    std::vector<Obj> v1;
    v1.reserve(2);
    // v1.push_back(Obj());  // move to emplace_back()
    v1.emplace_back(); // build in vector
    v1.emplace_back();
    // if Obj does not have a move ctor with noexcept
    // vector will use the copy ctor when expand
    v1.emplace_back();
    return 0;
}
