#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

template <typename Callable, typename... Args>
void measure_time(Callable &&func, Args &&...args)
{
    auto start = chrono::system_clock::now();
    func(args...);
    chrono::duration<double> elapsed = chrono::system_clock::now() - start;
    cout << "elapsed: " << elapsed.count() << "s" << endl;
}

void sum(int &s)
{
    for (int i = 0; i < 1e8; i++)
    {
        s++;
    }
}

std::mutex mtx;
void sum2(int &s)
{
    mtx.lock();
    sum(s);
    mtx.unlock();
}

void test_without_mutex()
{
    measure_time([]()
                 {
                     vector<thread> vt;
                     vt.reserve(4);
                     int s = 0;
                     for (int i = 0; i < 4; i++)
                     {
                         //            vt.emplace_back(thread(sum, ref(s)));
                         vt.emplace_back(sum, ref(s));
                     }
                     for (auto &&t : vt)
                     {
                         t.join();
                     }

                     cout << "s = " << s << endl;
                 });
}

void test_with_mutex()
{
    measure_time([]()
                 {
                     vector<thread> vt;
                     vt.reserve(4);
                     int s = 0;
                     for (int i = 0; i < 4; i++)
                     {
                         vt.emplace_back(sum2, ref(s));
                     }
                     for (auto &&t : vt)
                     {
                         t.join();
                     }
                     cout << "s = " << s << endl;
                 });
}
int main()
{
    test_without_mutex();
    test_with_mutex();
    cout << "main end" << endl;
}
