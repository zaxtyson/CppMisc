#include <iostream>
#include <chrono>

using namespace std;

template<typename Callable, typename ...Args>
void measure_time(Callable &&func, Args &&...args) {
    auto start = chrono::system_clock::now();
    func(args...);
    chrono::duration<double> elapsed = chrono::system_clock::now() - start;
    cout << "elapsed: " << elapsed.count() << "s" << endl;
}

int main() {
    measure_time([](int n) {
        unsigned long long sum = 0;
        for (unsigned int i = 0; i < 1e9; i++) {
            sum += i;
        }
    }, 3);
    cout << "main end" << endl;
}

