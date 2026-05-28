#include <iostream>
#include <list>
#include <memory>
#include <algorithm>

using namespace std;

// 抽象观察者类
class Observer
{
public:
    virtual ~Observer() = default;
    // 更新
    virtual void update(int state) = 0;
};
// 具体观察者类
class ConcreteObserver : public Observer
{
public:
    ConcreteObserver() = default;
    ~ConcreteObserver() = default;
    // 更新
    void update(int state) override
    {
        cout << "ConcreteObserver::update, state = " << state << endl;
    }
};

// 抽象主题类
class Subject
{
public:
    virtual ~Subject() = default;
    // 添加观察者
    virtual void attach(Observer *observer) = 0;
    // 移除观察者
    virtual void detach(Observer *observer) = 0;
    // 通知所有观察者
    virtual void notify() = 0;
};
// 具体主题类
class ConcreteSubject : public Subject
{
public:
    ConcreteSubject() = default;
    ~ConcreteSubject() = default;
    // 获取状态
    int getState() const
    {
        return m_state;
    }
    // 设置状态
    void setState(int state)
    {
        m_state = state;
        // 状态更新时  通知所有观察者
        notify();
    }
    // 添加观察者
    void attach(Observer *observer) override
    {
        m_observers.push_back(observer);
    }
    // 移除观察者
    void detach(Observer *observer) override
    {
        m_observers.remove(observer);
    }
    // 通知所有观察者
    void notify() override
    {
        for (auto observer : m_observers)
        {
            observer->update(m_state);
        }
    }

private:
    list<Observer *> m_observers;
    int m_state;
};



void test1()
{
    // 创建主题对象
    ConcreteSubject subject;
    // 创建观察者对象
    ConcreteObserver observer;
    ConcreteObserver observer2;
    // 添加观察者
    subject.attach(&observer);
    subject.attach(&observer2);
    // 主题对象更新
    subject.setState(100);
}

int main(int argc, char const *argv[])
{
    test1();
    return 0;
}
