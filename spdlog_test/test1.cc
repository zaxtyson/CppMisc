#include <iostream>
#include <spdlog/spdlog.h>

int main(int argc, char const *argv[])
{
    spdlog::info("Welcome to spdlog");
    spdlog::warn("Warning");
    spdlog::error("Something wrong happened: {}", "aaaaaaa");
    spdlog::critical("die");
    return 0;
}
