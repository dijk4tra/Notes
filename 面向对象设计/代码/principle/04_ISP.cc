#include <iostream>
using namespace std;

/*
ISP: 接口隔离原则
一个类不应该依赖于它不需要的接口
*/
// 定义一个接口类-->只有纯虚函数的类
class IBird
{
public:
    // 所有的鸟都会飞
    virtual void fly()   = 0;  
    
    virtual void swim()  = 0;
    virtual void run()  = 0;
    virtual ~IBird() = default;
};


// 乌鸦类
class Crow : public IBird
{
public:
    virtual void fly() override
    {
        cout << "Crow fly fly" << endl;
    }

    virtual void swim() override
    {
        cout << "Crow swim swim" << endl;
    }
    virtual void run() override
    {
        cout << "Crow run run" << endl;
    }
};

// 鸵鸟类
class Ostrich : public IBird
{
public:
    virtual void fly() override
    {
        cout << "Ostrich can not fly!!!" << endl;
    }

    virtual void swim() override
    {
        cout << "Ostrich swim swim" << endl;
    }
    virtual void run() override
    {
        cout << "Ostrich run run" << endl;
    }
};

/*
抽象类IBird要求子类实现所有的抽象函数
但是并不是所有的鸟都会飞 都会游泳 
强迫鸵鸟实现会飞的行为 违背了ISP原则
*/
void func(IBird &bird)
{
    bird.fly(); // 如果参数为Ostrich对象 并不会飞 

}


/*
修改: 将抽象类中的抽象函数拆分成多个小接口类
*/
// 飞行的能力
class IFlyable{
    public:
        virtual void fly() = 0;
        virtual ~IFlyable() = default;
};

class ISwimable
{
    public:
        virtual void swim() = 0;
        virtual ~ISwimable() = default;
};

class IRunable
{
    public:
        virtual void run() = 0;
        virtual ~IRunable() = default;
};

class Ostrich2 : public IRunable, public ISwimable
{
public:
    virtual void run() override
    {
        cout << "Ostrich run run" << endl;
    }
    virtual void swim() override
    {
        cout << "Ostrich swim swim" << endl;
    }
};

void func2(IFlyable * flyable)
{
    flyable->fly();
}

void func3(ISwimable * swimable)
{
    swimable->swim();
}

void func4(IRunable * runable)
{
    runable->run();
}

void test1()
{
    Ostrich2 ostrich;
    // func2(&ostrich);
    func3(&ostrich);
    func4(&ostrich);
}

int main(int argc, char const *argv[])
{
    test1();
    return 0;
}
