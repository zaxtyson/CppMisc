#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <deque>
#include <map>
#include <string>
#include <cctype>
#include <iomanip>
#include <hash_map>
using namespace std;

int main(int argc, char const *argv[])
{
    // std::list<int> lst{12, 44, 66, -111, 12};
    // lst.emplace_back(111);
    // lst.emplace_front(44);
    // lst.unique();
    // lst.sort();
    // for (auto &i : lst)
    // {
    //     cout << i << endl;
    // }

    // cout << lst.front() << endl;
    // cout << lst.back() << endl;

    // cout << "=============================" << endl;

    // vector<int> vec;
    // vec.reserve(30);
    // vec.emplace_back(30);
    // if (find(vec.begin(), vec.end(), 30) != vec.end())
    //     cout << "yes: " << endl;
    // cout << vec.capacity() << endl;

    // cout << "=============================" << endl;

    // std::deque<int> d{1, 4, 33, 777};
    // d.emplace_back(100);
    // d.emplace_front(1);
    // std::sort(d.begin(), d.end());
    // for_each(d.begin(), d.end(), [](auto &i)
    //          { cout << i << endl; });
    // cout << d.at(1) << endl;

    // cout << "=============================" << endl;

    map<string, string> mp;
    mp.emplace("name", "zhangsan");
    mp.emplace("foo", "bar");
    mp.emplace("hello", "world");

    // for (auto iter = mp.begin(); iter != mp.end();)
    // {
    //     if (iter->second == "bar")
    //     {
    //         iter = mp.erase(iter);
    //     }
    //     else
    //     {
    //         iter++;
    //     }
    // }

    // for (auto &&[key, value] : mp)
    // {
    //     cout << key << ": " << value << endl;
    // }

    // vector<int> vec{12, 44, 12, 45, 66};
    // auto rm = [](int i)
    // { return i == 12; };
    // vec.erase(std::remove_if(vec.begin(), vec.end(), rm), vec.end());
    // // vec.erase(std::ranges::remove_if(vec, rm));

    // for (auto &i : vec)
    //     cout << i << endl;

    string str = "Hello World\tyyds";
    auto rm = [](unsigned char c)
    { return std::isspace(c); };
    std::remove(str.begin(), str.end(), ' ');
    // std::remove_if(str.begin(), str.end(), rm);
    cout << quoted(str) << endl;
    return 0;
}
