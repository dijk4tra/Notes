#include <iostream>
using namespace std;

/*
 * OCP:开闭原则 : 对扩展开放 对修改关闭
 * 
 * 新增功能时，尽量不去修改已有的稳定代码，而是通过增加新代码来实现
 */

class Calculator
{
public:
    int getResult(int a, int b)
    {
        return a + b;
    }
};

// 设置为抽象类
class Calculator2
{
public:
    // 纯虚函数, 子类实现
    virtual int getResult(int a, int b) = 0;
};

// 加法类
class AddCalculator : public Calculator2
{
public:
    virtual int getResult(int a, int b) override
    {
        return a + b;
    }
};

// 新增需求, 原来的继承上进行扩展 新增减法类
class SubCalculator : public Calculator2
{
public:
    virtual int getResult(int a, int b) override
    {
        return a - b;
    }
};

// 还可以继承扩展乘法 除非类

void calculate(Calculator2 & calculator)
{
    cout << "calculator.getResult(1, 2) = " << calculator.getResult(1, 2) << endl;
}

void test1()
{

    AddCalculator addCalculator;
    calculate(addCalculator);
}

int main(int argc, char const *argv[])
{
    test1();
    return 0;
}
