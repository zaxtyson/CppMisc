#include <iostream>
#include <functional>
#include <string>

int main(int argc, char *argv[])
{
    size_t n = std::hash<std::string>()("Just fucking google it");
    std::cout << n << std::endl;

    return 0;
}
