#include <iostream>
using std::cout;
using std::endl;

/*
 * LSP:里氏替换原则
 * 1.子类可以替换父类
 * 2.子类可以扩展父类的功能 但不能改变父类原本的功能
 */

// 矩形与正方形的关系
class Rectangle
{
public:
    // 虚函数, 子类可以重写
    virtual void setWidth(int width)
    {
        m_width = width;
    }
    // 虚函数, 子类可以重写
    virtual void setHeight(int height)
    {
        m_height = height;
    }
    // 普通函数
    int getArea()
    {
        return m_width * m_height;
    }
    virtual ~Rectangle() = default;

    int m_width;
    int m_height;
};

class Square : public Rectangle
{
public:
    virtual void setWidth(int width) override
    {
        m_width = width;
        m_height = width; // 正方形的宽和高是相等的
    }
    virtual void setHeight(int height) override
    {
        m_height = height;
        m_width = height;
    }
};

void calculateArea(Rectangle &rect)
{
    // LSP:子类可以替换父类
    cout << "area:" << rect.getArea() << endl;
    // 测试发现子类改变父类原本的功能 不能完全替代父类
}

void test1()
{
    Rectangle rect;
    rect.setWidth(10);
    rect.setHeight(20);
    calculateArea(rect);
    cout << "-------" << endl;
    Square square;
    square.setWidth(10);
    square.setHeight(20);
    calculateArea(square);
}

// 修改, 使用抽象类, 子类继承抽象类
class Shape
{
public:
    // 纯虚函数, 子类必须实现
    virtual int getArea() = 0;
    virtual ~Shape() = default;
};

class Rectangle2 : public Shape
{
public:
    virtual int getArea() override
    {
        return m_width * m_height;
    }
    int m_width;
    int m_height;
};

class Square2 : public Shape
{
public:
    virtual int getArea() override
    {
        return m_side * m_side;
    }

    int m_side;
};

void calculateArea(Shape &shape)
{
    cout << "area:" << shape.getArea() << endl;
}

void test2()
{
    Rectangle2 rect;
    rect.m_width = 10;
    rect.m_height = 20;
    calculateArea(rect);
    cout << "-------" << endl;
    Square2 square;
    square.m_side = 10;
    calculateArea(square);
}

/*
LSP在C++中的体现:
1.使用虚函数, 子类可以重写父类的虚函数
2.使用纯虚函数, 子类必须实现纯虚函数
3.不要在子类中隐藏父类中的普通成员函数
*/
class Father
{
public:
    // 纯虚函数
    // virtual void func1() = 0;
    // 虚函数
    virtual void func2()
    {
        cout << "fatherMethod2" << endl;
    }
    // 普通函数
    void func3()
    {
        cout << "fatherMethod3" << endl;
    }
};

class Son : public Father
{
public:
    // 纯虚函数, 子类必须实现
    // virtual void func1() override
    // {
    //     cout << "sonMethod" << endl;
    // }
    // 虚函数, 子类可以重写
    virtual void func2() override
    {
        cout << "sonMethod2" << endl;
    }
    // 普通函数  不建议隐藏
    void func3()
    {
        cout << "sonMethod3" << endl;
    }

    // 子类自己定义的行为
    void func4()
    {
        cout << "sonMethod4" << endl;
    }
};

void test3()
{
    Father father;
    Son son;
    // 通过对象访问 针对普通父子类中的普通函数
    father.func3();
    son.func3(); // 隐藏父类的普通函数 违背LSP原则 改变了父类的行为


    cout << "-------" << endl;
    // 多态体现, 通过父指针访问虚函数
    Father * ff = &father;
    Father * fs = &son;
    ff->func2(); // fatherMethod2
    fs->func2(); // sonMethod2 子类的虚函数--->重写 多态


}

int main(int argc, char const *argv[])
{
    // test1();
    // test2();
    test3();
    return 0;
}
