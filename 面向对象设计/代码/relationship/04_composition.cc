#include <iostream>
using namespace std;

/*
组合关系 : 整体与局部的关系, 整体和局部不相互独立  整体与局部共存亡(共享生命周期)
*/
class Heart {
public:
    void beat() { cout << "心脏跳动..." << endl; }
    Heart() { cout << "心脏诞生" << endl; }
    ~Heart() { cout << "心脏消亡" << endl; }
};

class Person {
public:
    Person() {
        heart = new Heart();  // 构造整体时时创建局部
        cout << "人出生" << endl;
    }
    void live() { heart->beat(); }
    ~Person() {
        delete heart;        // 析构: 销毁整体时 销毁局部
        cout << "人去世" << endl;
    }
private:
    Heart* heart;  // 组合：Person 独占 Heart 的生命周期
};

int main() {
    Person p;
    p.live();
    return 0;
}