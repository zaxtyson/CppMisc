#include <iostream>
using namespace std;

int main(int argc, char const *argv[])
{
    set_new_handler([]()
                    {
                        cout << "no more memery" << endl;
                        abort();
                    });

    try
    {
        int *p = new int[10000000000];
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
