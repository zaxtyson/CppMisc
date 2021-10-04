#include <iostream>
#include <memory>
#include <list>
#include <algorithm>

namespace sync {

    using std::string;
    using std::cout;
    using std::endl;
    using std::shared_ptr;

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
            cout << m_name << ": " << msg << endl;
        }

        ~MsgSubscriber() override {
            cout << "~MsgSubscriber() : " << m_name << endl;
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
            for (const auto &subscriber: subscribers) {
                subscriber->update(msg);
            }
        }

        ~MsgPublisher() override {
            subscribers.clear();
            cout << "~MsgPublisher() : " << m_name << endl;
        };
    };

    void test() {
        shared_ptr<Publisher> p{new MsgPublisher("Teacher")};
        shared_ptr<Subscriber> s1{new MsgSubscriber("zhangsan")};
        shared_ptr<Subscriber> s2{new MsgSubscriber("lisi")};
        shared_ptr<Subscriber> s3{new MsgSubscriber("wang5")};

        p->registerSubscriber(s1);
        p->registerSubscriber(s2);
        p->registerSubscriber(s3);
        p->notifySubscribers("This is a msg.");
        cout << "============" << endl;
        p->removeSubscriber(s1);
        p->notifySubscribers("This is another msg.");
    }

}