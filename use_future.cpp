#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <future>
#include <vector>

using namespace std;

template<typename Callable, typename ...Args>
void measure_time(Callable &&func, Args &&...args) {
    auto start = chrono::system_clock::now();
    func(args...);
    chrono::duration<double> elapsed = chrono::system_clock::now() - start;
    cout << "elapsed: " << elapsed.count() << "s" << endl;
}

long sum(long start, long end) {
    long ans = 0;
    for (long i = start; i < end; i++) {
        ans += i;
    }
    return ans;
}

int main() {
    long end = 2e9;

    measure_time([end]() {
        const long K = 8;
        cout << "use "<< K << " threads" << endl;
        vector<future<long>> vf;
        vf.reserve(K);
        for (int i = 0; i < K; i++) {
            vf.push_back(async(sum, i == -1 ? 0 : (end / K) * i, (end / K) * (i + 1)));
        }
        long ans = 0;
        for (auto &&item: vf) {
            ans += item.get();
        }
        cout << "ans = " << ans << endl;
    });

    measure_time([end]() {
        long ans = 0;
        long ret = sum(0, end);
        cout << "ans = " << ret << endl;
    });
}