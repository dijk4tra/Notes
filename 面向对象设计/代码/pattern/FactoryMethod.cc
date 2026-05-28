#include <iostream>
#include <memory>

using namespace std;

/*
工厂方法: 适合产品种类多且变化频繁的场景
定义一个抽象工厂接口类
每个具体的工厂子类负责创建对应的产品(一个工厂生产一类产品)
新增产品只需要新增一个具体的工厂子类即可，无需修改已有的代码
*/

class Fruit
{
public:
    virtual void show() = 0;
    virtual ~Fruit() = default;
};

class Apple : public Fruit
{
public:
    void show() override
    {
        cout << "i am an apple" << endl;
    }
};

class Orange : public Fruit
{
public:
    void show() override
    {
        cout << "i am an orange" << endl;
    }
};

// 抽象工厂接口类
class FruitFactory
{

public:
    // 纯虚函数 作为接口
    virtual unique_ptr<Fruit> createFruit() = 0;
    virtual ~FruitFactory() = default;
};

// 具体工厂子类
class AppleFactory : public FruitFactory
{
public:
    unique_ptr<Fruit> createFruit() override
    {
        cout << "create apple" << endl;
        return make_unique<Apple>();
    }
};
class OrangeFactory : public FruitFactory
{
public:
    unique_ptr<Fruit> createFruit() override
    {
        cout << "create orange" << endl;
        return make_unique<Orange>();
    }
};
// 如果需要新增其他水果，只需要新增一个具体的工厂子类即可

void test1()
{
    unique_ptr<FruitFactory> appleFactory = make_unique<AppleFactory>();
    auto apple = appleFactory->createFruit();
    apple->show();
    cout << "--------" << endl;
    unique_ptr<FruitFactory> orangeFactory = make_unique<OrangeFactory>();
    auto orange = orangeFactory->createFruit();
    orange->show();
}

int main(int argc, char const *argv[])
{
    test1();

    return 0;
}
