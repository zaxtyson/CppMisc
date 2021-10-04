#include <iostream>
#include "shape.hpp"

// 函数传递时避免使用unique_ptr作为函数参数，除非需要转移所有权到函数里面，
// 一般通过Get函数获取裸指针去传递
// https://zhuanlan.zhihu.com/p/101938827

// implementation of c++11 unique_ptr
template <typename T>
class unique_ptr
{
public:
    explicit unique_ptr(T *ptr = nullptr) : m_ptr(ptr) {}

    template <typename U>             // support subclass ptr cast to superclass
    unique_ptr(unique_ptr<U> &&other) // move semantics, it will auto disable copy constructor
    {
        m_ptr = other.release();
    }

    // unique_ptr &operator=(unique_ptr &rhs)
    // {
    //     unique_ptr(rhs).swap(*this); // for strong exception security
    //     return *this;
    // }

    unique_ptr &operator=(unique_ptr rhs) // we get tmp object rhs
    {
        rhs.swap(*this);
        return *this;
    }

    ~unique_ptr()
    {
        delete m_ptr;
    }

    T &operator*() const { return *m_ptr; }
    T *operator->() const { return m_ptr; }
    operator bool() const { return m_ptr; }

    T *release()
    {
        std::cout << "release()" << std::endl;
        T *ptr = m_ptr;
        m_ptr = nullptr;
        return ptr;
    }

    void swap(unique_ptr &rhs)
    {
        std::swap(m_ptr, rhs.m_ptr);
    }

private:
    T *m_ptr;
};

void test_smart_ptr()
{
    unique_ptr<Shape> obj{make_shape(ShapeType::Circle)};
    obj->draw();

    // unique_ptr<Shape> obj2{obj};  // error, not copy constructor
    unique_ptr<Shape> obj2{std::move(obj)}; // OK
    obj2->draw();

    if (!obj)
    {
        std::cout << "obj is null" << std::endl;
    }

    // up cast
    unique_ptr<Triangle> triangle{new Triangle()};

    unique_ptr<Shape> obj3 = std::move(triangle); // OK
    obj3->draw();
    // unique_ptr<Circle> obj4 = std::move(triangle); // Error
    // obj4->draw();
}

int main(int argc, char const *argv[])
{
    test_smart_ptr();
    return 0;
}
