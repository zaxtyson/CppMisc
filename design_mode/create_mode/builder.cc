#include <iostream>
using std::string;

class ResourcePoolConfig
{
private:
    string name;
    int32_t max_total;
    int32_t max_idle;

public:
    class Builder
    {
    private:
        friend class ResourcePoolConfig;
        string name;
        int32_t max_total = 8;
        int32_t max_idle = 8;

    public:
        Builder() = default;
        Builder &build()
        {
            // validate args here
            if (name.empty())
            {
                throw std::runtime_error("name is invalid");
            }
            if (max_idle > max_total)
            {
                throw std::runtime_error("max_idle too large");
            }
            // and more...
            return *this;
        }

        Builder &setName(const string &name)
        {
            this->name = "Pool[" + name + "]";
            return *this;
        }

        Builder &setMaxIdle(int32_t max_idle)
        {
            this->max_idle = max_idle;
            return *this;
        }

        Builder &setMaxTotal(int32_t max_total)
        {
            this->max_total = max_total;
            return *this;
        }
    };

public:
    ResourcePoolConfig(Builder &builder)
    {
        name = builder.name;
        max_total = builder.max_total;
        max_idle = builder.max_idle;
    }

    void display() const
    {
        std::cout << "name: " << name << std::endl;
        std::cout << "max_total: " << max_total << std::endl;
        std::cout << "max_idle: " << max_idle << std::endl;
    }
};

int main(int argc, char const *argv[])
{
    ResourcePoolConfig config = ResourcePoolConfig::Builder().setName("01").setMaxTotal(10).setMaxIdle(6).build();
    config.display();
    return 0;
}
