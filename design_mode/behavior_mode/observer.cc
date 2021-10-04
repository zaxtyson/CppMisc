#include <iostream>

using std::string;

class Subscriber
{
public:
    Subscriber() = default;
    virtual void update(const string &message) = 0;
    virtual ~Subscriber() {}
};

class Publisher
{
public:
    Publisher() = default;
    virtual void registerSubscriber(const Subscriber &subscriber) = 0;
    virtual void removeSubscriber(const Subscriber &subscriber) = 0;
    virtual void notifySubscribers(const string &message) = 0;
    virtual ~Publisher() {}
};

class MsgPublisher : public Publisher
{
public:
    MsgPublisher() = default;
    ~MsgPublisher(){};

    void registerSubscriber(const Subscriber &subscriber)
};

int main(int argc, char const *argv[])
{

    return 0;
}
