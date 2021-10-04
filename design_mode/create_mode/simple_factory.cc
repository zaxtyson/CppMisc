#include <iostream>
#include <map>

using std::string;

enum class ColorType
{
    Red,
    Green
};

class Color
{
public:
    Color()
    {
        std::cout << "Color()" << std::endl;
    }
    virtual ~Color()
    {
        std::cout << "~Color()" << std::endl;
    };
    virtual void display() const = 0;
};

class Red : public Color
{
public:
    Red()
    {
        std::cout << "Red()" << std::endl;
    }

    ~Red()
    {
        std::cout << "~Red()" << std::endl;
    }

    void display() const
    {
        std::cout << "I'am Red" << std::endl;
    }
};

class Green : public Color
{
public:
    Green()
    {
        std::cout << "Green()" << std::endl;
    }
    ~Green()
    {
        std::cout << "~Green()" << std::endl;
    }

    void display() const
    {
        std::cout << "I'am Green" << std::endl;
    }
};

class ColorFactory
{
public:
    static Color *create(ColorType type)
    {
        switch (type)
        {
        case ColorType::Red:
            return new Red();
        case ColorType::Green:
            return new Green();
        default:
            return nullptr;
        }
    }
};

// class ColorFactoryMap
// {
// public:
//     static Color *create(ColorType type)
//     {
//         return cached_colors[type];
//     }

// private:
//     // since c++17
//     static inline std::map<ColorType, Color *> cached_colors{
//         {ColorType::Red, new Red()},
//         {ColorType::Green, new Green()},
//     };
// };

int main(int argc, char const *argv[])
{
    Color *color = ColorFactory::create(ColorType::Red);
    color->display();
    delete color;
    // auto color = ColorFactoryMap::create(ColorType::Green);
    // color->display();
    return 0;
}
