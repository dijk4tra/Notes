#include <iostream>
using namespace std;
/*
依赖关系: A use B
一个类通过方法参数、局部变量等临时使用另一个类，不持有对方成员。
*/
class Printer {
public:
    void print(const string& content) {
        cout << "打印: " << content << endl;
    }
};

class User {
public:
    // 依赖 Printer 作为方法参数
    void doPrint(Printer& printer, const string& text) {
        printer.print(text);
    }
};

int main() {
    Printer p;
    User u;
    u.doPrint(p, "Hello World");
    return 0;
}