#include <iostream>
#include <map>
#include <cstring>
using std::string;

// ------------------ Products ----------------------------
class ConfigParser
{
public:
    ConfigParser() = default;
    virtual ~ConfigParser() = default;
    virtual void parse() = 0;
};

class JsonConfigParser : public ConfigParser
{
public:
    void parse()
    {
        std::cout << "parse json..." << std::endl;
    }
};

class XmlConfigParser : public ConfigParser
{
public:
    void parse()
    {
        std::cout << "parse xml..." << std::endl;
    }
};

// ------------- Factory ------------------------------

class ConfigParserFactory
{
public:
    ConfigParserFactory() = default;
    virtual ~ConfigParserFactory() = default;
    virtual ConfigParser *createParser() = 0;
};

class JsonConfigParserFactory : public ConfigParserFactory
{
public:
    ConfigParser *createParser()
    {
        return new JsonConfigParser();
    }
};

class XmlConfigParserFactory : public ConfigParserFactory
{
public:
    ConfigParser *createParser()
    {
        return new XmlConfigParser();
    }
};

// ----------------------------------------------------

int main(int argc, char const *argv[])
{
    const char *configType = argv[1];
    if (!configType)
    {
        throw std::runtime_error("except 1 arg, 0 was given");
    };

    ConfigParserFactory *factory = nullptr;
    if (strcmp(configType, "json") == 0)
        factory = new JsonConfigParserFactory();
    if (strcmp(configType, "xml") == 0)
        factory = new XmlConfigParserFactory();

    ConfigParser *parser = factory->createParser();
    parser->parse();
    delete parser;
    delete factory;
}
