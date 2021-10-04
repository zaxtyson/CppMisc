#include <iostream>
#include <string>

using namespace std;

struct point{
    int x;
    int y;
    point(int x, int y) : x(x), y(y){
        cout << "point constor" << endl;
    }
};

void test(point& p){
    p.x = 1000;
    cout << p.x << " " << p.y << endl;
}

int main(int argc, char const *argv[])
{
    point p = {3, 5};
    test(p);
    cout << p.x << " " << p.y << endl;
    return 0;
}
