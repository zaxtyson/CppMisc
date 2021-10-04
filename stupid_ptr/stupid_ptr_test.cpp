#include <iostream>
#include <cstring>
#include "stupid_ptr.h"
using namespace std;

enum class ShapeType
{
    Circle,
    Triangle,
    Square
};


class Shape
{
public:
    Shape() { cout << "shape" << endl; }
    virtual void speak() { cout << "I am shape" << endl; }
    virtual ~Shape(){cout<<"shape destroyed"<<endl;};
};

class Circle : public Shape
{
public:
    Circle() { cout << "circle" << endl; }
    void speak() override { cout << "I am circle" << endl; }
};

class Triangle : public Shape
{
public:
    Triangle() { cout << "triangle" << endl; }
    virtual void speak() override { cout << "I am triangle" << endl; }
};

class Square : public Shape
{
public:
    Square() { cout << "square" << endl; }
    virtual void speak() override { cout << "I am square" << endl; }
};

Shape *createShape(ShapeType type)
{
    switch (type)
    {
    case ShapeType::Circle:
        return new Circle();
    case ShapeType::Square:
        return new Square();
    case ShapeType::Triangle:
        return new Triangle();
    default:
        return nullptr;
    }
}

class ShapeWapper{
    public:
    ShapeWapper(Shape* shape):_shape(shape){}
    ~ShapeWapper(){delete _shape;}
    Shape* Get() const {return _shape;}
    private:
    Shape* _shape;
};


int main()
{
    stupid_ptr<Shape> sp(createShape(ShapeType::Circle));
    sp->speak();
    stupid_ptr<Shape> sp2 = sp;
    sp2->speak();

    char *str = (char *)malloc(7);
    strcpy(str, "toptal");
    printf("char array = \"%s\" @ %u\n", str, str);

    str = (char *)realloc(str, 1111);
    strcat(str, ".com");
    printf("char array = \"%s\" @ %u\n", str, str);

    free(str);
    cout << endl;
    char* str2 = static_cast<char*>(malloc(7));
    str2 = static_cast<char*>(realloc(str2, 100));
    strcpy(str2, "abbba\naaaaaaaaaaa");
    printf("%s", str2);
}