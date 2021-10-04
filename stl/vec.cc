#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char const *argv[])
{
    vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vec.resize(1);
    // vec.resize(100);
    cout << "capacity: " << vec.capacity() << " size: " << vec.size() << endl;
    for (auto i : vec)
        cout << i << endl;
    return 0;
}
