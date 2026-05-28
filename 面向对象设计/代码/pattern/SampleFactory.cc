#include <iostream>
#include <memory>

using namespace std;

/*
简单工厂模式: 适合产品种类少且基本不变的场景
根据参数创建不同类型的产品
*/

class Fruit{
    public:
        virtual void show() = 0;
        virtual ~Fruit() = default;
};

class Apple : public Fruit{
    public:
        void show() override{
            cout << "i am an apple" << endl;
        }
};

class Orange : public Fruit{
    public:
        void show() override{
            cout << "i am an orange" << endl;
        }
};

class FruitFactory{

public:
    // 根据传入的类型创建不同的水果
    static Fruit* createFruit(const string& type){
        if(type == "apple"){
            cout << "create apple" << endl;
            return new Apple();
        }else if(type == "orange"){
            cout << "create orange" << endl;
            return new Orange();
        }
        return nullptr;
    }

    // 或者返回值直接设置为智能unique_ptr<Fruit>类型
    static unique_ptr<Fruit> createFruit2(const string& type){
        if(type == "apple"){
            cout << "create apple" << endl;
            return make_unique<Apple>();
        }else if(type == "orange"){
            cout << "create orange" << endl;
            return make_unique<Orange>();
        }
        return nullptr;
    }
};


void test1()
{
    Fruit* apple = FruitFactory::createFruit("apple");
    Fruit* orange = FruitFactory::createFruit("orange");
    // 资源如何回收?  可以使用智能指针
    unique_ptr<Fruit> p1(apple);
    unique_ptr<Fruit> p2(orange);
    cout << "=======" << endl;
    unique_ptr<Fruit> p3 = FruitFactory::createFruit2("apple");
    unique_ptr<Fruit> p4 = FruitFactory::createFruit2("orange");
    p3->show();
    p4->show();
    
}

int main(int argc, char const *argv[])
{
    test1();

    return 0;
}

