#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <future>
#include <atomic>

using std::cout;
using std::endl;

// ----------------------------------------------------------------

void test_thread_mutex()
{
    int r = 0;
    auto func = [&r](int n) {
        static std::mutex mtx;
        // mtx.lock();  // 可以手动 lock 和 unlock

        // 或者使用 std::lock_guard<std::mutex> 当离开作用域会自动 unlock, 但是不能显式调用 lock 和 unlock
        // 但是建议用 unique_lock, 可以显示 unlock, 减小 lock 区域, 提高并发度
        std::unique_lock<std::mutex> lock(mtx);
        cout << "change r " << r << " to " << n << endl;
        r = n;
        lock.unlock(); // 显式释放锁

        // mtx.unlock();
    };

    std::thread t1(func, 1), t2(func, 2), t3(func, 3);
    t1.join();
    t2.join();
    t3.join();
}

// ----------------------------------------------------------------------------------------

void test_thread_future()
{
    auto my_work = []() {
        int sum = 0;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        for (int i = 0; i < 1000; i++)
            sum += i;
        return sum;
    };

    std::packaged_task<int()> task(my_work);
    std::future<int> result = task.get_future();
    std::thread(std::move(task)).detach();
    cout << "waiting for result..." << endl;
    result.wait();
    cout << "result is " << result.get() << endl;
}

// ----------------------------------------------------------------

struct SA
{
    // std::string name;
    int age;
    double score;
};

void test_atomic()
{
    std::atomic<int> count = {0};
    std::thread t1([&]() { count.fetch_add(1); });
    std::thread t2([&]() { count += 1; }); // 相当于 fetch_add
    t1.join();
    t2.join();
    cout << "count is " << count << endl;

    std::atomic<SA> sa;  // 并非所有类型都能满足原子操作, 这取决于CPU架构和内存对齐要求
    // cout << std::boolalpha << sa.is_lock_free() << endl;

    std::vector<std::thread> vt;
    for(int i = 0; i <100;i++)
    {
        vt.emplace_back([&](){
            count.fetch_add(1, std::memory_order_relaxed);
        });
    }

    for(auto& t : vt)
        t.join();
    
    cout << count << endl;
}

int main(int argc, char const *argv[])
{
    // test_thread_mutex();
    // test_thread_future();
    test_atomic();
}
