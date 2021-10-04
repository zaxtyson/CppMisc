#include <iostream>
#include <filesystem> // C++17

// https://en.cppreference.com/w/cpp/header/filesystem

using std::cout;
using std::endl;
using std::string;
namespace fs = std::filesystem;

void show_path_info(const fs::path &path)
{
    cout << "======================" << endl;
    if (!fs::exists(path))
    {
        cout << path << " not found" << endl;
        return;
    }

    if (fs::is_directory(path))
        cout << "* This is directory: " << path << endl;
    else
        cout << "* This is file: " << path << endl;

    cout << "parent_path: " << path.parent_path() << endl;
    cout << "root_name: " << path.root_name() << endl;
    cout << "root_path: " << path.root_path() << endl;
    cout << "root_directory: " << path.root_directory() << endl;
    cout << "filename: " << path.filename() << endl;
    cout << "extension: " << path.extension() << endl;
}

int main(int argc, char const *argv[])
{
    cout << "CXX Version: " << __cplusplus << endl;

    fs::path root = "/home/zaxtyson/cpptest/modern_cpp";
    show_path_info(root);

    fs::path file = root / "chapter02.cc";
    show_path_info(file);

    //  fs::path file = root + fs::path("/chapter02.cc");
    //  show_path_info(file);
}
