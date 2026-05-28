#include <iostream>
#include <memory>
using namespace std;

/*
组合复用原则: 组合优于继承
*/


class Vechicle
{
public:
    virtual void run() = 0;
    ~Vechicle() = default;

};

class Tesla : public Vechicle
{
public:
    virtual void run() override
    {
        cout << "run tesla" << endl;
    }
};

class BYD : public Vechicle
{
public:
    virtual void run() override
    {
        cout << "run byd" << endl;
    }
};

class Geely : public Vechicle
{
public:
    virtual void run() override
    {
        cout << "run geely" << endl;
    }
};

class Person
{
public:
    void setVechicle(Vechicle * vechicle)
    {
        m_vechicle = vechicle;
    }
    void drive()
    {
        m_vechicle->run();
    }
private:
    Vechicle * m_vechicle;

};

void test1()
{

    unique_ptr<Vechicle> tesla(new Tesla());
    unique_ptr<Vechicle> byd(new BYD());
    unique_ptr<Vechicle> geely(new Geely());

    Person p;
    p.setVechicle(tesla.get());
    p.drive();

    p.setVechicle(byd.get());
    p.drive();
    
    p.setVechicle(geely.get());
    p.drive();



}

int main(int argc, char const *argv[])
{
    
    return 0;
}

