#include <iostream>
using namespace std;

/*
继承关系 : 继承章节讲过 不再赘述 简单看下就行

*/

class Animal {
public:
    virtual void speak() { cout << "动物叫" << endl; }
    virtual ~Animal() {}
};

class Dog : public Animal {
public:
    void speak() override { cout << "汪汪" << endl; }
};

int main() {
    Animal* a = new Dog();
    a->speak();   // 多态调用 Dog::speak
    delete a;
    return 0;
}