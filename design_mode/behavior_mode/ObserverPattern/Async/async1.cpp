#include <memory>
#include <list>
#include <algorithm>
#include <thread>
#include <fmt/ostream.h>

namespace async {
    using std::string;
    using std::endl;
    using std::shared_ptr;
    using fmt::print;

    class Subscriber {
    public:
        virtual void update(const string &msg) = 0;

        virtual ~Subscriber() = default;
    };

    class Publisher {
    public:
        virtual void registerSubscriber(shared_ptr<Subscriber> subscriber) = 0;

        virtual void removeSubscriber(shared_ptr<Subscriber> subscriber) = 0;

        virtual void notifySubscribers(const string &msg) = 0;

        virtual ~Publisher() = default;
    };

// ==================================

    class MsgSubscriber : public Subscriber {
    private:
        string m_name;
    public:
        explicit MsgSubscriber(string name) : m_name(std::move(name)) {}

        void update(const string &msg) override {
            print("[{}] {}: {}\n", std::this_thread::get_id(), m_name, msg);
        }

        ~MsgSubscriber() override {
            print("{}: ~MsgSubscriber()\n", m_name);
        };
    };

    class MsgPublisher : public Publisher {
    private:
        string m_name;
        std::list<shared_ptr<Subscriber>> subscribers;

    public:
        explicit MsgPublisher(string name) : m_name(std::move(name)) {}

        void registerSubscriber(shared_ptr<Subscriber> subscriber) override {
            if (std::find(subscribers.begin(), subscribers.end(), subscriber) == subscribers.end()) {
                subscribers.emplace_back(subscriber);
            }
        }

        void removeSubscriber(shared_ptr<Subscriber> subscriber) override {
            subscribers.remove(subscriber);
        }

        void notifySubscribers(const string &msg) override {
            std::vector<std::thread> threads;
            for (const auto &subscriber: subscribers) {
                threads.emplace_back([&] {
                    subscriber->update(msg);
                });
            }
            for (auto &t: threads) {
                t.join();
            }
        }

        ~MsgPublisher() override {
            subscribers.clear();
            print("{}: ~MsgPublisher()", m_name);
        };
    };

    void test_async1() {
        shared_ptr<Publisher> p{new MsgPublisher("Teacher")};
        shared_ptr<Subscriber> s1{new MsgSubscriber("zhangsan")};
        shared_ptr<Subscriber> s2{new MsgSubscriber("lisi")};
        shared_ptr<Subscriber> s3{new MsgSubscriber("wang5")};

        p->registerSubscriber(s1);
        p->registerSubscriber(s2);
        p->registerSubscriber(s3);
        p->notifySubscribers("This is a msg.");
        print("=====================\n");
        p->removeSubscriber(s1);
        p->notifySubscribers("This is another msg.");
    }
}