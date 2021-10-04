#include <iostream>
#include <thread>
#include <iomanip>
#include <vector>
#include <algorithm>
using std::string;

class SimpleLogger
{
public:
    void log(const string &msg) const
    {
        auto now_point = std::chrono::system_clock::now();
        time_t now = std::chrono::system_clock::to_time_t(now_point);
        std::cout << "[" << std::put_time(std::localtime(&now), "%F %T") << "]: " << msg << std::endl;
    }
};

class IUserController
{
public:
    virtual ~IUserController(){};
    virtual bool login(const string &username, const string &password) = 0;
};

class UserController : public IUserController
{
public:
    UserController() = default;
    bool login(const string &username, const string &password) override
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1500)); // simulate login
        return username.compare("zhangsan") && password.compare("1234");
    }
};

class UserControllerProxy : public IUserController
{
private:
    UserController *m_controller;
    SimpleLogger *m_logger;

public:
    UserControllerProxy(UserController *controller) : m_controller(controller)
    {
        m_logger = new SimpleLogger();
    }
    ~UserControllerProxy()
    {
        delete m_logger;
    }
    // override the target interface
    bool login(const string &username, const string &password) override
    {
        m_logger->log("start login...");
        m_logger->log("username is " + username + ", password is " + password);

        // block blacklist users
        std::vector<string> blacklist{"lisi", "wang5"};
        if (std::find(blacklist.begin(), blacklist.end(), username) != blacklist.end())
        {
            m_logger->log("user is in blacklist, login forbidden!");
            return false;
        }

        // delegate request to raw object
        bool state = m_controller->login(username, password);
        m_logger->log(state ? "login sucess" : "login failure");
        return state;
    }
};

int main(int argc, char const *argv[])
{
    UserController *controller_ = new UserController();
    UserControllerProxy *controller = new UserControllerProxy(controller_);
    controller->login("zhangsan", "passwd");
    controller->login("zhangsan", "1234");
    controller->login("lisi", "1234");
    delete controller;
    delete controller_;
    return 0;
}
