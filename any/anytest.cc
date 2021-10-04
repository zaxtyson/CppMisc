#include <any>
#include <iostream>
#include <memory>
using namespace std;

struct Object {
    Object() {
        cout << "Object()" << endl;
    }

    void say(){
        cout << "Hello" << endl;
    }

    ~Object() {
        cout << "~Object()" << endl;
    }
};

int main(int argc, char const *argv[])
{
    {
        shared_ptr<void> object = shared_ptr<void>(new Object());
    }
    return 0;
}
