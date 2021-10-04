#include <iostream>

using std::string;

class Singleton
{

private:
    string name;

public:
    Singleton()
    {
        name = "Zhangsan";
        std::cout << "Singleton()" << std::endl;
    }

    ~Singleton()
    {
        std::cout << "~Singleton()" << std::endl;
    }

    static Singleton &getInstance()
    {
        static Singleton instance; // this is thread safe in c++11
        return instance;
    }

    string getName()
    {
        return name;
    }

private:
    Singleton(const Singleton &) = delete;
    Singleton(const Singleton &&) = delete;
    Singleton &operator=(const Singleton &) = delete;
    Singleton &operator=(const Singleton &&) = delete;
};

int main(int argc, char const *argv[])
{
    string name{Singleton::getInstance().getName()};
    std::cout << "name is " << name << std::endl;
    return 0;
}
