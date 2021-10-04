#pragma once
#include <iostream>

enum class ShapeType
{
    Circle,
    Triangle,
    Rectangle
};

class Shape
{
public:
    Shape(){};
    virtual ~Shape(){};
    virtual void draw() = 0;
};

class Circle : public Shape
{
public:
    Circle()
    {
        std::cout << "Circle()" << std::endl;
    };

    ~Circle()
    {
        std::cout << "~Circle()" << std::endl;
    };

    void draw() override
    {
        std::cout << "A Circle" << std::endl;
    }
};

class Triangle : public Shape
{
public:
    Triangle()
    {
        std::cout << "Triangle()" << std::endl;
    }
    ~Triangle()
    {
        std::cout << "~Triangle()" << std::endl;
    }
    void draw() override
    {
        std::cout << "A Triangle" << std::endl;
    }
};

class Rectangle : public Shape
{
public:
    Rectangle()
    {
        std::cout << "Rectangle()" << std::endl;
    }
    ~Rectangle()
    {
        std::cout << "~Rectangle()" << std::endl;
    }
    void draw() override
    {
        std::cout << "A Rectangle" << std::endl;
    }
};

Shape *make_shape(ShapeType type)
{
    switch (type)
    {
    case ShapeType::Circle:
        return new Circle();
    case ShapeType::Triangle:
        return new Triangle();
    case ShapeType::Rectangle:
        return new Rectangle();
    default:
        return nullptr;
    }
}
