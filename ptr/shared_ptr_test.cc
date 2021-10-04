#include <memory>
#include <iostream>
#include <map>
using namespace std;

class Obj
{
private:
    int i_;

public:
    Obj(int i) : i_(i)
    {
        cout << "Obj()" << endl;
    }

    Obj(const Obj& other) {
        cout << "Obj(const Obj&)" << endl;
        i_ = other.i_;

    };

    ~Obj()
    {
        cout << "~Obj()" << endl;
    }

    void say() const
    {
        cout << "OOK: " << i_ << endl;
    }
};

class Other
{
public:
    Other(const Obj &obj) : obj_(obj)
    {
        cout << "Other()" << endl;
    }

    ~Other()
    {
        cout << "~Other()" << endl;
    }

    void say() const
    {
        obj_.say();
    }

private:
    Obj obj_;
};

map<int, shared_ptr<Other>> mydata{};

void test02(shared_ptr<Other> ptr)
{
    cout << ptr.use_count() << endl;
}

void test01()
{
    Obj obj(10000);
    shared_ptr<Other> other = make_shared<Other>(obj);
    // test02(other);
    mydata[1] = other;
    // test02(other);
}

Other *test03()
{
    Obj obj(1111);
    return new Other(obj);
}

Obj make_obj(){
    Obj obj(2222);
    return move(obj);
}

int main(int argc, char const *argv[])
{
    test01();
    cout << "=====" << endl;
    mydata[1]->say();
    mydata.erase(2);
    cout << "=====" << endl;

    // Other *other = test03();
    // other->say();

    // Obj obj = make_obj();
    // obj.say();
    // return 0;
}
