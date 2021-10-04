#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

class A {
public:
    void operator()(int x) {
        for (int i = 0; i < x; i++) {
            cout << "id = " << this_thread::get_id() << ", i = " << i << endl;
//            this_thread::sleep_for(100ms); // c++14
            this_thread::sleep_for(chrono::milliseconds(1000)); // c++11
        }
    }
};

void func(const char* s) {
    cout << "func: " << s << endl;
}

int main() {
    thread t0 = thread([]() {
//        cout << "lambda: " << this_thread::get_id() << endl;
        A a;
        a(10);
    });

    thread t1 = thread(A(), 10);
    thread t2 = thread(func, "world");
    t0.join();
    t1.join();
    t2.join();
    cout << "main end" << endl;
}
