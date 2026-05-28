#include <iostream>
#include <memory>

using namespace std;

/*
抽象工厂: 负责创建一系列相关的产品族 
*/

// 抽象苹果类
class Apple{
    public:
        virtual void show() = 0;
        virtual ~Apple() = default;
};

class ChineseApple : public Apple{
    public:
        void show() override{
            cout << "红富士" << endl;
        }
};

class USAApple : public Apple{
    public:
        void show() override{
            cout << "蛇果" << endl;
        }
};

class Orange
{
    public:
        virtual void show() = 0;
        virtual ~Orange() = default;
};

class ChineseOrange : public Orange{
    public:
        void show() override{
            cout << "本地橙子" << endl;
        }
};

class USAOrange : public Orange{
    public:
        void show() override{
            cout << "进口橙子" << endl;
        }
};

// 抽象工厂接口类
class FruitFactory{

public:
    // 纯虚函数 作为接口
    virtual unique_ptr<Apple> createApple() = 0;
    virtual unique_ptr<Orange> createOrange() = 0;
    virtual ~FruitFactory() = default;
};

// 具体工厂子类
class ChineseFruitFactory : public FruitFactory{
public:
    unique_ptr<Apple> createApple() override{
        cout << "生产红富士" << endl;
        return make_unique<ChineseApple>();
    }
    unique_ptr<Orange> createOrange() override{
        cout << "生产本地橙子" << endl;
        return make_unique<ChineseOrange>();
    }
};
class USAFruitFactory : public FruitFactory{
public:
    unique_ptr<Apple> createApple() override{
        cout << "生产蛇果" << endl;
        return make_unique<USAApple>();
    }
    unique_ptr<Orange> createOrange() override{
        cout << "生产进口橙子" << endl;
        return make_unique<USAOrange>();
    }
};


/*
特点: 增加产品族时，只需要新增一个具体的工厂子类即可，无需修改已有的代码
中国工厂,美国工厂不需要改动

但是 增加一个新品牌(新风格), 即产品等级结构 , 日系的苹果和橙子, 那么就需要修改现有的代码, 需要
修改抽象工厂接口类, 违背OCP开闭原则

*/

void test1()
{
    // 使用时不需要知道具体工厂子类的类型, 只需要知道抽象工厂接口类的类型
    unique_ptr<FruitFactory> fruitFactory = make_unique<ChineseFruitFactory>();
    // 没有使用具体类, 只使用了抽象类 不关心具体产品的实现细节
    unique_ptr<Apple> apple = fruitFactory->createApple(); 
    // createApple() 时，它并不知道返回的 Apple* 实际上指向的是 ChineseApple 
    // 还是 USAApple
    // 它只相信这个对象是一个 Apple,可以调用 show() 方法。
    apple->show(); 
    unique_ptr<Orange> orange = fruitFactory->createOrange();
    orange->show(); 
    cout << "--------" << endl;
    fruitFactory = make_unique<USAFruitFactory>();
    unique_ptr<Apple> usApple = fruitFactory->createApple();
    usApple->show(); 
    unique_ptr<Orange> usOrange = fruitFactory->createOrange();
    usOrange->show(); 
}

int main(int argc, char const *argv[])
{
    test1();

    return 0;
}

