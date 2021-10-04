#include "template.hpp"

int main(int argc, char const *argv[])
{
    Foo<const char *> obj;
    obj.bar("Hello");

    Foo<int> obj2;
    obj2.bar(111);

    return 0;
}
