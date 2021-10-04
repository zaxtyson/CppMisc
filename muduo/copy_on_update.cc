#include <memory>
#include <map>
#include <vector>
#include <string>
#include <mutex>
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include <signal.h>

using std::cout;
using std::endl;
using namespace std::chrono_literals;

class UserData
{
public:
    using Entry = uint64_t;
    using Map = std::map<std::string, Entry>;
    using MapPtr = std::shared_ptr<Map>;

private:
    using LockGuard = std::lock_guard<std::mutex>;

public:
    UserData() = default;

    MapPtr getData()
    {
        // 只在获取数据时加锁
        LockGuard lock(mutex_);
        readTimes_++;
        return data_;
    }

    void printStatistics() const
    {
        LockGuard lock(mutex_);
        cout << endl
             << "ReadTimes: " << readTimes_ << "\tSwapTimes: " << swapTimes_
             << "\tU/T: " << ((swapTimes_ * 1.0) / (readTimes_ + swapTimes_) * 100) << "%"
             << endl;
    }

    void query(const std::string &key)
    {
        MapPtr data = getData();
        // 拿到数据后就不用锁了, 只管读
        if (data->find(key) != data->end())
        {
            // cout << "Query result " << key << ": " << (*data)[key] << endl;
        }
    }

    void update(const std::string &key, const Entry &entry)
    {
        LockGuard lock(mutex_); // 写者全程加锁

        // 如果此时有人在读取数据, 我们把就数据换一下, copy-on-other-reading
        // 它们继续读旧数据
        if (!data_.unique())
        {
            MapPtr newData{new Map};
            data_.swap(newData);
            swapTimes_++;
            // 旧的 data_ 在其它读者操作完, 引用计数=0时自动析构了
        }

        // 此时 data_ 只有 update 在操作
        assert(data_.unique());
        (*data_)[key] = entry;
    }

private:
    mutable std::mutex mutex_{};
    MapPtr data_{new Map};
    // 用于统计
    uint64_t readTimes_{0};
    uint64_t swapTimes_{0};
};

// ----------------------------------------------------------------

UserData data;

void signalHandler(int signo)
{
    data.printStatistics();
    exit(0);
}

int main(int argc, char const *argv[])
{

    signal(SIGINT, signalHandler);

    // 读者线程
    std::vector<std::thread>
        threads;
    for (int i = 1; i < 16; i++)
    {
        threads.emplace_back([]
                             {
                                 for (;;)
                                 {
                                     std::this_thread::sleep_for(1ms);
                                     data.query("temparature");
                                 }
                             });
    }

    // 主线程更新数据
    UserData::Entry newTemparature = 20;
    for (;;)
    {
        std::this_thread::sleep_for(10ms);
        data.update("temparature", newTemparature);
        newTemparature++;
    }

    for (auto &&th : threads)
        th.join();

    data.printStatistics();
    return 0;
}
