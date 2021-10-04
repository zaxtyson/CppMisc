#include <queue>
#include <iostream>
#include <vector>
#include <memory>
#include <unistd.h>

//using namespace std;
using std::cout;
using std::endl;

struct Object
{
    int value_;
    explicit Object(int value) : value_(value) {}

    // Object(Object &&)
    // {
    //     cout << "Move ctor" << endl;
    // }

    ~Object()
    {
        cout << "~Objec(), value = " << value_ << endl;
    }

    bool operator<(const Object &other) const { return value_ < other.value_; }
};

using ObjectPtr = std::unique_ptr<Object>;

int main(int argc, char const *argv[])
{
    struct cmp
    {
        bool operator()(const ObjectPtr &o1, const ObjectPtr &o2)
        {
            return o2->value_ < o1->value_;
        }
    };

    std::priority_queue<ObjectPtr, std::vector<ObjectPtr>, cmp> queue;
    // queue.push(std::make_unique<Object>(100));

    for (auto i : {3,4,4,1,5})
    {
        queue.push(std::make_unique<Object>(i));
    }

    // while(!queue.empty()){
    //     cout << queue.top()->value_ << " ";
    //     queue.pop();
    // }
    // cout << std::endl;

    // std::vector<ObjectPtr> vec;
    // vec.push_back(std::make_unique<Object>(100));
    // ObjectPtr a = std::move(vec.front());
    // cout << a->value_ << endl;
    // cout << vec[0]->value_ << endl;
    // cout << vec.size() << std::endl;

    std::vector<ObjectPtr> ret;
    for (;;)
    {
        if (queue.top()->value_ > 4)
        {
            break;
        }
        auto a = std::move(queue.top());
        // ret.push_back();
        queue.pop();
    }

    cout << "wait..." << endl;
    sleep(3);

    // for (auto& i : ret)
    // {
    //     cout << i.value_ << " ";
    // }
    // cout << endl;
}
