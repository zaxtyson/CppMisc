#include <iostream>

class Person
{
public:
    virtual ~Person(){};
    virtual void show() const = 0;
};

class PersonDecorator : public Person
{
protected:
    Person *m_person;

public:
    PersonDecorator(Person *person) : m_person(person) {}
    virtual ~PersonDecorator(){};
};

// -----------------------------------------------------

class Student : public Person
{
public:
    void show() const override
    {
        std::cout << "A student" << std::endl;
    }
};

class Teacher : public Person
{
public:
    void show() const override
    {
        std::cout << "A teacher" << std::endl;
    }
};

// ----------------------------------------------------

class GlassesDecorator : public PersonDecorator
{
public:
    GlassesDecorator(Person *person) : PersonDecorator(person) {}
    ~GlassesDecorator() = default;
    void show() const override
    {
        m_person->show();
        this->addGlasses();
    }

private:
    void addGlasses() const
    {
        std::cout << "with glasses" << std::endl;
    }
};

class ClothesDecorator : public PersonDecorator
{
public:
    ClothesDecorator(Person *person) : PersonDecorator(person) {}
    ~ClothesDecorator() = default;
    void show() const override
    {
        m_person->show();
        this->addCoolClothes();
    }

private:
    void addCoolClothes() const
    {
        std::cout << "with cool clothes" << std::endl;
    }
};

int main(int argc, char const *argv[])
{
    Person *person = new Student();
    Person *d1 = new GlassesDecorator(person);
    Person *d2 = new ClothesDecorator(d1);
    d2->show();
    delete d2;
    delete d1;
    delete person;
    return 0;
}
