# 第一章：C与C++

## 1. 一个基本 C++ 源文件包含哪些内容？

一个最小的 C++ 程序通常包含：

- 头文件引入：告诉编译器要使用哪些库。
- 命名空间处理：避免每次都写很长的名字。
- 函数或类的声明与定义：写具体功能。
- `main` 函数：程序入口。

```cpp
#include <iostream>

int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(3, 4);
    std::cout << result << std::endl;
    return 0;
}
```

编译执行：

```bash
g++ main.cpp -o main
./main
```

如果拆成头文件和源文件，通常这样组织：

```text
project/
  main.cpp       // 程序入口
  student.h      // 类、函数的声明
  student.cpp    // 类、函数的实现
```

> [!TIP]
>
> 常见组织原则：
>
> - `.h` 或 `.hpp` 放声明，说明“有什么”。
> - `.cpp` 放实现，说明“怎么做”。
> - 可执行程序必须有且只有一个 `main` 函数入口。

## 2. 命名空间 namespace

命名空间的主要作用是避免名字冲突。

假设两个库里都有一个叫 `print` 的函数，如果没有命名空间，编译器不知道你想调用哪一个。

```cpp
namespace A {
    void print() {
        std::cout << "A::print" << std::endl;
    }
}

namespace B {
    void print() {
        std::cout << "B::print" << std::endl;
    }
}

int main() {
    A::print();
    B::print();
    return 0;
}
```

命名空间的使用方式常见有三种。

第一种：每次都写完整名字，最清晰。

```cpp
std::cout << "hello" << std::endl;
```

第二种：引入某一个名字。

```cpp
using std::cout;
using std::endl;

cout << "hello" << endl;
```

第三种：引入整个命名空间。

```cpp
using namespace std;

cout << "hello" << endl;
```

> [!CAUTION]
>
> 初学阶段可以在小练习里用 `using namespace std;`，但在真实项目或头文件中不推荐这样写，因为它容易把很多名字一次性暴露出来，引发冲突。

> [!NOTE]
>
> 同一个作用域中可以多次定义同名命名空间，它们会自动合并。

```cpp
namespace Test {
    int a = 10;
}

namespace Test {
    int b = 20;
}

int main() {
    std::cout << Test::a << " " << Test::b << std::endl;
}
```

## 3. const 关键字

`const` 表示“只读”。被 `const` 修饰的对象不能通过这个名字被修改。

```cpp
const int n = 10;
// n = 20; // 错误
```

对于普通内置类型，`const` 的特点是：

- 必须初始化。
- 初始化后不能修改。
- 编译器可以利用它做安全检查和优化。

```cpp
const double pi = 3.14159;
```

#### const 修饰指针

理解 `const` 指针时，可以先看 `const` 离谁近。

`const int* p`：指向常量的指针。

```cpp
int a = 10;
int b = 20;

const int* p = &a;
// *p = 30; // 错误，不能通过 p 修改 a
p = &b;    // 正确，p 可以指向别的变量
```

`int const* p` 和 `const int* p` 含义相同。

```cpp
int const* p = &a;
```

`int* const p`：常量指针。

```cpp
int a = 10;
int b = 20;

int* const p = &a;
*p = 30;   // 正确，可以修改指向的值
// p = &b; // 错误，p 自己不能再指向别人
```

`const int* const p`：指向常量的常量指针。

```cpp
int a = 10;

const int* const p = &a;
// *p = 20; // 错误，不能改值
// p = nullptr; // 错误，不能改指向
```

> [!TIP]
>
> 记忆方式：
>
> - `const` 在 `*` 左边：指向的值不能改。
> - `const` 在 `*` 右边：指针变量本身不能改。

## 4. new/delete 与 malloc/free

`malloc/free` 是 C 语言的内存管理方式，`new/delete` 是 C++ 的内存管理方式。

```cpp
int* p1 = (int*)malloc(sizeof(int));
free(p1);

int* p2 = new int(10);
delete p2;
```

主要区别：

| 对比点 | `new/delete` | `malloc/free` |
| --- | --- | --- |
| 所属语言 | C++ | C |
| 返回类型 | 返回具体类型指针 | 返回 `void*`，常需要强转 |
| 构造函数 | 会调用构造函数 | 不会 |
| 析构函数 | `delete` 会调用析构函数 | 不会 |
| 失败行为 | 默认抛出异常 | 返回 `NULL` |

> [!IMPORTANT]
>
> 创建数组时要配对使用 `new[]` 和 `delete[]`：

```cpp
int* arr = new int[10];
delete[] arr;
```

> [!CAUTION]
>
> 不能混用 `new/delete` 和 `malloc/free`：

```cpp
int* p = new int;
// free(p); // 错误

int* q = (int*)malloc(sizeof(int));
// delete q; // 错误
```

## 5. 引用

引用可以理解为变量的别名。

```cpp
int a = 10;
int& r = a;

r = 20;
std::cout << a << std::endl; // 20
```

引用的特点：

- 定义时必须初始化。
- 初始化后不能再绑定到其他变量。
- 对引用的操作就是对原变量的操作。

> [!NOTE]
>
> 引用的本质可以理解为：编译器帮我们隐藏了一层指针，但使用形式更像普通变量。
>
> 对引用取地址，得到的是原变量的地址。

```cpp
int a = 10;
int& r = a;

std::cout << &a << std::endl;
std::cout << &r << std::endl; // 与 &a 相同
```

## 6. 引用和指针的区别

| 对比点 | 引用 | 指针 |
| --- | --- | --- |
| 是否必须初始化 | 必须 | 不必须 |
| 是否可以为空 | 正常引用不能为空 | 可以是 `nullptr` |
| 是否可以改变绑定目标 | 不可以 | 可以 |
| 使用方式 | 像普通变量 | 需要 `*` 解引用 |
| 常见用途 | 函数参数、返回值 | 动态内存、链表、数组、底层操作 |

引用常用于函数传参，避免拷贝，也能让函数修改外部变量。

```cpp
void swapValue(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}
```

> [!TIP]
>
> 如果不想函数修改参数，可以使用常引用。这样既能避免拷贝，又能保护实参不被函数修改。

```cpp
void printName(const std::string& name) {
    std::cout << name << std::endl;
}
```

## 7. 函数重载

函数重载是指：同一个作用域中，函数名相同，但参数列表不同。

```cpp
int add(int a, int b) {
    return a + b;
}

double add(double a, double b) {
    return a + b;
}

int add(int a, int b, int c) {
    return a + b + c;
}
```

形参列表不同可以表现为：

- 参数个数不同。
- 参数类型不同。
- 参数顺序不同。

```cpp
void test(int a, double b) {}
void test(double a, int b) {}
```

> [!CAUTION]
>
> 只有返回值不同不能构成重载。

```cpp
// int func(int a) {}
// double func(int a) {} // 错误
```

> [!NOTE]
>
> 函数重载的原理是：C++ 编译器会对函数名进行改编，把参数类型也编码到符号名中。因此同名函数在底层其实会变成不同的符号。

## 8. 内联函数 inline

内联函数的想法是：把函数调用处直接替换成函数体，减少函数调用开销。

```cpp
inline int square(int x) {
    return x * x;
}
```

适合使用内联函数的场景：

- 函数体很短。
- 调用非常频繁。
- 逻辑简单，例如 getter、setter、小计算函数。

不适合使用内联函数的场景：

- 函数体很长。
- 有复杂循环或递归。
- 函数逻辑经常变化，导致编译依赖扩大。

> [!IMPORTANT]
>
> `inline` 是给编译器的建议，不是强制命令。编译器可以选择内联，也可以选择不内联。

## 9. C++ 异常处理

C++ 用 `throw`、`try`、`catch` 处理异常。

```cpp
int divide(int a, int b) {
    if (b == 0) {
        throw std::runtime_error("除数不能为 0");
    }
    return a / b;
}

int main() {
    try {
        std::cout << divide(10, 0) << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
```

三个关键字的作用：

- `throw`：抛出异常。
- `try`：包住可能出问题的代码。
- `catch`：捕获并处理异常。

如果异常没有被处理，程序通常会终止。

如果异常被合适的 `catch` 捕获，程序可以继续执行。

> [!CAUTION]
>
> 异常适合处理“异常情况”，不应该用来代替普通的 `if/else` 流程控制。

## 10. C++ 程序的内存布局

一个程序运行时，内存通常可以粗略分为这些区域：

| 区域 | 存放内容 | 特点 |
| --- | --- | --- |
| 代码区 | 程序指令 | 通常只读 |
| 全局/静态区 | 全局变量、静态变量 | 程序开始时创建，程序结束时释放 |
| 常量区 | 字符串常量、部分常量数据 | 通常不能修改 |
| 栈区 | 局部变量、函数参数 | 自动创建和释放 |
| 堆区 | `new`、`malloc` 申请的内存 | 需要手动释放，或交给智能指针管理 |

示例：

```cpp
int globalValue = 10; // 全局/静态区

int main() {
    int localValue = 20;        // 栈区
    int* heapValue = new int;   // heapValue 变量在栈区，指向的 int 在堆区
    const char* text = "hello"; // 字符串字面量通常在常量区

    delete heapValue;
    return 0;
}
```

> [!IMPORTANT]
>
> 初学时最重要的是区分：
>
> - 栈上的变量离开作用域会自动释放。
> - 堆上的内存不会自动释放，忘记释放会导致内存泄漏。

## 本章反思

第一章的重点是理解 C++ 相比 C 增加了哪些更安全、更抽象的机制：命名空间解决命名冲突，引用简化间接访问，函数重载提高表达能力，`new/delete` 配合构造析构管理对象生命周期，异常提供错误传播方式。

# 第二章：类与对象

## 1. 类和对象

类是对一类事物的抽象描述，说明它有什么数据和行为。

对象是根据类创建出来的具体实例。

```cpp
#include <iostream>
#include <string>

class Student {
public:
    std::string name;
    int age;

    void study() {
        std::cout << name << " 正在学习" << std::endl;
    }
};

int main() {
    Student s;
    s.name = "张三";
    s.age = 18;
    s.study();

    return 0;
}
```

可以这样理解：

```text
类：学生这个概念
对象：张三、李四这些具体学生
```

## 2. 权限修饰符 public 和 private

权限修饰符控制类成员能不能被外部访问。

`public`：类外可以访问。

```cpp
class Student {
public:
    std::string name;
};

int main() {
    Student s;
    s.name = "张三";
}
```

`private`：只能在类内部访问。

```cpp
class Student {
private:
    int age;

public:
    void setAge(int value) {
        if (value >= 0) {
            age = value;
        }
    }

    int getAge() {
        return age;
    }
};
```

外部不能直接访问：

```cpp
Student s;
// s.age = 18; // 错误
s.setAge(18);
```

> [!IMPORTANT]
>
> `private` 的意义不是“麻烦”，而是保护对象状态，避免外部随意修改导致对象不合法。

## 3. struct 和 class 的区别

在 C++ 中，`struct` 和 `class` 都可以定义成员变量和成员函数。

主要区别是默认访问权限不同：

- `struct` 默认是 `public`。
- `class` 默认是 `private`。

```cpp
struct A {
    int x; // 默认 public
};

class B {
    int x; // 默认 private
};
```

> [!TIP]
>
> 使用习惯：
>
> - `struct` 常用于简单数据聚合。
> - `class` 常用于有封装、有行为、有不变量的对象。

## 4. 构造函数

构造函数在创建对象时自动调用，用来初始化对象。

特点：

- 函数名和类名相同。
- 没有返回值，连 `void` 也不能写。
- 创建对象时自动调用。

```cpp
class Student {
private:
    std::string name;
    int age;

public:
    Student() {
        name = "未知";
        age = 0;
    }

    Student(const std::string& n, int a) {
        name = n;
        age = a;
    }
};
```

> [!TIP]
>
> 更推荐使用初始化列表：

```cpp
class Student {
private:
    std::string name;
    int age;

public:
    Student(const std::string& n, int a)
        : name(n), age(a) {
    }
};
```

什么时候会调用构造函数：

```cpp
Student s1;              // 调用无参构造
Student s2("张三", 18);  // 调用有参构造
```

## 5. 拷贝构造函数

拷贝构造函数用已有对象创建新对象。

```cpp
class Student {
public:
    std::string name;
    int age;

    Student(const std::string& n, int a) : name(n), age(a) {}

    Student(const Student& other) {
        name = other.name;
        age = other.age;
    }
};

int main() {
    Student s1("张三", 18);
    Student s2 = s1; // 调用拷贝构造函数
}
```

拷贝构造函数的常见形式：

```cpp
ClassName(const ClassName& other);
```

> [!IMPORTANT]
>
> 拷贝构造函数的参数必须优先使用引用。如果不用引用，传参时又要拷贝一次对象，会继续调用拷贝构造函数，导致无限递归。

什么时候会调用拷贝构造：

- 用一个对象初始化另一个新对象。
- 对象按值传参。
- 函数按值返回对象时，某些情况下可能发生拷贝或移动。

## 6. 浅拷贝和深拷贝

浅拷贝只复制成员变量的值。如果成员变量里有指针，就只复制地址，两个对象会指向同一块内存。

```cpp
class Array {
public:
    int* data;

    Array(int value) {
        data = new int(value);
    }

    ~Array() {
        delete data;
    }
};
```

如果直接使用编译器生成的默认拷贝：

```cpp
Array a(10);
Array b = a; // 浅拷贝，a.data 和 b.data 指向同一块内存
```

> [!CAUTION]
>
> 浅拷贝管理堆内存时很危险：两个对象析构时可能会 `delete` 同一块内存，导致错误。

深拷贝会重新申请一块内存，再复制内容。

```cpp
class Array {
public:
    int* data;

    Array(int value) {
        data = new int(value);
    }

    Array(const Array& other) {
        data = new int(*other.data);
    }

    ~Array() {
        delete data;
    }
};
```

> [!IMPORTANT]
>
> 只要类中管理堆内存，就要认真考虑拷贝构造、赋值运算符和析构函数。

## 7. 析构函数

析构函数在对象销毁时自动调用，用来释放资源。

特点：

- 函数名是 `~类名`。
- 没有返回值。
- 没有参数。
- 一个类只能有一个析构函数。

```cpp
class File {
public:
    File() {
        std::cout << "打开资源" << std::endl;
    }

    ~File() {
        std::cout << "释放资源" << std::endl;
    }
};
```

> [!NOTE]
>
> 析构函数一般用来：
>
> - 释放 `new` 出来的内存。
> - 关闭文件。
> - 关闭网络连接。
> - 释放锁或其他系统资源。

## 8. this 指针

`this` 是成员函数内部自动存在的指针，指向当前对象。

```cpp
class Student {
private:
    std::string name;

public:
    void setName(const std::string& name) {
        this->name = name;
    }
};
```

这里左边的 `this->name` 是成员变量，右边的 `name` 是参数。

> [!NOTE]
>
> `this` 的本质：
>
> - 每个非静态成员函数都有一个隐藏的 `this` 指针参数。
> - `this` 指向调用该成员函数的对象。
> - 静态成员函数没有 `this` 指针。

## 9. 赋值运算符重载

赋值运算符用于“已有对象给已有对象赋值”。

```cpp
Student s1("张三", 18);
Student s2("李四", 20);
s2 = s1; // 调用赋值运算符
```

它和拷贝构造不同：

```cpp
Student s2 = s1; // 创建新对象，调用拷贝构造
s2 = s1;         // 两个对象都已经存在，调用赋值运算符
```

常见写法：

```cpp
class Array {
private:
    int* data;

public:
    Array(int value) {
        data = new int(value);
    }

    Array(const Array& other) {
        data = new int(*other.data);
    }

    Array& operator=(const Array& other) {
        if (this == &other) {
            return *this;
        }

        delete data;
        data = new int(*other.data);
        return *this;
    }

    ~Array() {
        delete data;
    }
};
```

> [!IMPORTANT]
>
> 赋值运算符重载需要注意：
>
> - 返回 `Array&`，方便连续赋值：`a = b = c;`
> - 要处理自赋值：`a = a;`
> - 旧资源释放后再拷贝新资源。

## 10. 特殊成员的初始化

有些成员必须在初始化列表中初始化。

例如 `const` 成员：

```cpp
class Student {
private:
    const int id;

public:
    Student(int value) : id(value) {
    }
};
```

引用成员也必须在初始化列表中初始化：

```cpp
class Holder {
private:
    int& ref;

public:
    Holder(int& value) : ref(value) {
    }
};
```

对象成员如果没有默认构造函数，也需要在初始化列表中初始化。

```cpp
class Address {
public:
    Address(const std::string& city) {
    }
};

class Student {
private:
    Address address;

public:
    Student() : address("北京") {
    }
};
```

> [!IMPORTANT]
>
> `const` 成员、引用成员、没有默认构造函数的对象成员，必须通过初始化列表初始化。
>
> 总结：能用初始化列表时，优先使用初始化列表。

## 11. static 成员

静态成员属于类，不属于某一个对象。

```cpp
class Student {
public:
    static int count;

    Student() {
        count++;
    }
};

int Student::count = 0;
```

访问静态成员：

```cpp
std::cout << Student::count << std::endl;
```

静态成员函数也属于类：

```cpp
class Student {
private:
    static int count;

public:
    static int getCount() {
        return count;
    }
};
```

静态成员函数能访问：

- 静态成员变量。
- 其他静态成员函数。

静态成员函数不能直接访问：

- 非静态成员变量。
- 非静态成员函数。

> [!CAUTION]
>
> 静态成员函数没有 `this` 指针，不知道你想访问哪个对象的普通成员，所以不能直接访问非静态成员变量和非静态成员函数。

## 12. const 成员函数

`const` 成员函数表示：这个函数不会修改当前对象的普通成员变量。

```cpp
class Student {
private:
    std::string name;

public:
    std::string getName() const {
        return name;
    }
};
```

`const` 写在函数参数列表后面：

```cpp
返回类型 函数名(参数列表) const;
```

> [!IMPORTANT]
>
> `const` 成员函数的特点：
>
> - `const` 成员函数内部不能修改普通成员变量。
> - `const` 对象只能调用 `const` 成员函数。
> - 非 `const` 对象既可以调用 `const` 成员函数，也可以调用非 `const` 成员函数。

```cpp
class Student {
public:
    void read() const {}
    void write() {}
};

int main() {
    const Student s1;
    s1.read();
    // s1.write(); // 错误

    Student s2;
    s2.read();
    s2.write();
}
```

## 13. 类对象的大小

一个对象的大小主要取决于它的非静态成员变量。

```cpp
class A {
public:
    int x;
    char y;
};
```

> [!NOTE]
>
> 对象大小还会受内存对齐影响，所以不一定等于所有成员大小的简单相加。

```cpp
std::cout << sizeof(A) << std::endl;
```

> [!TIP]
>
> 不影响普通对象大小的内容：
>
> - 成员函数：代码放在代码区，不在每个对象里复制一份。
> - 静态成员变量：属于类，不属于单个对象。
> - 静态成员函数：属于类，不属于单个对象。

空类对象大小通常是 1 字节。这样不同对象才能有不同地址。

```cpp
class Empty {};

std::cout << sizeof(Empty) << std::endl; // 通常是 1
```

## 14. 类中的 new 和 delete

使用 `new` 创建对象时，会做两件事：

1. 申请内存。
2. 调用构造函数。

```cpp
Student* p = new Student("张三", 18);
```

使用 `delete` 释放对象时，也会做两件事：

1. 调用析构函数。
2. 释放内存。

```cpp
delete p;
```

数组对象要使用 `new[]` 和 `delete[]`：

```cpp
Student* arr = new Student[3];
delete[] arr;
```

> [!IMPORTANT]
>
> 初学时要牢记：申请和释放必须成对出现。`new` 配 `delete`，`new[]` 配 `delete[]`。

## 15. 单例模式

单例模式的目标是：一个类在整个程序中只有一个对象。

实现步骤：

1. 构造函数私有化，禁止外部随便创建对象。
2. 类内部保存一个静态对象或静态指针。
3. 提供一个公共静态函数返回这个唯一对象。
4. 禁止拷贝和赋值，避免产生第二个对象。

简单写法：

```cpp
class Singleton {
private:
    Singleton() = default;

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }
};
```

使用：

```cpp
Singleton& s = Singleton::getInstance();
```

> [!TIP]
>
> 这是一种常见且简洁的写法，局部静态对象会在第一次调用时创建，程序结束时自动销毁。

## 16. std::string

`std::string` 是 C++ 标准库提供的字符串类，用来替代 C 语言中的字符数组。

使用前需要包含头文件：

```cpp
#include <string>
```

基本使用：

```cpp
#include <iostream>
#include <string>

int main() {
    std::string name = "张三";
    std::string message("hello");

    std::cout << name << std::endl;
    std::cout << message << std::endl;

    return 0;
}
```

`std::string` 的优点：

- 不需要自己管理字符数组的长度。
- 可以直接赋值、拼接和比较。
- 自动管理内部内存，比手动操作 `char*` 更安全。

常见成员函数：

```cpp
std::string s = "hello";

std::cout << s.size() << std::endl;   // 字符个数
std::cout << s.empty() << std::endl;  // 是否为空

s += " world";                        // 拼接字符串
s.append("!");                        // 继续拼接

std::cout << s[0] << std::endl;       // 访问第一个字符，不检查越界
std::cout << s.at(0) << std::endl;    // 访问第一个字符，检查越界
```

查找和截取：

```cpp
std::string s = "hello world";

std::size_t pos = s.find("world");
if (pos != std::string::npos) {
    std::cout << "找到了，位置是：" << pos << std::endl;
}

std::string part = s.substr(0, 5); // "hello"
```

修改字符串：

```cpp
std::string s = "hello";

s.insert(5, " world"); // "hello world"
s.erase(5, 1);         // 删除下标 5 开始的 1 个字符
s.replace(0, 5, "hi"); // 把下标 0 开始的 5 个字符替换为 "hi"
```

比较字符串：

```cpp
std::string a = "abc";
std::string b = "abd";

if (a < b) {
    std::cout << "a 在字典序上更小" << std::endl;
}

if (a == "abc") {
    std::cout << "内容相等" << std::endl;
}
```

和 C 风格字符串互相配合：

```cpp
std::string s = "hello";
const char* p = s.c_str();
```

> [!CAUTION]
>
> 使用 `std::string` 时要注意：
>
> - `size()` 返回的是字符数量，不包含字符串结尾的 `'\0'`。
> - `s[下标]` 越界时行为不安全，`s.at(下标)` 越界会抛出异常。
> - `std::string` 是一个类，不是普通数组，它内部会管理自己的动态内存。

## 17. vector

`vector` 是 C++ 标准库中的动态数组。

```cpp
#include <vector>

int main() {
    std::vector<int> nums;
    nums.push_back(10);
    nums.push_back(20);
    nums.push_back(30);
}
```

它的特点：

- 底层是一段连续内存。
- 支持下标访问。
- 可以自动扩容。
- 适合频繁尾部插入和随机访问。

底层结构可以理解为：

```text
begin 指向第一个元素
end 指向最后一个元素的下一个位置
capacity_end 指向已申请空间的末尾
```

常用函数：

```cpp
std::vector<int> v;

v.push_back(1);
v.push_back(2);

std::cout << v.size() << std::endl;     // 当前元素个数
std::cout << v.capacity() << std::endl; // 当前容量
std::cout << v[0] << std::endl;         // 不检查越界
std::cout << v.at(0) << std::endl;      // 检查越界
```

扩容机制：

- 当 `size()` 超过当前 `capacity()` 时，`vector` 会申请更大的连续空间。
- 然后把旧元素搬到新空间。
- 最后释放旧空间。

> [!CAUTION]
>
> `vector` 扩容可能导致原来的指针、引用、迭代器失效。

```cpp
std::vector<int> v;
v.push_back(1);

int* p = &v[0];
v.push_back(2);
v.push_back(3);
// p 可能已经失效，不能继续依赖它
```

> [!TIP]
>
> 如果提前知道大概元素个数，可以使用 `reserve` 减少扩容次数。

```cpp
std::vector<int> v;
v.reserve(100);
```

## 本章反思

第二章的重点是理解“对象不仅是数据的集合，也是行为和资源的管理者”。构造函数负责初始化，析构函数负责清理，拷贝构造和赋值运算符决定对象如何复制，`private` 和 `const` 帮助我们写出更可靠的类。

# 第三章：IO

## 1. IO 流是什么？

IO 是 Input/Output 的缩写，表示输入和输出。

> [!NOTE]
>
> C++ 把输入输出抽象成“流”。可以把流理解为一条数据通道：
>
> - 输入流：数据从外部进入程序。
> - 输出流：数据从程序输出到外部。

常见 IO 类型：

| 类型 | 作用 | 头文件 |
| --- | --- | --- |
| `std::cin` | 标准输入，通常来自键盘 | `<iostream>` |
| `std::cout` | 标准输出，通常输出到屏幕 | `<iostream>` |
| `std::cerr` | 标准错误输出，通常不缓冲 | `<iostream>` |
| `std::ifstream` | 文件输入流，用来读文件 | `<fstream>` |
| `std::ofstream` | 文件输出流，用来写文件 | `<fstream>` |
| `std::stringstream` | 字符串流，在字符串中读写数据 | `<sstream>` |

## 2. 流的四种状态

流对象内部会记录当前状态。常见状态有四种：

| 状态 | 含义 |
| --- | --- |
| `goodbit` | 表示流当前状态正常 |
| `eofbit` | 到达文件末尾 (end of file) |
| `failbit` | 读取或格式转换失败，但流通常还可以恢复，例如输入的数据类型和接收变量的类型不匹配 |
| `badbit` | 发生严重错误，流可能无法继续使用，例如文件系统错误、设备损坏等 |

可以通过成员函数判断：

```cpp
std::cin.good(); // 是否正常
std::cin.eof();  // 是否到达末尾
std::cin.fail(); // 是否发生可恢复失败
std::cin.bad();  // 是否发生严重错误
```

最常见的是 `failbit`：

```cpp
int age;
std::cin >> age;
```

如果用户输入的是 `abc`，而不是整数，`cin` 无法把它转换成 `int`，就会进入失败状态。

> [!CAUTION]
>
> 进入失败状态后，继续读取通常不会成功，需要清除状态并处理缓冲区里的错误输入：

```cpp
#include <iostream>
#include <limits>

int main() {
    int age;

    if (!(std::cin >> age)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "输入的不是整数" << std::endl;
    }

    return 0;
}
```

什么时候不会进入这些错误状态？

- 正确读取到符合类型的数据时，不会进入失败状态。
- 普通输出成功时，不会进入失败状态。
- 文件成功打开并正常读取时，不会进入失败状态。
- 读取文件时，只有真正读到末尾之后，才会设置 `eofbit`。

## 3. cout 的本质

`std::cout` 是一个标准输出流对象，类型是 `std::ostream`。

它负责把程序中的数据输出到标准输出设备，通常就是终端。

```cpp
#include <iostream>

int main() {
    int age = 18;
    std::cout << "age = " << age << std::endl;
    return 0;
}
```

`<<` 是输出运算符。它把右边的数据写入左边的输出流。

```cpp
std::cout << "hello" << " " << 123 << std::endl;
```

> [!NOTE]
>
> 这行代码可以连续输出，是因为每次 `<<` 执行后都会返回输出流本身。

`std::endl` 的作用：

- 输出一个换行。
- 刷新缓冲区。

> [!TIP]
>
> 如果只想换行，通常使用 `'\n'` 更轻量：

```cpp
std::cout << "hello\n";
```

## 4. cin 的本质

`std::cin` 是一个标准输入流对象，类型是 `std::istream`。

它负责从标准输入设备读取数据，通常就是键盘。

```cpp
#include <iostream>

int main() {
    int age;
    std::cin >> age;
    std::cout << age << std::endl;
    return 0;
}
```

`>>` 是输入运算符。它会按照变量类型读取并转换数据。

```cpp
int age;
double score;
std::string name;

std::cin >> age >> score >> name;
```

使用 `>>` 读取字符串时，会以空白字符作为分隔符。

```cpp
std::string name;
std::cin >> name;
```

如果输入：

```text
zhang san
```

`name` 只会读到 `"zhang"`。

如果要读取一整行，应该使用 `std::getline`：

```cpp
std::string line;
std::getline(std::cin, line);
```

> [!CAUTION]
>
> 如果前面用过 `std::cin >>`，再使用 `getline`，可能会读到残留的换行。常见处理方式：

```cpp
int age;
std::cin >> age;
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

std::string line;
std::getline(std::cin, line);
```

## 5. 全缓冲和行缓冲

缓冲区可以理解为一块临时存放数据的内存。

输出时，程序不一定每写一次就立刻把数据送到终端或文件，而是可能先放到缓冲区中，等合适时机再统一写出。

常见缓冲方式：

| 缓冲方式 | 特点 | 常见场景 |
| --- | --- | --- |
| 全缓冲 | 缓冲区满了才输出 | 文件输出 |
| 行缓冲 | 遇到换行时输出 | 终端输出 |
| 无缓冲 | 立即输出 | `std::cerr` 常用于错误信息 |

刷新缓冲区的方式：

```cpp
std::cout << "hello" << std::endl; // 换行并刷新
std::cout << "hello" << std::flush; // 只刷新
```

普通换行 `'\n'` 不一定强制刷新：

```cpp
std::cout << "hello\n";
```

> [!CAUTION]
>
> 频繁使用 `std::endl` 会频繁刷新缓冲区，可能影响性能。只需要换行时，优先考虑 `'\n'`。

## 6. 如何读取文件中的数据？

读取文件需要使用 `std::ifstream`，头文件是 `<fstream>`。

```cpp
#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::ifstream file("data.txt");

    if (!file.is_open()) {
        std::cout << "文件打开失败" << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }

    file.close();
    return 0;
}
```

常见读取方式：

按单词读取：

```cpp
std::ifstream file("data.txt");
std::string word;

while (file >> word) {
    std::cout << word << std::endl;
}
```

按行读取：

```cpp
std::ifstream file("data.txt");
std::string line;

while (std::getline(file, line)) {
    std::cout << line << std::endl;
}
```

读取结构化数据：

```cpp
std::ifstream file("students.txt");

std::string name;
int age;

while (file >> name >> age) {
    std::cout << name << " " << age << std::endl;
}
```

## 7. 如何向文件中写入数据？

写文件需要使用 `std::ofstream`。

```cpp
#include <fstream>
#include <iostream>

int main() {
    std::ofstream file("result.txt");

    if (!file.is_open()) {
        std::cout << "文件打开失败" << std::endl;
        return 1;
    }

    file << "hello" << std::endl;
    file << 123 << std::endl;

    file.close();
    return 0;
}
```

> [!CAUTION]
>
> 默认情况下，`std::ofstream` 会覆盖原文件内容。

如果想追加写入，可以使用 `std::ios::app`：

```cpp
std::ofstream file("result.txt", std::ios::app);
file << "追加一行内容" << std::endl;
```

常见文件打开模式：

| 模式 | 含义 |
| --- | --- |
| `std::ios::in` | 读 |
| `std::ios::out` | 写 |
| `std::ios::app` | 追加 |
| `std::ios::trunc` | 打开时清空原内容 |
| `std::ios::binary` | 二进制模式 |

## 8. stringstream 有什么作用？

`std::stringstream` 是字符串流，头文件是 `<sstream>`。

它可以把字符串当成输入输出流来处理，常用于：

- 从一行字符串中解析多个字段。
- 把数字和字符串拼接成一个字符串。
- 做简单的类型转换。

从字符串中解析数据：

```cpp
#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string line = "zhangsan 18 90.5";
    std::stringstream ss(line);

    std::string name;
    int age;
    double score;

    ss >> name >> age >> score;

    std::cout << name << " " << age << " " << score << std::endl;
    return 0;
}
```

把多个数据拼成字符串：

```cpp
std::stringstream ss;

std::string name = "张三";
int age = 18;

ss << "name=" << name << ", age=" << age;

std::string result = ss.str();
```

> [!TIP]
>
> 初学时可以把 `stringstream` 理解成：不用真实文件，也不用键盘，而是在一段字符串里进行输入输出。

## 本章反思

第三章的重点是理解“流”这个抽象：`cin` 从输入流取数据，`cout` 向输出流写数据，文件流负责读写文件，字符串流负责在字符串中解析和组织数据。学习 IO 时要特别注意流状态和缓冲区，它们是很多输入输出问题的根源。

# 第四章：日志log4cpp

## 1. 日志用途

日志就是程序运行时留下的记录。

它不是给用户看的主要功能，而是给开发者、运维人员排查问题和观察程序状态用的。

常见用途：

- 记录程序是否正常启动、结束。
- 记录关键业务流程走到了哪一步。
- 记录错误原因，例如文件打开失败、网络连接失败。
- 记录调试信息，帮助定位问题。
- 在线上环境中保留运行证据，方便事后分析。

> [!NOTE]
>
> 简单来说：程序输出给用户的是结果，程序写入日志的是过程和问题。

如果没有日志，程序出错时只能靠猜；有了日志，就可以根据时间、等级、位置和错误信息去定位问题。

日志通常会分等级：

| 等级 | 含义 |
| --- | --- |
| `DEBUG` | 调试信息，开发阶段最常用 |
| `INFO` | 普通运行信息 |
| `WARN` | 警告，程序还能继续运行 |
| `ERROR` | 错误，某个操作失败 |
| `FATAL` | 严重错误，程序可能无法继续运行 |

## 2. log4cpp 编译运行

使用 log4cpp 前，需要先安装库和头文件。

在 Ubuntu/Debian 中常见安装方式：

```bash
sudo apt install liblog4cpp5-dev
```

一个最小示例：

```cpp
#include <iostream>
#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>

int main() {
    log4cpp::OstreamAppender* appender =
        new log4cpp::OstreamAppender("console", &std::cout);

    appender->setLayout(new log4cpp::BasicLayout());

    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.addAppender(appender);
    root.setPriority(log4cpp::Priority::DEBUG);

    root.debug("debug message");
    root.info("info message");
    root.error("error message");

    log4cpp::Category::shutdown();
    return 0;
}
```

编译时要链接 log4cpp：

```bash
g++ main.cpp -o main -llog4cpp -lpthread
./main
```

> [!CAUTION]
>
> 如果头文件找不到，说明开发包没有安装好。
>
> 如果链接时报 `undefined reference`，通常是没有加 `-llog4cpp`，或者库没有安装到编译器能找到的位置。

## 3. log4cpp 核心组件

> [!NOTE]
>
> log4cpp 常见组件可以这样理解：

| 组件 | 作用 |
| --- | --- |
| `Category` | 日志分类器，负责接收日志请求，也负责设置日志等级 |
| `Appender` | 输出目的地，决定日志输出到哪里 |
| `Layout` | 输出格式，决定每条日志长什么样 |
| `Priority` | 日志等级，决定哪些日志可以被输出 |
| `Configurator` | 配置器，可以从配置文件读取日志配置 |

`Category` 是使用时最常接触的对象。

```cpp
log4cpp::Category& logger = log4cpp::Category::getInstance("server");
logger.info("server started");
```

`Appender` 决定输出位置。

常见 Appender：

| Appender | 作用 |
| --- | --- |
| `OstreamAppender` | 输出到标准输出或标准错误 |
| `FileAppender` | 输出到普通文件 |
| `RollingFileAppender` | 输出到滚动文件 |

`Layout` 决定格式。

```cpp
log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
layout->setConversionPattern("%d [%p] %c: %m%n");
```

`Priority` 决定日志等级过滤。

```cpp
logger.setPriority(log4cpp::Priority::INFO);
```

这表示 `DEBUG` 级别的日志不会输出，`INFO`、`WARN`、`ERROR`、`FATAL` 会输出。

## 4. RollingFileAppender

`RollingFileAppender` 是一种可以自动滚动日志文件的 Appender。

> [!IMPORTANT]
>
> 普通 `FileAppender` 会一直往同一个文件写日志。如果程序长期运行，日志文件可能越来越大，最后占满磁盘。

`RollingFileAppender` 可以设置单个日志文件的最大大小和备份文件数量。

示例：

```cpp
#include <log4cpp/RollingFileAppender.hh>

log4cpp::RollingFileAppender* appender =
    new log4cpp::RollingFileAppender(
        "rolling",
        "app.log",
        10 * 1024 * 1024,
        5
    );
```

含义：

- 日志写入 `app.log`。
- 单个文件最大约 10MB。
- 最多保留 5 个备份文件。

当 `app.log` 达到大小限制后，log4cpp 会把旧日志文件改名为备份文件，再重新创建新的 `app.log` 继续写。

它的作用是：

- 防止单个日志文件无限变大。
- 保留最近一段时间的日志。
- 让日志文件更容易查看、移动和清理。

## 5. 自定义输出格式

使用 `PatternLayout` 可以自定义日志格式。

```cpp
log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
layout->setConversionPattern("%d [%p] %c: %m%n");
```

常见占位符：

| 占位符 | 含义 |
| --- | --- |
| `%c` | Category 名字，也就是日志分类名称 |
| `%d` | 时间 |
| `%p` | 日志等级，例如 `INFO`、`ERROR` |
| `%m` | 日志消息内容 |
| `%n` | 换行 |

例如：

```cpp
logger.info("server started");
```

按照格式：

```text
%d [%p] %c: %m%n
```

可能输出成：

```text
2026-05-07 10:20:30 [INFO] server: server started
```

> [!TIP]
>
> 初学时最常用的格式是：时间 + 等级 + 模块名 + 日志内容。

这样排查问题时可以知道：

- 什么时候发生的。
- 严重程度是什么。
- 哪个模块输出的。
- 具体发生了什么。

## 6. 日志输出流程

日志从产生到输出，大致过程是：

1. 程序调用 `logger.info()`、`logger.error()` 等函数产生一条日志。
2. `Category` 接收到这条日志请求。
3. `Category` 根据 `Priority` 判断日志等级是否满足输出条件。
4. 如果等级太低，这条日志会被丢弃。
5. 如果等级满足条件，日志会交给绑定的 `Appender`。
6. `Appender` 使用自己的 `Layout` 把日志格式化成字符串。
7. `Appender` 把格式化后的字符串输出到目标位置，例如终端、文件或滚动文件。

> [!NOTE]
>
> 可以理解为：日志请求 -> Category 过滤 -> Layout 格式化 -> Appender 输出。

例如：

```cpp
logger.error("open file failed");
```

如果当前日志等级允许输出 `ERROR`，并且 logger 绑定了文件 Appender，那么这条日志会被格式化后写入日志文件。

## 本章反思

第四章的重点是理解日志不是简单的 `cout`，而是一套可配置的记录系统。`Category` 决定谁在写日志，`Priority` 决定哪些日志能写，`Layout` 决定日志长什么样，`Appender` 决定日志写到哪里。真正写项目时，日志可以帮助我们在程序已经运行起来之后继续观察它。

# 第五章：运算符重载

## 1. 友元

友元的作用是：让类外的函数或其他类可以访问当前类的 `private` 和 `protected` 成员。

正常情况下，类外不能直接访问私有成员：

```cpp
class Point {
private:
    int x;
    int y;
};
```

如果一个普通函数需要访问这些私有成员，可以把它声明成友元函数：

```cpp
class Point {
private:
    int x;
    int y;

public:
    Point(int xValue, int yValue) : x(xValue), y(yValue) {}

    friend void printPoint(const Point& p);
};

void printPoint(const Point& p) {
    std::cout << p.x << ", " << p.y << std::endl;
}
```

友元常见形式：

| 形式 | 含义 |
| --- | --- |
| 友元函数 | 某个普通函数可以访问类的私有成员 |
| 友元类 | 另一个类的所有成员函数都可以访问当前类的私有成员 |
| 友元成员函数 | 另一个类中的某一个成员函数可以访问当前类的私有成员 |

友元类示例：

```cpp
class Engine {
private:
    int power = 100;

    friend class Car;
};

class Car {
public:
    void show(const Engine& engine) {
        std::cout << engine.power << std::endl;
    }
};
```

> [!CAUTION]
>
> 友元不是成员函数，它只是获得了访问权限。
>
> 友元会破坏一部分封装性，不应该随便使用。它适合用在运算符重载、两个类关系非常紧密、或者必须让类外函数访问内部数据的场景。

## 2. 可重载运算符

C++ 中大多数运算符都可以重载，例如：

| 类型 | 示例 |
| --- | --- |
| 算术运算符 | `+`、`-`、`*`、`/`、`%` |
| 关系运算符 | `==`、`!=`、`<`、`>`、`<=`、`>=` |
| 逻辑运算符 | `!`、`&&`、`||` |
| 下标运算符 | `[]` |
| 函数调用运算符 | `()` |
| 成员访问相关运算符 | `->`、`->*` |
| 输入输出运算符 | `>>`、`<<` |
| 赋值和复合赋值 | `=`、`+=`、`-=`、`*=` |
| 自增自减 | `++`、`--` |
| new/delete | `new`、`delete`、`new[]`、`delete[]` |

不能重载的运算符主要有：

| 运算符 | 含义 |
| --- | --- |
| `.` | 成员访问 |
| `.*` | 成员指针访问 |
| `::` | 作用域解析 |
| `?:` | 三目运算符 |
| `sizeof` | 求大小 |
| `typeid` | 类型信息 |
| `alignof` | 对齐要求 |

> [!IMPORTANT]
>
> 运算符重载要注意：
>
> - 不能创造新的运算符。
> - 不能改变运算符的优先级。
> - 不能改变运算符的结合性。
> - 不能改变运算符需要的操作数个数。
>
> 例如 `+` 本来是双目运算符，重载后仍然只能作为双目运算符使用。

## 3. 运算符重载的本质

运算符重载的本质是函数调用。

例如：

```cpp
c = a + b;
```

如果 `a` 和 `b` 是自定义类型，编译器会把它理解成对某个函数的调用：

```cpp
c = operator+(a, b);
```

或者成员函数形式：

```cpp
c = a.operator+(b);
```

> [!NOTE]
>
> 运算符重载不是给 C++ 增加新语法，而是让已有运算符可以用于自定义类型；它的本质仍然是函数调用。

示例：

```cpp
class Point {
private:
    int x;
    int y;

public:
    Point(int xValue, int yValue) : x(xValue), y(yValue) {}

    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
};
```

调用：

```cpp
Point a(1, 2);
Point b(3, 4);
Point c = a + b;
```

这里的 `a + b` 本质就是：

```cpp
Point c = a.operator+(b);
```

## 4. 运算符重载的形式

运算符重载常见有两种形式：

- 成员函数形式。
- 全局函数形式。

成员函数形式：

```cpp
class Point {
private:
    int x;
    int y;

public:
    Point(int xValue, int yValue) : x(xValue), y(yValue) {}

    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
};
```

调用方式：

```cpp
Point c = a + b;
```

本质：

```cpp
Point c = a.operator+(b);
```

全局函数形式：

```cpp
class Point {
private:
    int x;
    int y;

public:
    Point(int xValue, int yValue) : x(xValue), y(yValue) {}

    friend Point operator+(const Point& left, const Point& right);
};

Point operator+(const Point& left, const Point& right) {
    return Point(left.x + right.x, left.y + right.y);
}
```

调用方式：

```cpp
Point c = a + b;
```

本质：

```cpp
Point c = operator+(a, b);
```

> [!TIP]
>
> 一般选择：
>
> - 左操作数必须是当前类对象时，可以写成成员函数。
> - 左操作数不是当前类对象时，通常写成全局函数。
> - `<<`、`>>` 通常写成全局函数，并配合友元访问私有成员。

例如输出运算符：

```cpp
class Point {
private:
    int x;
    int y;

public:
    Point(int xValue, int yValue) : x(xValue), y(yValue) {}

    friend std::ostream& operator<<(std::ostream& os, const Point& p);
};

std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}
```

## 5. 运算符重载的使用场景

当一个自定义类型使用某个运算符后，含义自然、清晰，并且符合使用习惯时，才适合重载运算符。

适合重载的例子：

- `Complex` 复数支持 `+`、`-`、`*`。
- `String` 字符串支持 `+` 拼接。
- `Vector` 向量支持 `+`、`-`。
- `Date` 日期支持比较。
- 智能指针支持 `*` 和 `->`。

不适合重载的例子：

```cpp
student1 + student2
```

> [!CAUTION]
>
> 如果两个学生相加没有明确含义，就不要为了“看起来高级”去重载。

不是只有内置类型可以重载。

> [!IMPORTANT]
>
> 准确地说：运算符重载至少要有一个操作数是自定义类型。

下面这种不能重载：

```cpp
int operator+(int a, int b); // 错误
```

因为 `int + int` 是内置类型之间的运算，不能改变它原本的含义。

下面这种可以：

```cpp
Point operator+(const Point& a, const Point& b);
```

因为 `Point` 是自定义类型。

## 6. 运算符重载的注意点

重载运算符时要考虑语义是否自然。

例如 `+` 一般表示不改变原对象，返回一个新对象：

```cpp
Point c = a + b;
```

`+=` 一般表示修改左操作数：

```cpp
a += b;
```

> [!IMPORTANT]
>
> 运算符重载常见注意点：
>
> - 返回值类型是否合适。
> - 参数是否应该使用 `const` 引用，避免不必要拷贝。
> - 成员函数是否应该加 `const`。
> - 是否需要支持连续调用。
> - 是否需要处理自赋值。
> - 是否和已有运算符语义保持一致。
> - 是否破坏封装。
> - 是否会产生资源泄漏。

例如赋值运算符通常返回当前对象引用：

```cpp
Array& operator=(const Array& other) {
    if (this == &other) {
        return *this;
    }

    delete data;
    data = new int(*other.data);
    return *this;
}
```

返回引用的原因是支持连续赋值：

```cpp
a = b = c;
```

输入输出运算符也要返回流引用：

```cpp
std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << p.x << ", " << p.y;
    return os;
}
```

这样才能连续输出：

```cpp
std::cout << p1 << p2 << std::endl;
```

## 7. 资源自动释放

> [!IMPORTANT]
>
> 用一个类去管理资源，并在析构函数中释放资源，这就是 RAII 思想。

RAII 的意思是：资源获取即初始化。

简单理解：

```text
对象创建时获取资源
对象销毁时释放资源
```

例如用类管理堆内存：

```cpp
class IntHolder {
private:
    int* data;

public:
    IntHolder(int value) {
        data = new int(value);
    }

    ~IntHolder() {
        delete data;
    }
};
```

使用：

```cpp
void test() {
    IntHolder holder(10);
} // holder 离开作用域，自动调用析构函数，释放 data
```

原理是：局部对象离开作用域时会自动析构，成员对象也会随着外层对象自动析构。

这种方式可以管理：

- 堆内存。
- 文件句柄。
- 网络连接。
- 互斥锁。
- 数据库连接。

如果类中管理了资源，还要考虑拷贝控制。

最少要考虑：

- 析构函数。
- 拷贝构造函数。
- 赋值运算符。

这就是常说的“三法则”。

> [!TIP]
>
> 更现代的 C++ 中，优先使用标准库资源管理类型：

```cpp
std::unique_ptr<int> p = std::make_unique<int>(10);
```

这样可以少写很多手动 `new/delete`。

## 8. 函数指针

函数指针保存的是函数的地址。

普通函数：

```cpp
int add(int a, int b) {
    return a + b;
}
```

函数指针语法：

```cpp
int (*p)(int, int) = add;
```

含义：

```text
p 是一个指针
它指向的函数返回 int
它指向的函数有两个 int 参数
```

调用方式：

```cpp
int result1 = p(3, 4);
int result2 = (*p)(3, 4);
```

> [!TIP]
>
> 函数指针调用时，`p(3, 4)` 和 `(*p)(3, 4)` 两种写法都可以，第一种更常见。

函数指针常用于回调：

```cpp
void calculate(int a, int b, int (*func)(int, int)) {
    std::cout << func(a, b) << std::endl;
}

int main() {
    calculate(3, 4, add);
    return 0;
}
```

可以用 `using` 简化类型：

```cpp
using Func = int (*)(int, int);

Func p = add;
```

## 9. 成员函数指针

成员函数指针保存的是类中成员函数的地址。

它和普通函数指针不一样，因为非静态成员函数需要对象才能调用。

示例：

```cpp
class Calculator {
public:
    int add(int a, int b) {
        return a + b;
    }
};
```

成员函数指针语法：

```cpp
int (Calculator::*p)(int, int) = &Calculator::add;
```

含义：

```text
p 是一个指向 Calculator 成员函数的指针
这个成员函数返回 int
这个成员函数有两个 int 参数
```

第一种调用方式：通过对象调用。

```cpp
Calculator calc;
int result = (calc.*p)(3, 4);
```

第二种调用方式：通过对象指针调用。

```cpp
Calculator* ptr = &calc;
int result = (ptr->*p)(3, 4);
```

> [!CAUTION]
>
> 成员函数指针调用时，括号不能省略：

```cpp
(calc.*p)(3, 4);
(ptr->*p)(3, 4);
```

因为成员函数指针的调用语法优先级比较特殊。

如果是静态成员函数，它不依赖对象，可以当普通函数指针使用。

## 10. 函数对象和可调用实体

函数对象是重载了 `operator()` 的对象。

也叫仿函数。

示例：

```cpp
class Add {
public:
    int operator()(int a, int b) const {
        return a + b;
    }
};

int main() {
    Add add;
    std::cout << add(3, 4) << std::endl;
}
```

> [!NOTE]
>
> 虽然 `add` 是对象，但它可以像函数一样调用，因为它重载了函数调用运算符 `()`。

常见可调用实体：

| 可调用实体 | 示例 |
| --- | --- |
| 普通函数 | `add(1, 2)` |
| 函数指针 | `p(1, 2)` |
| 成员函数指针 | `(obj.*p)(1, 2)` |
| 函数对象 | `func(1, 2)` |
| Lambda 表达式 | `[](int x) { return x * x; }` |
| `std::function` | `std::function<int(int)> f` |

Lambda 示例：

```cpp
auto square = [](int x) {
    return x * x;
};

std::cout << square(5) << std::endl;
```

`std::function` 可以保存多种可调用对象：

```cpp
#include <functional>

std::function<int(int, int)> func = add;
std::cout << func(3, 4) << std::endl;
```

## 11. 嵌套类访问规则

嵌套类就是定义在另一个类内部的类。

```cpp
class Outer {
public:
    class Inner {
    public:
        void show() {
            std::cout << "Inner" << std::endl;
        }
    };
};
```

类外访问内部类，要看内部类本身的访问权限。

如果内部类是 `public`，类外可以这样使用：

```cpp
Outer::Inner obj;
obj.show();
```

如果内部类是 `private`，类外不能直接使用：

```cpp
class Outer {
private:
    class Inner {};
};

// Outer::Inner obj; // 错误
```

三种访问关系：

| 关系 | 规则 |
| --- | --- |
| 外部类外部 -> 内部类 | 只能访问 `public` 嵌套类 |
| 外部类 -> 内部类 | 外部类可以使用内部类类型名，但访问内部类成员仍然看权限 |
| 内部类 -> 外部类 | 内部类不会自动拥有外部类对象，访问外部类非静态成员需要外部类对象 |

> [!CAUTION]
>
> 内部类访问外部类成员时要注意：嵌套关系不等于对象包含关系。

```cpp
class Outer {
private:
    int value = 10;

public:
    class Inner {
    public:
        void show(Outer& outer) {
            std::cout << outer.value << std::endl;
        }
    };
};
```

这里 `Inner` 要通过 `Outer& outer` 才能访问某一个外部类对象的 `value`。

如果访问的是外部类静态成员，就不需要对象：

```cpp
class Outer {
private:
    static int count;

public:
    class Inner {
    public:
        void show() {
            std::cout << Outer::count << std::endl;
        }
    };
};
```

> [!IMPORTANT]
>
> 嵌套类只是名字定义在另一个类里面，主要影响作用域和访问权限，不代表内部类对象自动属于某个外部类对象。

## 12. 单例自动释放

单例模式要求一个类在程序中只创建一个对象。

最简单、最推荐的自动释放方式是使用局部静态对象：

```cpp
class Singleton {
private:
    Singleton() = default;
    ~Singleton() = default;

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }
};
```

使用：

```cpp
Singleton& s = Singleton::getInstance();
```

> [!TIP]
>
> 局部静态对象写法的特点：
>
> - 第一次调用 `getInstance()` 时创建对象。
> - 程序结束时自动调用析构函数。
> - 不需要手动 `delete`。
> - C++11 以后局部静态变量初始化是线程安全的。

如果使用指针方式，也可以用内部类或智能指针帮助自动释放。

指针方式示例：

```cpp
class Singleton {
private:
    Singleton() = default;

    static Singleton* instance;

    class AutoRelease {
    public:
        ~AutoRelease() {
            delete instance;
            instance = nullptr;
        }
    };

    static AutoRelease autoRelease;

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static Singleton* getInstance() {
        if (instance == nullptr) {
            instance = new Singleton;
        }

        return instance;
    }
};

Singleton* Singleton::instance = nullptr;
Singleton::AutoRelease Singleton::autoRelease;
```

原理是：`autoRelease` 是静态对象，程序结束时它会自动析构，在它的析构函数中释放单例指针。

> [!IMPORTANT]
>
> 实际项目中更推荐局部静态对象写法，因为它更简单，也更不容易漏释放。

## 13. 懒加载和立即加载

懒加载是指：对象、资源或数据不在一开始就创建，而是在第一次真正需要使用时才创建。

例如单例中的局部静态对象就是一种懒加载：

```cpp
class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }
};
```

这里 `instance` 不会在程序启动时马上创建，而是在第一次调用 `getInstance()` 时才创建。

立即加载是指：对象、资源或数据在程序启动、对象构造或初始化阶段就提前创建好。

```cpp
class Config {
public:
    Config() {
        loadFile();
    }

    void loadFile() {
        std::cout << "load config" << std::endl;
    }
};
```

这里 `Config` 对象一创建，就会立刻执行 `loadFile()`。

两者对比：

| 方式 | 创建时机 | 优点 | 缺点 |
| --- | --- | --- | --- |
| 懒加载 | 第一次使用时 | 节省启动开销，避免创建暂时不用的资源 | 第一次使用时可能变慢，需要考虑线程安全 |
| 立即加载 | 初始化阶段 | 使用时已经准备好，逻辑直接 | 启动开销可能变大，可能提前创建了不用的资源 |

> [!IMPORTANT]
>
> 懒加载的核心思想是“用到时再创建”，立即加载的核心思想是“先准备好再使用”。

## 14. std::string 的写时复制原理

写时复制也叫 COW（Copy On Write），意思是：拷贝对象时先不真正复制底层数据，而是让多个对象共享同一份数据；只有当其中一个对象要修改数据时，才真正复制一份新的数据。

早期某些 `std::string` 实现可能使用写时复制：

```cpp
std::string s1 = "hello";
std::string s2 = s1;     // 先共享同一块字符串数据

s2[0] = 'H';             // s2 要修改时，才复制一份自己的数据
```

写时复制的大致过程：

| 步骤 | 行为 |
| --- | --- |
| 创建字符串 | 分配一块字符缓冲区，并记录引用计数 |
| 拷贝字符串 | 不复制字符内容，只让引用计数加一 |
| 读取字符串 | 多个对象可以共享同一份字符数据 |
| 修改字符串 | 如果引用计数大于一，先深拷贝一份，再修改自己的副本 |
| 销毁字符串 | 引用计数减一，减到零时释放底层缓冲区 |

核心思想是：把真正的深拷贝延迟到写操作发生时。

这样做的好处是普通拷贝很快，因为只需要共享指针和增加引用计数；缺点是实现更复杂，修改时需要判断是否共享，还要处理线程安全、引用和迭代器失效等问题。

> [!CAUTION]
>
> 现代 C++ 标准库中的 `std::string` 通常不再使用写时复制，而是更多依赖短字符串优化、移动语义等机制。学习写时复制时，重点掌握它的设计思想：读共享，写拷贝。

## 本章反思

第五章的重点是理解“语法背后仍然是函数调用”。运算符重载让自定义类型更像内置类型，但前提是语义要自然。友元可以解决类外函数访问私有成员的问题，但要控制使用范围。函数指针、成员函数指针、函数对象和 Lambda 都属于可调用实体，它们让函数也可以像数据一样被传递和保存。RAII 和单例自动释放则强调：资源应该交给对象生命周期去管理，而不是依赖手动记忆释放。懒加载和立即加载体现的是资源创建时机的选择，写时复制体现的是“读共享，写拷贝”的延迟深拷贝思想。

# 第六章：关联式容器

## 1. set 底层结构

`set` 是 C++ 标准库中的关联式容器，头文件是 `<set>`。

它的特点是：存储一组不重复的元素，并且会按照一定规则自动排序。

```cpp
#include <set>

int main() {
    std::set<int> nums;

    nums.insert(3);
    nums.insert(1);
    nums.insert(2);
    nums.insert(1);
}
```

上面插入了两次 `1`，但 `set` 中只会保留一个。

`set` 的底层通常使用平衡二叉搜索树实现，常见实现是红黑树。

可以这样理解：

```text
普通数组/vector：按插入顺序存放
set：按排序规则组织元素
```

因此 `set` 的查找、插入、删除效率通常是：

```text
O(log n)
```

> [!NOTE]
>
> 标准只规定了 `set` 的行为和复杂度要求，并不强制规定一定要用红黑树实现。但实际标准库中通常是红黑树。

## 2. set 元素特点

`set` 中存储的数据元素有两个重要特点：

- 元素不重复。
- 元素自动有序。

默认情况下，`set` 使用 `<` 比较元素大小。

```cpp
std::set<int> nums = {3, 1, 2};

for (int num : nums) {
    std::cout << num << std::endl;
}
```

输出顺序是：

```text
1
2
3
```

如果想修改排序规则，可以指定比较器。

```cpp
std::set<int, std::greater<int>> nums = {3, 1, 2};
```

这时遍历顺序是从大到小。

对于自定义类型，如果想放入 `set`，需要让它可以比较大小。

```cpp
class Student {
public:
    std::string name;
    int score;

    Student(const std::string& n, int s) : name(n), score(s) {}

    bool operator<(const Student& other) const {
        return score < other.score;
    }
};
```

> [!CAUTION]
>
> `set` 中的元素不能直接修改。原因是元素的位置由排序规则决定，如果直接修改元素，可能破坏树的有序结构。

> [!TIP]
>
> 如果确实要修改 `set` 中的元素，一般做法是：
>
> 1. 先删除旧元素。
> 2. 再插入新元素。

## 3. set 增删遍历

插入元素使用 `insert`。

```cpp
std::set<int> nums;

nums.insert(10);
nums.insert(20);
nums.insert(10);
```

> [!TIP]
>
> `insert` 的返回值可以判断是否插入成功。

```cpp
auto result = nums.insert(10);

if (result.second) {
    std::cout << "插入成功" << std::endl;
} else {
    std::cout << "元素已存在" << std::endl;
}
```

查找元素使用 `find`。

```cpp
auto it = nums.find(20);

if (it != nums.end()) {
    std::cout << "找到了" << std::endl;
}
```

判断元素是否存在也可以使用 `count`。

```cpp
if (nums.count(20) > 0) {
    std::cout << "存在" << std::endl;
}
```

> [!NOTE]
>
> 因为 `set` 不允许重复元素，所以 `count` 的结果只可能是 `0` 或 `1`。

删除元素使用 `erase`。

```cpp
nums.erase(20);
```

也可以通过迭代器删除。

```cpp
auto it = nums.find(10);

if (it != nums.end()) {
    nums.erase(it);
}
```

遍历 `set`：

```cpp
for (const auto& num : nums) {
    std::cout << num << std::endl;
}
```

也可以使用迭代器：

```cpp
for (auto it = nums.begin(); it != nums.end(); ++it) {
    std::cout << *it << std::endl;
}
```

> [!IMPORTANT]
>
> `set` 的遍历顺序就是排序后的顺序，不是插入顺序。

## 4. set 应用场景

`set` 适合用在需要“去重”和“有序”的场景。

常见用途：

- 保存不重复的数据。
- 判断某个元素是否出现过。
- 自动维护有序集合。
- 做交集、并集、差集等集合操作。

例如统计一组数字中出现过哪些不同的数字：

```cpp
std::vector<int> nums = {3, 1, 2, 3, 2, 5};
std::set<int> uniqueNums;

for (int num : nums) {
    uniqueNums.insert(num);
}

for (int num : uniqueNums) {
    std::cout << num << std::endl;
}
```

> [!TIP]
>
> 如果只关心是否存在，并且不需要有序，也可以考虑 `unordered_set`。

```cpp
#include <unordered_set>
```

简单选择：

| 容器 | 特点 |
| --- | --- |
| `set` | 有序、不重复、查找插入删除通常是 `O(log n)` |
| `unordered_set` | 无序、不重复、平均查找插入删除是 `O(1)` |

## 5. map 底层结构

`map` 是 C++ 标准库中的关联式容器，头文件是 `<map>`。

它保存的是键值对，也就是 `key-value`。

```cpp
#include <map>

int main() {
    std::map<std::string, int> scores;

    scores["zhangsan"] = 90;
    scores["lisi"] = 85;
}
```

可以理解为：

```text
key：用来查找的名字
value：这个名字对应的数据
```

`map` 的底层通常也是平衡二叉搜索树，常见实现是红黑树。

它会按照 `key` 自动排序。

```cpp
std::map<int, std::string> data;

data[3] = "three";
data[1] = "one";
data[2] = "two";
```

遍历时会按 `key` 从小到大输出。

`map` 的查找、插入、删除效率通常也是：

```text
O(log n)
```

## 6. map 元素特点

> [!NOTE]
>
> `map` 中存储的每个元素本质上是一个 `pair`。

```cpp
std::pair<const Key, Value>
```

例如：

```cpp
std::map<std::string, int> scores;
```

其中每个元素可以理解为：

```cpp
std::pair<const std::string, int>
```

`first` 是键，`second` 是值。

```cpp
for (const auto& item : scores) {
    std::cout << item.first << " " << item.second << std::endl;
}
```

`map` 的元素特点：

- `key` 唯一。
- 根据 `key` 自动排序。
- `value` 可以重复。
- `key` 不能直接修改。
- 可以通过 `key` 快速找到对应的 `value`。

例如：

```cpp
std::map<std::string, int> scores;

scores["zhangsan"] = 90;
scores["lisi"] = 90;
```

> [!IMPORTANT]
>
> 两个学生的分数可以相同，因为 `value` 可以重复；但两个 `key` 不能相同。

## 7. map 增删遍历

插入或修改元素最常见的方式是使用 `[]`。

```cpp
std::map<std::string, int> scores;

scores["zhangsan"] = 90;
scores["lisi"] = 85;
scores["zhangsan"] = 95;
```

> [!NOTE]
>
> 如果 `key` 不存在，`[]` 会创建一个新元素。
>
> 如果 `key` 已经存在，`[]` 会修改对应的 `value`。

插入也可以使用 `insert`。

```cpp
scores.insert(std::make_pair("wangwu", 88));
```

也可以这样写：

```cpp
scores.insert({"zhaoliu", 92});
```

> [!CAUTION]
>
> 如果 `key` 已经存在，`insert` 不会覆盖原来的值。

查找元素使用 `find`。

```cpp
auto it = scores.find("zhangsan");

if (it != scores.end()) {
    std::cout << it->first << " " << it->second << std::endl;
}
```

判断是否存在可以使用 `count`。

```cpp
if (scores.count("lisi") > 0) {
    std::cout << "存在" << std::endl;
}
```

> [!NOTE]
>
> 因为 `map` 中的 `key` 唯一，所以 `count` 的结果只可能是 `0` 或 `1`。

删除元素使用 `erase`。

```cpp
scores.erase("lisi");
```

也可以通过迭代器删除。

```cpp
auto it = scores.find("zhangsan");

if (it != scores.end()) {
    scores.erase(it);
}
```

遍历 `map`：

```cpp
for (const auto& item : scores) {
    std::cout << item.first << " " << item.second << std::endl;
}
```

C++17 以后可以使用结构化绑定：

```cpp
for (const auto& [name, score] : scores) {
    std::cout << name << " " << score << std::endl;
}
```

遍历顺序是按照 `key` 排序后的顺序。

> [!CAUTION]
>
> 使用 `[]` 时要小心：

```cpp
std::cout << scores["unknown"] << std::endl;
```

如果 `"unknown"` 不存在，这行代码会插入一个新元素，值会被默认初始化为 `0`。

> [!IMPORTANT]
>
> 如果只是查找，不想插入新元素，应该使用 `find`。

## 8. map 应用场景

`map` 适合用在需要根据一个键查找另一个值的场景。

常见用途：

- 统计词频。
- 根据名字查找分数、年龄、编号等信息。
- 保存配置项。
- 建立编号和对象之间的映射关系。
- 需要按照 `key` 有序遍历数据。

词频统计示例：

```cpp
std::vector<std::string> words = {"cpp", "java", "cpp", "python", "cpp"};
std::map<std::string, int> count;

for (const auto& word : words) {
    ++count[word];
}

for (const auto& item : count) {
    std::cout << item.first << " " << item.second << std::endl;
}
```

配置项示例：

```cpp
std::map<std::string, std::string> config;

config["host"] = "127.0.0.1";
config["port"] = "8080";
config["mode"] = "debug";
```

> [!TIP]
>
> 如果不需要按 `key` 排序，只需要快速查找，可以考虑 `unordered_map`。

| 容器 | 特点 |
| --- | --- |
| `map` | 有序、`key` 唯一、查找插入删除通常是 `O(log n)` |
| `unordered_map` | 无序、`key` 唯一、平均查找插入删除是 `O(1)` |

## 本章反思

第六章的重点是理解关联式容器的核心是“通过规则组织数据”。`set` 关心元素本身，适合去重和维护有序集合；`map` 关心键值关系，适合通过 `key` 查找 `value`。它们和 `vector` 最大的区别不是语法，而是底层组织方式不同：`vector` 更像连续表，`set` 和 `map` 更像按顺序维护的搜索结构。使用时要特别注意 `set` 元素和 `map` 的 `key` 不能随意修改，以及 `map` 的 `[]` 在查找不存在的 `key` 时会自动插入新元素。

# 第七章：继承

## 1. 继承的意义

继承是面向对象中的一种代码复用和类型扩展机制。

可以把一个已有类作为基础类，再在它的基础上定义新的类。

```cpp
class Person {
public:
    std::string name;

    void eat() {
        std::cout << name << " is eating" << std::endl;
    }
};

class Student : public Person {
public:
    int score;

    void study() {
        std::cout << name << " is studying" << std::endl;
    }
};
```

这里 `Person` 是基类，也叫父类。

`Student` 是派生类，也叫子类。

继承表达的是一种 `is-a` 关系：

```text
Student is a Person
```

也就是说，学生是一种人。

继承的主要意义：

- 复用已有类中的数据和行为。
- 在已有类型基础上扩展新功能。
- 建立类型之间的层次关系。
- 为多态打基础。

> [!IMPORTANT]
>
> 继承不是为了“少写代码”而滥用。只有当两个类型之间确实存在 `is-a` 关系时，继承才比较自然。

## 2. 单继承和多继承

C++ 既支持单继承，也支持多继承。

单继承是指一个派生类只有一个直接基类。

```cpp
class Person {};

class Student : public Person {};
```

多继承是指一个派生类可以同时继承多个直接基类。

```cpp
class Printer {};
class Scanner {};

class AllInOne : public Printer, public Scanner {};
```

可以这样理解：

| 继承形式 | 含义 |
| --- | --- |
| 单继承 | 一个子类只继承一个父类 |
| 多继承 | 一个子类同时继承多个父类 |

> [!CAUTION]
>
> 多继承表达能力更强，但也更容易带来名字冲突、对象布局复杂、菱形继承等问题。初学时先把单继承掌握清楚。

## 3. 继承方式

C++ 中继承方式有三种：

- `public` 继承。
- `protected` 继承。
- `private` 继承。

语法：

```cpp
class Derived : public Base {};
class Derived : protected Base {};
class Derived : private Base {};
```

不同继承方式会影响基类成员在派生类中的访问权限。

| 基类成员权限 | `public` 继承后 | `protected` 继承后 | `private` 继承后 |
| --- | --- | --- | --- |
| `public` | 派生类中仍是 `public` | 派生类中变成 `protected` | 派生类中变成 `private` |
| `protected` | 派生类中仍是 `protected` | 派生类中仍是 `protected` | 派生类中变成 `private` |
| `private` | 派生类不可直接访问 | 派生类不可直接访问 | 派生类不可直接访问 |

示例：

```cpp
class Base {
public:
    int a;

protected:
    int b;

private:
    int c;
};

class Derived : public Base {
public:
    void test() {
        a = 1; // 可以访问
        b = 2; // 可以访问
        // c = 3; // 错误，基类 private 成员不可直接访问
    }
};
```

> [!IMPORTANT]
>
> 基类的 `private` 成员会被派生类对象继承下来，占对象空间，但派生类成员函数不能直接访问它。

实际项目中最常见的是 `public` 继承，因为它最符合 `is-a` 关系。

```cpp
class Student : public Person {};
```

## 4. 单继承对象大小

单继承下，子类对象的大小通常和这些内容有关：

- 基类的非静态成员变量。
- 派生类自己的非静态成员变量。
- 内存对齐。
- 如果类有虚函数，还可能包含虚函数表指针。

示例：

```cpp
class Base {
public:
    int a;
};

class Derived : public Base {
public:
    int b;
};

int main() {
    std::cout << sizeof(Base) << std::endl;
    std::cout << sizeof(Derived) << std::endl;
}
```

`Derived` 对象中包含一份 `Base` 子对象。

可以理解为：

```text
Derived 对象 =
  Base 部分
  Derived 自己新增的部分
```

> [!NOTE]
>
> 成员函数不存放在每个对象内部，所以普通成员函数不会增加对象大小。对象大小主要看非静态成员变量、对齐和虚函数相关结构。

## 5. 单继承构造析构

创建派生类对象时，会先构造基类部分，再构造派生类自己的部分。

销毁派生类对象时，顺序相反。

```cpp
class Base {
public:
    Base() {
        std::cout << "Base()" << std::endl;
    }

    ~Base() {
        std::cout << "~Base()" << std::endl;
    }
};

class Derived : public Base {
public:
    Derived() {
        std::cout << "Derived()" << std::endl;
    }

    ~Derived() {
        std::cout << "~Derived()" << std::endl;
    }
};

int main() {
    Derived d;
}
```

输出顺序：

```text
Base()
Derived()
~Derived()
~Base()
```

如果基类没有默认构造函数，派生类必须在初始化列表中显式调用基类构造函数。

```cpp
class Base {
public:
    Base(int value) {}
};

class Derived : public Base {
public:
    Derived() : Base(10) {}
};
```

> [!IMPORTANT]
>
> 构造顺序是：先基类，后派生类。析构顺序是：先派生类，后基类。

## 6. 隐藏和 override

派生类中如果定义了和基类同名的成员函数，会隐藏基类中的同名函数。

```cpp
class Base {
public:
    void show() {
        std::cout << "Base::show" << std::endl;
    }

    void show(int value) {
        std::cout << value << std::endl;
    }
};

class Derived : public Base {
public:
    void show() {
        std::cout << "Derived::show" << std::endl;
    }
};
```

使用：

```cpp
Derived d;
d.show();
// d.show(10); // 错误，Base::show(int) 被同名函数隐藏了
```

如果想调用被隐藏的基类函数，可以加作用域。

```cpp
d.Base::show(10);
```

也可以用 `using` 把基类同名函数引入派生类作用域。

```cpp
class Derived : public Base {
public:
    using Base::show;

    void show() {
        std::cout << "Derived::show" << std::endl;
    }
};
```

`override` 用在虚函数重写中，表示派生类函数确实要重写基类虚函数。

```cpp
class Base {
public:
    virtual void run() {
        std::cout << "Base::run" << std::endl;
    }
};

class Derived : public Base {
public:
    void run() override {
        std::cout << "Derived::run" << std::endl;
    }
};
```

> [!CAUTION]
>
> “同名隐藏”和“虚函数重写”不是一回事。隐藏看名字，重写要求基类函数是虚函数，并且函数签名匹配。

> [!TIP]
>
> 重写虚函数时建议写 `override`。如果函数签名写错，编译器会直接报错，能避免很多隐蔽问题。

## 7. 多继承对象大小

多继承下，派生类对象中会包含多个基类子对象。

```cpp
class A {
public:
    int a;
};

class B {
public:
    int b;
};

class C : public A, public B {
public:
    int c;
};
```

`C` 对象可以理解为：

```text
C 对象 =
  A 子对象
  B 子对象
  C 自己新增的部分
```

因此，`C` 的大小通常和这些内容有关：

- 所有直接基类子对象的大小。
- 派生类自己的非静态成员变量。
- 内存对齐。
- 虚函数、虚继承等额外结构。

> [!NOTE]
>
> 多继承对象中，每个直接基类通常都有自己独立的一段子对象空间。

## 8. 多继承构造析构

多继承下，基类构造顺序和继承列表中的声明顺序有关，而不是初始化列表中的书写顺序。

```cpp
class A {
public:
    A() {
        std::cout << "A()" << std::endl;
    }
};

class B {
public:
    B() {
        std::cout << "B()" << std::endl;
    }
};

class C : public A, public B {
public:
    C() : B(), A() {
        std::cout << "C()" << std::endl;
    }
};
```

虽然初始化列表中写的是 `B(), A()`，但真正构造顺序仍然是：

```text
A()
B()
C()
```

销毁顺序相反：

```text
~C()
~B()
~A()
```

> [!IMPORTANT]
>
> 多继承构造顺序由继承列表决定。`class C : public A, public B` 中，一定先构造 `A`，再构造 `B`，最后构造 `C` 自己。

## 9. 菱形继承

菱形继承是多继承中的一个典型问题。

结构如下：

```text
    A
   / \
  B   C
   \ /
    D
```

示例：

```cpp
class A {
public:
    int value;
};

class B : public A {};
class C : public A {};
class D : public B, public C {};
```

这时 `D` 中会有两份 `A` 子对象：

```text
D =
  B 中的一份 A
  C 中的一份 A
```

访问 `value` 会产生二义性。

```cpp
D d;
// d.value = 10; // 错误，不知道访问 B::A::value 还是 C::A::value
```

可以用作用域指定：

```cpp
d.B::value = 10;
d.C::value = 20;
```

但更常见的解决方式是虚继承。

```cpp
class A {
public:
    int value;
};

class B : virtual public A {};
class C : virtual public A {};
class D : public B, public C {};
```

虚继承会让最终派生类中只保留一份公共基类 `A` 子对象。

> [!IMPORTANT]
>
> 菱形继承的问题是公共基类被重复继承，导致数据冗余和访问二义性。虚继承可以让公共基类只保留一份。

## 10. 父子类型转换

父子类型之间发生转换时，转换的不是对象的“真实类型”，而是访问对象的视角。

派生类对象可以当作基类对象使用，这是向上转型。

```cpp
class Base {
public:
    int a;
};

class Derived : public Base {
public:
    int b;
};

int main() {
    Derived d;

    Base* p = &d;
    Base& r = d;
}
```

这里 `p` 和 `r` 只能通过 `Base` 的视角访问对象。

```cpp
p->a = 10;
// p->b = 20; // 错误，Base* 看不到 Derived 新增成员
```

如果按值转换，会发生对象切片。

```cpp
Derived d;
Base b = d;
```

这时只会拷贝 `Base` 部分，`Derived` 新增的部分会被切掉。

> [!CAUTION]
>
> 指针和引用的向上转型不会切对象；按值赋给基类对象会发生对象切片。

## 11. 向上转型和向下转型

向上转型是把派生类指针或引用转换成基类指针或引用。

```cpp
Derived d;

Base* p = &d;
Base& r = d;
```

向上转型通常是安全的，可以隐式完成。

向下转型是把基类指针或引用转换成派生类指针或引用。

```cpp
Base* p = new Derived;

Derived* d = static_cast<Derived*>(p);
```

向下转型需要额外语法，常见有两种：

| 方式 | 说明 |
| --- | --- |
| `static_cast` | 编译期转换，不做运行时类型检查 |
| `dynamic_cast` | 运行时检查，要求基类有虚函数 |

示例：

```cpp
class Base {
public:
    virtual ~Base() = default;
};

class Derived : public Base {};

int main() {
    Base* p = new Derived;

    Derived* d = dynamic_cast<Derived*>(p);
    if (d != nullptr) {
        std::cout << "转换成功" << std::endl;
    }

    delete p;
}
```

> [!IMPORTANT]
>
> 向上转型一般安全，向下转型需要确认对象真实类型。需要运行时检查时，优先考虑 `dynamic_cast`。

## 12. 向下转型成功条件

向下转型能否成功，关键看基类指针或引用实际指向的对象到底是不是目标派生类型。

可以成功的情况：

```cpp
Base* p = new Derived;

Derived* d = dynamic_cast<Derived*>(p);
```

因为 `p` 实际指向的是 `Derived` 对象。

不能成功的情况：

```cpp
Base* p = new Base;

Derived* d = dynamic_cast<Derived*>(p);
```

因为 `p` 实际指向的是 `Base` 对象，不是 `Derived` 对象。

`dynamic_cast` 转换指针失败时，会返回 `nullptr`。

```cpp
if (d == nullptr) {
    std::cout << "转换失败" << std::endl;
}
```

`dynamic_cast` 转换引用失败时，会抛出 `std::bad_cast` 异常。

```cpp
try {
    Derived& r = dynamic_cast<Derived&>(*p);
} catch (const std::bad_cast& e) {
    std::cout << "转换失败" << std::endl;
}
```

> [!CAUTION]
>
> `static_cast` 向下转型不会检查真实对象类型。如果判断错了，后续访问派生类成员可能产生未定义行为。

用自己的话描述：

```text
向下转型不是把父类对象变成子类对象，
而是确认一个“看起来像父类的指针/引用”，背后实际是不是某个子类对象。
```

## 本章反思

第七章的重点是理解继承带来的“类型层次”和“对象内部包含关系”。派生类对象中包含基类子对象，构造时先构造基类，析构时先析构派生类。`public` 继承最符合 `is-a` 关系，名字隐藏和虚函数重写要区分清楚。多继承会让对象布局和构造顺序变复杂，菱形继承需要用虚继承解决公共基类重复的问题。父子类型转换时要特别注意：向上转型通常安全，向下转型必须确认对象真实类型，按值转换还会发生对象切片。

# 第八章：多态

## 1. 多态

多态的字面意思是：同一个接口，在不同对象上表现出不同的行为。

在 C++ 中，多态常见有两类：

- 编译时多态：函数重载、运算符重载、模板等。
- 运行时多态：继承 + 虚函数 + 基类指针或引用。

本章重点是运行时多态。

示例：

```cpp
class Animal {
public:
    virtual void speak() {
        std::cout << "Animal speak" << std::endl;
    }
};

class Dog : public Animal {
public:
    void speak() override {
        std::cout << "Dog bark" << std::endl;
    }
};

class Cat : public Animal {
public:
    void speak() override {
        std::cout << "Cat meow" << std::endl;
    }
};

void makeSpeak(Animal& animal) {
    animal.speak();
}

int main() {
    Dog dog;
    Cat cat;

    makeSpeak(dog);
    makeSpeak(cat);
}
```

输出：

```text
Dog bark
Cat meow
```

同一个函数 `makeSpeak`，传入不同对象时，实际调用的函数不同，这就是多态。

> [!NOTE]
>
> 多态的核心不是“函数名一样”，而是“用基类接口操作对象时，运行时根据真实对象类型执行不同的行为”。

## 2. 多态产生条件

运行时多态通常需要三个条件：

1. 有继承关系。
2. 基类中有虚函数，派生类重写虚函数。
3. 通过基类指针或基类引用调用虚函数。

示例：

```cpp
class Base {
public:
    virtual void show() {
        std::cout << "Base::show" << std::endl;
    }
};

class Derived : public Base {
public:
    void show() override {
        std::cout << "Derived::show" << std::endl;
    }
};

int main() {
    Derived d;

    Base* p = &d;
    p->show();

    Base& r = d;
    r.show();
}
```

输出：

```text
Derived::show
Derived::show
```

如果不是通过基类指针或引用调用，而是普通对象按值调用，就不会体现运行时多态。

```cpp
Derived d;
Base b = d;
b.show(); // 对象切片后，只剩 Base 部分
```

> [!IMPORTANT]
>
> 运行时多态的关键调用形式是：基类指针或基类引用调用虚函数。

## 3. 虚函数

虚函数是在成员函数前加 `virtual` 的函数。

```cpp
class Base {
public:
    virtual void show() {
        std::cout << "Base::show" << std::endl;
    }
};
```

派生类可以重写这个虚函数。

```cpp
class Derived : public Base {
public:
    void show() override {
        std::cout << "Derived::show" << std::endl;
    }
};
```

`virtual` 写在基类函数声明中，表示这个函数支持动态绑定。

派生类重写时可以不写 `virtual`，但建议写 `override`。

```cpp
class Derived : public Base {
public:
    void show() override {
        std::cout << "Derived::show" << std::endl;
    }
};
```

虚函数的特点：

- 必须是成员函数。
- 不能是静态成员函数。
- 可以被派生类重写。
- 通过基类指针或引用调用时，会根据真实对象类型决定调用哪个版本。

> [!TIP]
>
> 基类中写 `virtual`，派生类重写时写 `override`。这样既表达意图，也能让编译器帮忙检查函数签名是否真的匹配。

## 4. 虚函数机制

虚函数机制通常依靠虚函数表和虚函数表指针实现。

一个含有虚函数的类，编译器通常会为它生成一张虚函数表，简称虚表。

虚表中存放的是虚函数的地址。

对象中通常会有一个隐藏的指针，指向该类对应的虚表。这个隐藏指针常被称为虚表指针。

可以这样理解：

```text
对象 -> 虚表指针 -> 虚函数表 -> 具体虚函数地址
```

示例：

```cpp
class Base {
public:
    virtual void f() {
        std::cout << "Base::f" << std::endl;
    }
};

class Derived : public Base {
public:
    void f() override {
        std::cout << "Derived::f" << std::endl;
    }
};
```

当执行：

```cpp
Base* p = new Derived;
p->f();
```

大致过程是：

1. `p` 的静态类型是 `Base*`。
2. `p` 实际指向的是 `Derived` 对象。
3. 通过对象内部的虚表指针找到 `Derived` 的虚函数表。
4. 从虚函数表中找到 `Derived::f` 的地址。
5. 调用 `Derived::f`。

> [!IMPORTANT]
>
> 通常是“一个类对应一张虚函数表”，而“每个含虚函数的对象中有一个虚表指针”。所以不是一个对象对应一整张虚函数表。

虚函数会带来一些额外开销：

- 对象中可能多一个虚表指针。
- 调用虚函数时需要通过虚表间接查找函数地址。

但它换来的是运行时动态绑定能力。

> [!NOTE]
>
> 虚函数表和虚表指针是常见实现方式，不是 C++ 标准强制规定的具体实现细节。标准规定的是虚函数的行为。

## 5. 隐藏、重载、重写

`oversee` 这个词在 C++ 语境里通常不是标准术语。这里更常对比的是隐藏、重载和重写。

| 概念 | 英文 | 发生位置 | 核心条件 |
| --- | --- | --- | --- |
| 隐藏 | name hiding | 基类和派生类之间 | 派生类出现同名成员，隐藏基类同名成员 |
| 重载 | overload | 同一作用域 | 函数名相同，参数列表不同 |
| 重写 | override | 基类和派生类之间 | 基类虚函数，派生类函数签名匹配 |

重载示例：

```cpp
void print(int value) {}
void print(double value) {}
```

隐藏示例：

```cpp
class Base {
public:
    void show(int value) {}
};

class Derived : public Base {
public:
    void show() {}
};
```

这里 `Derived::show()` 会隐藏 `Base::show(int)`。

重写示例：

```cpp
class Base {
public:
    virtual void run() {}
};

class Derived : public Base {
public:
    void run() override {}
};
```

用自己的话描述：

```text
重载：同一层里，同名函数参数不同。
隐藏：子类有同名成员，把父类同名成员遮住。
重写：子类重新实现父类的虚函数。
```

> [!CAUTION]
>
> 重写必须要求基类函数是虚函数，并且派生类函数签名匹配。只是名字一样，不一定是重写，也可能只是隐藏。

## 6. 纯虚函数

纯虚函数是在虚函数声明后面加 `= 0`。

```cpp
class Shape {
public:
    virtual double area() const = 0;
};
```

纯虚函数表示：这个函数在基类中只定义接口，不提供普通实现，要求派生类去实现。

派生类必须重写纯虚函数，否则派生类也会成为抽象类。

```cpp
class Circle : public Shape {
private:
    double radius;

public:
    Circle(double r) : radius(r) {}

    double area() const override {
        return 3.14 * radius * radius;
    }
};
```

使用：

```cpp
Shape* shape = new Circle(10);
std::cout << shape->area() << std::endl;
delete shape;
```

> [!IMPORTANT]
>
> 只要一个类中存在纯虚函数，这个类就是抽象类，不能直接创建对象。

## 7. 抽象类

抽象类是含有至少一个纯虚函数的类。

```cpp
class Animal {
public:
    virtual void speak() = 0;
};
```

抽象类的特点：

- 不能直接创建对象。
- 可以定义指针和引用。
- 主要用于定义统一接口。
- 派生类如果没有实现所有纯虚函数，仍然是抽象类。

不能直接创建对象：

```cpp
// Animal animal; // 错误，抽象类不能实例化
```

可以使用指针或引用指向派生类对象：

```cpp
class Dog : public Animal {
public:
    void speak() override {
        std::cout << "Dog bark" << std::endl;
    }
};

void test(Animal& animal) {
    animal.speak();
}

int main() {
    Dog dog;
    test(dog);
}
```

抽象类常用于设计接口。

```cpp
class Logger {
public:
    virtual void log(const std::string& message) = 0;
    virtual ~Logger() = default;
};
```

不同派生类可以提供不同实现。

```cpp
class ConsoleLogger : public Logger {
public:
    void log(const std::string& message) override {
        std::cout << message << std::endl;
    }
};

class FileLogger : public Logger {
public:
    void log(const std::string& message) override {
        // 写入文件
    }
};
```

> [!IMPORTANT]
>
> 如果一个类要作为多态基类使用，析构函数通常应该声明为 `virtual`。这样通过基类指针删除派生类对象时，才能正确调用派生类析构函数。

## 本章反思

第八章的重点是理解运行时多态的条件和机制。多态不是简单的函数同名，而是通过基类指针或引用调用虚函数时，程序在运行时根据对象真实类型选择具体函数。虚函数通常依靠虚表和虚表指针实现，一个类通常对应一张虚表，对象中保存指向虚表的指针。隐藏、重载、重写要区分清楚：重载发生在同一作用域，隐藏发生在父子类同名成员之间，重写必须基于虚函数。纯虚函数和抽象类的意义是定义接口，把具体行为交给派生类实现。

# 第九章：模板

## 1. 模板的本质

模板是 C++ 中的泛型编程机制。

它的作用是：先写一份和具体类型无关的代码，等真正使用时，再由编译器根据传入的类型生成对应版本的函数或类。

普通函数：

```cpp
int add(int a, int b) {
    return a + b;
}

double add(double a, double b) {
    return a + b;
}
```

如果逻辑完全一样，只是类型不同，就可以写成函数模板：

```cpp
template <typename T>
T add(T a, T b) {
    return a + b;
}
```

使用：

```cpp
std::cout << add(1, 2) << std::endl;
std::cout << add(1.5, 2.5) << std::endl;
```

编译器会根据调用生成对应版本：

```text
add<int>
add<double>
```

> [!IMPORTANT]
>
> 模板本身不是最终可执行代码，更像是一张“代码生成图纸”。真正使用某个类型调用模板时，编译器才会根据类型生成具体代码。

模板实例化发生在编译期。

也就是说，模板的类型推导、实例化和类型检查主要都发生在编译阶段，而不是程序运行时。

> [!NOTE]
>
> 模板带来的是编译期多态；虚函数带来的是运行时多态。

## 2. 模板基本语法

函数模板的基本语法：

```cpp
template <typename T>
返回类型 函数名(参数列表) {
    函数体
}
```

示例：

```cpp
template <typename T>
T maxValue(T a, T b) {
    return a > b ? a : b;
}
```

调用：

```cpp
std::cout << maxValue(3, 5) << std::endl;
std::cout << maxValue(3.5, 2.1) << std::endl;
```

类模板的基本语法：

```cpp
template <typename T>
class Box {
private:
    T value;

public:
    Box(const T& v) : value(v) {}

    T getValue() const {
        return value;
    }
};
```

使用类模板时，通常要显式写出类型：

```cpp
Box<int> intBox(10);
Box<std::string> stringBox("hello");
```

模板中常见关键字：

| 关键字 | 作用 |
| --- | --- |
| `template` | 声明模板 |
| `typename` | 声明模板类型参数 |
| `class` | 也可以声明模板类型参数 |

下面两种写法在声明类型模板参数时基本等价：

```cpp
template <typename T>
void func(T value) {}

template <class T>
void func(T value) {}
```

> [!TIP]
>
> 声明类型模板参数时，`typename` 和 `class` 大多数情况下都可以用。现代 C++ 中更常用 `typename`，因为它更直接表达“这里是一个类型”。

## 3. 隐式实例化和显式实例化

隐式实例化是最常见的情况。

当调用模板函数时，编译器根据实参类型自动推导模板参数，并生成对应版本。

```cpp
template <typename T>
T square(T value) {
    return value * value;
}

int main() {
    square(3);     // 编译器生成 square<int>
    square(2.5);   // 编译器生成 square<double>
}
```

显式实例化是主动告诉编译器生成某个具体版本。

```cpp
template <typename T>
T square(T value) {
    return value * value;
}

template int square<int>(int);
template double square<double>(double);
```

也可以显式指定模板实参调用：

```cpp
square<int>(3);
square<double>(3);
```

隐式实例化和显式实例化的区别：

| 类型 | 含义 |
| --- | --- |
| 隐式实例化 | 编译器根据使用情况自动生成模板代码 |
| 显式实例化 | 程序员明确要求编译器生成某个具体版本 |

> [!NOTE]
>
> 平时写代码最常用的是隐式实例化。显式实例化更多出现在模板实现拆分、减少重复实例化、控制编译产物等场景中。

## 4. 模板参数命名

模板参数名本质上只是一个占位符名字。

常见写法：

```cpp
template <typename T>
void print(T value) {}
```

这里的 `T` 表示某一种类型，具体是什么类型要等调用时才确定。

也可以写成其他名字：

```cpp
template <typename K, typename V>
class Pair {
private:
    K key;
    V value;
};
```

常见命名习惯：

| 名字 | 常见含义 |
| --- | --- |
| `T` | Type，普通类型参数 |
| `U` | 第二个普通类型参数 |
| `K` | Key，键类型 |
| `V` | Value，值类型 |
| `Args` | 一组可变参数类型 |

> [!TIP]
>
> `T`、`K`、`V` 这些名字没有特殊语法含义，只是约定俗成的命名习惯。真正起作用的是 `template <typename ...>` 这部分声明。

## 5. 普通函数和函数模板重载

普通函数和函数模板可以同时存在，并参与重载决议。

```cpp
void print(int value) {
    std::cout << "普通函数" << std::endl;
}

template <typename T>
void print(T value) {
    std::cout << "函数模板" << std::endl;
}
```

调用：

```cpp
print(10);
print(3.14);
```

通常输出：

```text
普通函数
函数模板
```

原因是：

- `print(10)` 可以精确匹配普通函数 `print(int)`。
- `print(3.14)` 没有对应普通函数，使用函数模板生成 `print<double>`。

如果普通函数和函数模板都能匹配，而且匹配程度一样，通常普通函数优先级更高。

```cpp
void func(int value) {
    std::cout << "普通函数" << std::endl;
}

template <typename T>
void func(T value) {
    std::cout << "函数模板" << std::endl;
}

func(10); // 优先调用普通函数
```

如果想强制使用函数模板，可以显式指定模板参数。

```cpp
func<int>(10);
```

> [!IMPORTANT]
>
> 普通函数和函数模板都能匹配时，优先选择更合适的匹配；如果匹配程度相同，普通函数通常优先。

## 6. 可变参数模板

可变参数模板可以接收任意数量的模板参数。

语法中常见的 `...Args` 表示模板参数包。

```cpp
template <typename... Args>
void func(Args... args) {
}
```

含义：

| 写法 | 含义 |
| --- | --- |
| `typename... Args` | 声明一个类型参数包 |
| `Args... args` | 声明一个函数参数包 |
| `args...` | 展开参数包 |

示例：

```cpp
template <typename... Args>
void printCount(Args... args) {
    std::cout << sizeof...(args) << std::endl;
}

int main() {
    printCount(1);
    printCount(1, 2.5, "hello");
}
```

`sizeof...(args)` 可以得到参数包中参数的数量。

C++17 中可以使用折叠表达式处理参数包。

```cpp
template <typename... Args>
auto sum(Args... args) {
    return (args + ...);
}

int main() {
    std::cout << sum(1, 2, 3, 4) << std::endl;
}
```

输出：

```text
10
```

可变参数模板常用于：

- 写通用转发函数。
- 写日志接口。
- 写工厂函数。
- 实现类似 `std::make_unique`、`std::tuple` 这样的工具。

> [!NOTE]
>
> `...Args` 可以理解为“一包类型”，`args...` 可以理解为“一包实参”。可变参数模板的核心就是声明参数包和展开参数包。

## 本章反思

第九章的重点是理解模板是编译期的代码生成机制。函数模板和类模板让我们可以写出与具体类型无关的通用代码，编译器在使用时根据类型实例化出具体版本。`T`、`K`、`V` 只是模板参数名，不是特殊语法。普通函数和函数模板重载时，编译器会选择更合适的匹配，匹配程度相同时普通函数通常优先。可变参数模板通过参数包和展开机制处理任意数量的参数，是很多现代 C++ 通用工具的基础。

# 第十章：移动语义与智能指针

## 1. 移动语义

移动语义是 C++11 引入的一种资源转移机制。

它的核心思想是：当一个对象马上就不用了，不必再做昂贵的深拷贝，可以直接把它内部管理的资源“转移”给另一个对象。

例如一个类内部管理一块堆内存：

```cpp
class Buffer {
private:
    char* data;
    std::size_t size;

public:
    Buffer(std::size_t n) : data(new char[n]), size(n) {}

    ~Buffer() {
        delete[] data;
    }
};
```

如果拷贝这个对象，通常要重新申请一块内存，再复制内容。

```text
深拷贝：重新申请资源 + 复制数据
移动：直接转移资源所有权
```

移动语义适合处理：

- 动态内存。
- 文件句柄。
- 网络连接。
- 互斥锁。
- 智能指针等不能或不适合复制的资源。

> [!IMPORTANT]
>
> 移动语义不是“移动对象本身在内存中的位置”，而是转移对象内部资源的所有权。

## 2. 左值引用和右值引用

左值可以简单理解为：有名字、可以取地址、生命周期相对明确的表达式。

```cpp
int a = 10;
```

这里 `a` 是左值。

左值引用使用 `&`。

```cpp
int& r = a;
```

右值通常是临时值、字面量、表达式结果等。

```cpp
int b = 1 + 2;
```

这里 `1 + 2` 的结果就是一个右值。

右值引用使用 `&&`。

```cpp
int&& r = 10;
```

常见对比：

| 类型 | 语法 | 能绑定什么 |
| --- | --- | --- |
| 左值引用 | `T&` | 左值 |
| 常量左值引用 | `const T&` | 左值和右值 |
| 右值引用 | `T&&` | 右值 |

示例：

```cpp
int a = 10;

int& r1 = a;
const int& r2 = 20;
int&& r3 = 30;
```

右值引用的主要作用是：识别那些可以被“移动资源”的临时对象。

> [!NOTE]
>
> 右值引用变量本身有名字，所以它在表达式中是左值。是否是右值引用类型，和表达式是不是左值，是两个不同概念。

## 3. 移动构造和移动赋值

移动构造函数用一个即将不用的对象创建新对象。

常见形式：

```cpp
ClassName(ClassName&& other);
```

移动赋值运算符用于已有对象接收另一个即将不用的对象的资源。

常见形式：

```cpp
ClassName& operator=(ClassName&& other);
```

示例：

```cpp
class Buffer {
private:
    char* data;
    std::size_t size;

public:
    Buffer(std::size_t n) : data(new char[n]), size(n) {}

    Buffer(const Buffer& other)
        : data(new char[other.size]), size(other.size) {
        std::copy(other.data, other.data + size, data);
    }

    Buffer(Buffer&& other) noexcept
        : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        delete[] data;

        data = other.data;
        size = other.size;

        other.data = nullptr;
        other.size = 0;

        return *this;
    }

    ~Buffer() {
        delete[] data;
    }
};
```

移动构造的关键步骤：

1. 接管 `other` 的资源。
2. 把 `other` 置为安全的空状态。
3. 避免两个对象析构时释放同一份资源。

> [!CAUTION]
>
> 移动后，源对象仍然必须是可析构、可赋值的有效对象，但它内部的资源可能已经被转移，不能再假设它还保留原来的值。

> [!TIP]
>
> 移动构造函数和移动赋值运算符通常建议标记为 `noexcept`，这样标准库容器在扩容搬移元素时更愿意使用移动操作。

## 4. std::move

`std::move` 的作用是把一个表达式强制转换成右值引用。

它本身不移动任何资源。

```cpp
std::string s1 = "hello";
std::string s2 = std::move(s1);
```

这里真正执行移动的是 `std::string` 的移动构造函数，不是 `std::move` 函数。

可以理解为：

```text
std::move：告诉编译器，这个对象可以被当成右值使用
移动构造/移动赋值：真正转移资源
```

自定义类型示例：

```cpp
Buffer b1(1024);
Buffer b2(std::move(b1));
```

`std::move(b1)` 之后，`b1` 仍然存在，但它的资源可能已经被 `b2` 接管。

> [!IMPORTANT]
>
> `std::move` 不等于移动资源，它只是类型转换。真正是否发生移动，取决于后续有没有匹配的移动构造函数或移动赋值运算符。

> [!CAUTION]
>
> 对一个对象使用 `std::move` 之后，不要继续依赖它原来的内容。通常只对它重新赋值或让它正常析构。

## 5. RAII

RAII 的全称是 Resource Acquisition Is Initialization，意思是“资源获取即初始化”。

它的思想是：把资源的获取和释放绑定到对象生命周期上。

```text
构造函数中获取资源
析构函数中释放资源
```

示例：

```cpp
class File {
private:
    FILE* fp;

public:
    File(const char* path) {
        fp = std::fopen(path, "r");
    }

    ~File() {
        if (fp != nullptr) {
            std::fclose(fp);
        }
    }
};
```

这样对象离开作用域时，析构函数会自动释放资源。

RAII 的特点：

- 不需要手动记住释放资源。
- 遇到异常时也能自动清理栈对象。
- 资源生命周期清晰。
- 是智能指针、锁管理器、文件封装类的基础思想。

> [!IMPORTANT]
>
> RAII 的关键不是“写析构函数”本身，而是让资源所有权由对象管理，让对象生命周期决定资源释放时机。

## 6. 智能指针类型

智能指针是标准库提供的 RAII 工具，用来管理动态分配的对象。

头文件：

```cpp
#include <memory>
```

常见智能指针：

| 智能指针 | 含义 |
| --- | --- |
| `std::unique_ptr` | 独占所有权，不能复制，只能移动 |
| `std::shared_ptr` | 共享所有权，通过引用计数管理资源 |
| `std::weak_ptr` | 弱引用，不拥有资源，常用来打破循环引用 |

`unique_ptr` 示例：

```cpp
std::unique_ptr<int> p = std::make_unique<int>(10);
```

`shared_ptr` 示例：

```cpp
std::shared_ptr<int> p1 = std::make_shared<int>(10);
std::shared_ptr<int> p2 = p1;
```

`weak_ptr` 示例：

```cpp
std::shared_ptr<int> sp = std::make_shared<int>(10);
std::weak_ptr<int> wp = sp;
```

> [!TIP]
>
> 创建智能指针时，优先使用 `std::make_unique` 和 `std::make_shared`，少直接写 `new`。

## 7. 智能指针特点

`unique_ptr` 表示独占所有权。

```cpp
std::unique_ptr<int> p1 = std::make_unique<int>(10);
// std::unique_ptr<int> p2 = p1; // 错误，不能复制

std::unique_ptr<int> p2 = std::move(p1);
```

移动之后，`p1` 不再拥有资源。

`shared_ptr` 表示共享所有权。

```cpp
std::shared_ptr<int> p1 = std::make_shared<int>(10);
std::shared_ptr<int> p2 = p1;

std::cout << p1.use_count() << std::endl;
```

当最后一个 `shared_ptr` 被销毁时，资源才会释放。

`weak_ptr` 不增加引用计数，不拥有资源。

```cpp
std::weak_ptr<int> wp;

{
    std::shared_ptr<int> sp = std::make_shared<int>(10);
    wp = sp;
}

if (wp.expired()) {
    std::cout << "资源已经释放" << std::endl;
}
```

如果要使用 `weak_ptr` 指向的对象，需要先调用 `lock()` 得到 `shared_ptr`。

```cpp
if (auto sp = wp.lock()) {
    std::cout << *sp << std::endl;
}
```

三者选择：

| 场景 | 推荐 |
| --- | --- |
| 只有一个所有者 | `unique_ptr` |
| 多个所有者共享资源 | `shared_ptr` |
| 观察资源但不拥有资源 | `weak_ptr` |

> [!IMPORTANT]
>
> 能用 `unique_ptr` 就不要急着用 `shared_ptr`。共享所有权会让资源生命周期更复杂。

`shared_ptr` 要注意循环引用问题。

```cpp
class B;

class A {
public:
    std::shared_ptr<B> b;
};

class B {
public:
    std::shared_ptr<A> a;
};
```

如果 `A` 和 `B` 互相持有 `shared_ptr`，引用计数可能永远不为 0，导致资源无法释放。

解决方式是把其中一边改成 `weak_ptr`。

```cpp
class B;

class A {
public:
    std::shared_ptr<B> b;
};

class B {
public:
    std::weak_ptr<A> a;
};
```

> [!CAUTION]
>
> `shared_ptr` 的循环引用是常见内存泄漏来源。两个对象互相引用时，至少一边通常应该使用 `weak_ptr`。

## 本章反思

第十章的重点是理解资源所有权。移动语义解决的是“资源能不能转移”的问题，右值引用让编译器能识别适合移动的对象，`std::move` 只是把对象转换成右值引用，真正转移资源的是移动构造和移动赋值。RAII 把资源释放绑定到对象生命周期上，智能指针则是 RAII 在动态内存管理中的典型应用。实际写代码时，优先使用 `unique_ptr` 表达独占所有权，确实需要共享时再使用 `shared_ptr`，遇到循环引用时用 `weak_ptr` 打破所有权闭环。

# 第十一章：STL

## 1. 顺序容器

顺序容器按照元素在容器中的位置来组织数据。

常见顺序容器：

| 容器 | 底层结构 | 特点 |
| --- | --- | --- |
| `vector` | 连续动态数组 | 随机访问快，尾插快，中间插入删除可能慢 |
| `deque` | 分段连续空间 | 头尾插入删除较快，支持随机访问 |
| `list` | 双向链表 | 任意位置插入删除快，不支持随机访问 |
| `forward_list` | 单向链表 | 更轻量，只能单向遍历 |
| `array` | 固定大小数组 | 大小编译期确定，连续存储 |

`vector` 示例：

```cpp
std::vector<int> nums;

nums.push_back(10);
nums.push_back(20);
nums.push_back(30);

std::cout << nums[0] << std::endl;
```

`list` 示例：

```cpp
std::list<int> nums;

nums.push_back(10);
nums.push_front(20);
```

顺序容器常见操作：

| 操作 | 含义 |
| --- | --- |
| `push_back` | 尾部插入 |
| `pop_back` | 删除尾部元素 |
| `insert` | 指定位置插入 |
| `erase` | 删除指定位置元素 |
| `begin` / `end` | 获取迭代器范围 |
| `size` | 元素个数 |
| `empty` | 是否为空 |

迭代器失效要特别注意。

常见情况：

| 容器 | 迭代器失效情况 |
| --- | --- |
| `vector` | 扩容后，原迭代器、指针、引用通常全部失效 |
| `deque` | 插入删除可能导致迭代器失效，规则比 `vector` 更复杂 |
| `list` | 插入一般不影响其他迭代器；删除某个元素只让该元素迭代器失效 |

> [!CAUTION]
>
> 使用顺序容器时，最容易踩坑的是“边遍历边插入删除”。删除元素后，要使用 `erase` 返回的新迭代器继续遍历。

## 2. 有序关联式容器

有序关联式容器会按照键自动排序。

常见有序关联式容器：

| 容器 | 特点 | 底层结构 |
| --- | --- | --- |
| `set` | key 即 value，元素唯一 | 通常是红黑树 |
| `multiset` | key 即 value，元素可重复 | 通常是红黑树 |
| `map` | key-value，key 唯一 | 通常是红黑树 |
| `multimap` | key-value，key 可重复 | 通常是红黑树 |

`set` 示例：

```cpp
std::set<int> nums = {3, 1, 2, 1};

for (int num : nums) {
    std::cout << num << std::endl;
}
```

输出：

```text
1
2
3
```

`map` 示例：

```cpp
std::map<std::string, int> scores;

scores["zhangsan"] = 90;
scores["lisi"] = 85;
```

常见操作：

| 操作 | 含义 |
| --- | --- |
| `insert` | 插入元素 |
| `erase` | 删除元素 |
| `find` | 查找元素 |
| `count` | 统计 key 个数 |
| `lower_bound` | 找到第一个不小于 key 的位置 |
| `upper_bound` | 找到第一个大于 key 的位置 |

> [!IMPORTANT]
>
> 有序关联式容器的关键是“按 key 排序”。`set` 的 key 就是元素本身，`map` 的 key 是 `pair` 中的 `first`。

## 3. 无序关联式容器

无序关联式容器不按大小排序，而是通过哈希表组织数据。

常见无序关联式容器：

| 容器 | 特点 | 底层结构 |
| --- | --- | --- |
| `unordered_set` | key 即 value，元素唯一 | 哈希表 |
| `unordered_multiset` | key 即 value，元素可重复 | 哈希表 |
| `unordered_map` | key-value，key 唯一 | 哈希表 |
| `unordered_multimap` | key-value，key 可重复 | 哈希表 |

示例：

```cpp
std::unordered_map<std::string, int> count;

count["cpp"]++;
count["java"]++;
count["cpp"]++;
```

无序关联式容器的平均查找、插入、删除复杂度通常是：

```text
O(1)
```

但最坏情况下可能退化。

> [!NOTE]
>
> `unordered_map` 不是“随机顺序”，而是哈希表内部桶的顺序。不要依赖它的遍历顺序。

常见操作：

| 操作 | 含义 |
| --- | --- |
| `insert` | 插入 |
| `erase` | 删除 |
| `find` | 查找 |
| `count` | 统计 key 个数 |
| `bucket_count` | 桶数量 |
| `load_factor` | 当前负载因子 |

> [!TIP]
>
> 如果需要有序遍历，使用 `map` / `set`。如果只关心快速查找，通常优先考虑 `unordered_map` / `unordered_set`。

## 4. 容器适配器

容器适配器不是全新的底层容器，而是在已有容器基础上封装出特定接口。

常见容器适配器：

| 适配器 | 默认底层容器 | 特点 |
| --- | --- | --- |
| `stack` | `deque` | 栈，后进先出 |
| `queue` | `deque` | 队列，先进先出 |
| `priority_queue` | `vector` | 优先队列，默认大顶堆 |

`stack` 示例：

```cpp
std::stack<int> s;

s.push(10);
s.push(20);
std::cout << s.top() << std::endl;
s.pop();
```

`queue` 示例：

```cpp
std::queue<int> q;

q.push(10);
q.push(20);
std::cout << q.front() << std::endl;
q.pop();
```

`priority_queue` 示例：

```cpp
std::priority_queue<int> pq;

pq.push(3);
pq.push(1);
pq.push(5);

std::cout << pq.top() << std::endl; // 5
```

> [!NOTE]
>
> 适配器的特点是限制接口，只暴露符合特定数据结构语义的操作。例如 `stack` 不提供迭代器，因为栈只应该通过栈顶操作。

## 5. 迭代器

迭代器可以理解为容器和算法之间的桥梁。

容器提供迭代器，算法通过迭代器访问容器元素。

```cpp
std::vector<int> nums = {1, 2, 3};

for (auto it = nums.begin(); it != nums.end(); ++it) {
    std::cout << *it << std::endl;
}
```

常见迭代器类型：

| 迭代器类型 | 能做的操作 | 常见容器 |
| --- | --- | --- |
| 输入迭代器 | 只读、单向前进 | 输入流 |
| 输出迭代器 | 只写、单向前进 | 输出流 |
| 前向迭代器 | 多次读写、单向前进 | `forward_list` |
| 双向迭代器 | 前进和后退 | `list`、`set`、`map` |
| 随机访问迭代器 | 加减整数、下标访问 | `vector`、`deque`、`array` |

不同迭代器支持的操作不同。

```cpp
std::vector<int> v = {1, 2, 3};
auto it = v.begin();
it = it + 2; // vector 支持随机访问
```

```cpp
std::list<int> lst = {1, 2, 3};
auto it = lst.begin();
// it = it + 2; // 错误，list 不是随机访问迭代器
```

> [!IMPORTANT]
>
> STL 算法依赖迭代器能力。比如 `std::sort` 需要随机访问迭代器，所以可以用于 `vector`，不能直接用于 `list`。

## 6. 常见算法

STL 算法通常定义在 `<algorithm>` 中。

算法不直接关心容器类型，而是通过迭代器范围工作。

常见算法：

| 算法 | 作用 |
| --- | --- |
| `find` | 查找某个值 |
| `count` | 统计某个值出现次数 |
| `sort` | 排序 |
| `for_each` | 遍历并执行操作 |
| `copy` | 拷贝 |
| `remove` | 移动要删除的元素到后面 |
| `unique` | 去除相邻重复 |
| `transform` | 转换元素 |

示例：

```cpp
std::vector<int> nums = {3, 1, 2};

std::sort(nums.begin(), nums.end());
```

查找：

```cpp
auto it = std::find(nums.begin(), nums.end(), 2);

if (it != nums.end()) {
    std::cout << "找到了" << std::endl;
}
```

配合 lambda：

```cpp
std::for_each(nums.begin(), nums.end(), [](int value) {
    std::cout << value << std::endl;
});
```

> [!CAUTION]
>
> `std::remove` 不会真正删除容器元素，它只是调整元素位置并返回新的逻辑结尾。通常要配合 `erase` 使用，这叫 erase-remove 惯用法。

```cpp
nums.erase(std::remove(nums.begin(), nums.end(), 2), nums.end());
```

## 7. 插入迭代器

插入迭代器是一种迭代器适配器，用来把赋值操作转换成容器插入操作。

常见插入迭代器：

| 插入迭代器 | 作用 |
| --- | --- |
| `back_inserter` | 调用容器的 `push_back` |
| `front_inserter` | 调用容器的 `push_front` |
| `inserter` | 调用容器的 `insert` |

示例：

```cpp
std::vector<int> src = {1, 2, 3};
std::vector<int> dst;

std::copy(src.begin(), src.end(), std::back_inserter(dst));
```

如果不用 `back_inserter`，直接拷贝到空容器会出错。

```cpp
std::vector<int> dst;
// std::copy(src.begin(), src.end(), dst.begin()); // 错误，dst 没有空间
```

`inserter` 示例：

```cpp
std::set<int> s;
std::vector<int> nums = {3, 1, 2};

std::copy(nums.begin(), nums.end(), std::inserter(s, s.begin()));
```

> [!TIP]
>
> 插入迭代器适合和 `copy`、`transform` 等算法配合，用来把算法输出安全地插入容器。

## 8. lambda 表达式

lambda 表达式可以在代码中直接定义一个匿名函数对象。

基本语法：

```cpp
[捕获列表](参数列表) -> 返回类型 {
    函数体
};
```

返回类型通常可以省略。

```cpp
auto add = [](int a, int b) {
    return a + b;
};

std::cout << add(3, 4) << std::endl;
```

捕获方式常见有：

| 捕获方式 | 含义 |
| --- | --- |
| `[]` | 不捕获外部变量 |
| `[=]` | 按值捕获外部变量 |
| `[&]` | 按引用捕获外部变量 |
| `[x]` | 按值捕获 `x` |
| `[&x]` | 按引用捕获 `x` |

值捕获和引用捕获区别：

```cpp
int x = 10;

auto f1 = [x]() {
    std::cout << x << std::endl;
};

auto f2 = [&x]() {
    std::cout << x << std::endl;
};

x = 20;

f1(); // 10
f2(); // 20
```

lambda 调用方式和普通函数对象一样：

```cpp
auto print = [](int value) {
    std::cout << value << std::endl;
};

print(10);
```

> [!CAUTION]
>
> 引用捕获要注意生命周期。如果 lambda 在外部变量销毁后才执行，引用会悬空。

## 9. bind

`std::bind` 可以把一个可调用对象和部分参数绑定起来，生成一个新的可调用对象。

头文件：

```cpp
#include <functional>
```

示例：

```cpp
void print(int a, int b) {
    std::cout << a << " " << b << std::endl;
}

auto f = std::bind(print, 10, std::placeholders::_1);

f(20); // 等价于 print(10, 20)
```

`std::bind` 可以绑定：

- 普通函数。
- 函数指针。
- 成员函数。
- 函数对象。
- lambda。

绑定成员函数：

```cpp
class Student {
public:
    void show(int score) {
        std::cout << score << std::endl;
    }
};

Student s;

auto f = std::bind(&Student::show, &s, std::placeholders::_1);
f(90);
```

> [!NOTE]
>
> `std::placeholders::_1` 表示新函数对象调用时传入的第一个参数，`_2` 表示第二个参数。

> [!TIP]
>
> 现代 C++ 中，很多 `bind` 场景可以用 lambda 写得更直观。

## 10. 引用折叠

引用折叠是模板和右值引用中经常出现的规则。

当引用的引用出现时，C++ 会把它折叠成一个引用类型。

规则：

| 形式 | 折叠结果 |
| --- | --- |
| `T& &` | `T&` |
| `T& &&` | `T&` |
| `T&& &` | `T&` |
| `T&& &&` | `T&&` |

简单记忆：

```text
只要出现一个 &，结果就是 &
只有 && + &&，结果才是 &&
```

引用折叠常出现在万能引用和完美转发中。

```cpp
template <typename T>
void func(T&& value) {
}
```

这里的 `T&&` 不一定总是右值引用。

```cpp
int x = 10;

func(x);  // T 推导为 int&，T&& 折叠为 int&
func(20); // T 推导为 int，T&& 是 int&&
```

> [!IMPORTANT]
>
> 模板中的 `T&&` 在发生类型推导时，通常叫万能引用或转发引用。它既能绑定左值，也能绑定右值，具体结果由引用折叠决定。

## 11. std::function

`std::function` 是一个通用的可调用对象包装器。

头文件：

```cpp
#include <functional>
```

它可以保存多种可调用实体，只要调用签名一致。

```cpp
int add(int a, int b) {
    return a + b;
}

std::function<int(int, int)> func = add;

std::cout << func(3, 4) << std::endl;
```

保存 lambda：

```cpp
std::function<int(int, int)> func = [](int a, int b) {
    return a * b;
};
```

保存函数对象：

```cpp
class Add {
public:
    int operator()(int a, int b) const {
        return a + b;
    }
};

std::function<int(int, int)> func = Add{};
```

> [!NOTE]
>
> `std::function<R(Args...)>` 中，`R` 是返回类型，`Args...` 是参数类型列表。

`std::function` 常用于：

- 回调函数。
- 保存用户传入的行为。
- 统一管理不同类型的可调用对象。

## 12. std::mem_fn

`std::mem_fn` 可以把成员函数指针包装成普通可调用对象。

头文件：

```cpp
#include <functional>
```

示例：

```cpp
class Student {
public:
    void show() const {
        std::cout << "show" << std::endl;
    }
};

auto f = std::mem_fn(&Student::show);

Student s;
f(s);
```

也可以通过指针调用：

```cpp
Student* p = &s;
f(p);
```

`std::mem_fn` 的作用是让成员函数指针更容易和算法、`std::function` 等工具配合。

```cpp
std::vector<Student> students;

std::for_each(students.begin(), students.end(), std::mem_fn(&Student::show));
```

> [!TIP]
>
> 成员函数指针原始调用语法比较别扭，`std::mem_fn` 可以把它包装成更像普通函数对象的形式。

## 13. 可调用实体

可调用实体就是可以像函数一样调用的东西。

常见可调用实体：

| 类型 | 示例 |
| --- | --- |
| 普通函数 | `func(1, 2)` |
| 函数指针 | `p(1, 2)` |
| 成员函数指针 | `(obj.*p)(1, 2)` |
| 函数对象 | `obj(1, 2)` |
| lambda | `[](int x) { return x; }` |
| `std::function` | `std::function<int(int)>` |
| `std::bind` 结果 | `auto f = std::bind(...)` |
| `std::mem_fn` 结果 | `auto f = std::mem_fn(...)` |

示例：

```cpp
void call(const std::function<void()>& func) {
    func();
}

call([]() {
    std::cout << "hello" << std::endl;
});
```

> [!IMPORTANT]
>
> STL 算法大量使用可调用实体。比较规则、筛选条件、遍历操作、转换逻辑，都可以通过可调用对象传给算法。

## 本章反思

第十一章的重点是理解 STL 的分层设计：容器负责存储数据，迭代器负责连接容器和算法，算法通过迭代器操作数据，可调用对象负责提供自定义行为。顺序容器强调位置，有序关联式容器强调排序，无序关联式容器强调哈希查找，容器适配器强调受限接口。写 STL 代码时要特别注意迭代器类别、迭代器失效、算法的真实行为，以及 lambda、`bind`、`function`、`mem_fn` 这些工具如何统一表达“可调用行为”。
