#include <iostream>
#include <memory>

using std::cout;
using std::endl;

class Resource
{
public:
    int data = 100;

public:
    Resource() { cout << "Resource constructor called" << endl; }
    ~Resource() { cout << "Resource destructor called" << endl; }
};

using sPtr = std::shared_ptr<Resource>;
using uPtr = std::unique_ptr<Resource>;

void func_modify_resource(sPtr p)
{
    p->data = 200;
}

void test_shared_ptr()
{
    sPtr p1 = std::make_shared<Resource>();

    {
        sPtr p2 = p1;                   // 引用计数 +1
        cout << p2.use_count() << endl; // 2
    }

    cout << p1.use_count() << endl; // 1
    func_modify_resource(p1);
    cout << p1->data << endl;
}

void test_unique_ptr()
{
    uPtr p1 = std::make_unique<Resource>();
    // uPtr p2 = p1; // error, unique_ptr(const unique_ptr&) = delete;
    uPtr p2 = std::move(p1); // ok
    // cout << p1->data << endl; // error, segmentation fault
    if (!p1)
        cout << "p1 is null" << endl;
    cout << p2->data << endl;

    Resource *rp = p2.release(); // get raw pointer
    if (!p2)
        cout << "p2 is null" << endl;
    cout << rp->data << endl;

    // uPtr p3;
    // p3.reset(rp);
    uPtr p3(rp); // make_unique use raw pointer
} // destructor called by p3

// ----------------------------------------------------------------

class A;
class B;

class A
{
public:
    std::string name = "Class A";
    std::shared_ptr<B> ptr;

public:
    A() { cout << "A()" << endl; }
    ~A() { cout << "~A()" << endl; }
};

class B
{
public:
    std::string name = "Class B";
    // std::shared_ptr<A> ptr; // 如果双方都是 shared_ptr, 将导致循环引用, 引用计数无法归零, 双方都不能自动析构
    std::weak_ptr<A> ptr; // weak_ptr 作为资源观测者, 不能直接使用资源, 创建时不增加引用计数

public:
    B() { cout << "B()" << endl; }
    ~B() { cout << "~B()" << endl; }
};

void test_weak_ptr()
{
    std::shared_ptr<A> pa = std::make_shared<A>();
    std::shared_ptr<B> pb = std::make_shared<B>();
    pa->ptr = pb;  // 使之相互引用
    pb->ptr = pa;

    std::weak_ptr<A> wkptr_a = pb->ptr;  // 指向 pa 的弱引用
    // cout <<wkptr_a->name << endl; // error, 不能通过 weak_ptr 直接操作对象
    std::shared_ptr<A> sptr_a(wkptr_a); // weak_ptr 转交给 shared_ptr, 获得资源的临时访问权, 直到 sptr_a 对象销毁
    cout << sptr_a->name << endl;
}

int main(int argc, char const *argv[])
{
    // test_shared_ptr();
    // test_unique_ptr();
    test_weak_ptr();
}
