#include <iostream>
#include <memory>
using namespace std;

/*
DIP:依赖倒置原则
1. 高层模块不应该依赖低层模块
2. 二者都应该依赖抽象
3. 抽象不应该依赖具体实现
*/


// 如果不倒置, 高层依赖底层
class MySQLDatabase {
public:
    void save() { /* 写 MySQL */ }
};

class OrderService {
    MySQLDatabase db;   // 高层直接依赖具体低层 必须使用MySQL数据库
public:
    void process() { db.save(); }
};
// 上述代码中,如果底层发生变化,即使用了Oracle数据库,则需要修改OrderService类 违背OCP原则
// 解决:
// 1.定义抽象接口类
// 2. 子类可以实现不同的数据库操作
// 3. 高层模块依赖抽象接口类, 低层模块实现具体数据库操作

// 抽象接口
class Database2 {   
public:
    virtual void save() = 0;
};
// MySQL数据库实现
class MySQLDatabase2 : public Database2 {
public:
    void save() override { /* 写 MySQL */ }
};
// Oracle数据库实现
class OracleDatabase2 : public Database2 {
public:
    void save() override { /* 写 Oracle */ }
};

class OrderService {
    Database2& db;   // 依赖抽象，不依赖具体 MySQLDatabase2依赖于Database2接口
                     // 数据库可以是MySQL数据库,也可以是Oracle数据库
public:
    OrderService(Database2& d) : db(d) {}
    void process() { db.save(); }
};


class BankWorker
{
public:
    // 存款业务
    void save(){cout << "save" << endl;}
    // 支付业务
    void pay(){cout << "pay" << endl;}
    // 转账业务
    void transfer(){cout << "transfer" << endl;}
    virtual ~BankWorker() = default;

};
/*
 * 银行员工
 * 1. 存款业务
 * 2. 支付业务
 * 3. 转账业务
 * 如果有新的业务,或者业务有变化,会修改BankWorker类违背OCP原则
*/

/*
解决:
1.定义抽象接口类
2. 子类可以实现不同的业务

*/
// 抽象接口类
class BankWorker2
{
public:
    virtual void doBusiness() = 0;
    virtual ~BankWorker2() = default;

};

//低层模块（具体业务）
class SaveService : public BankWorker2
{
public:
    virtual void doBusiness() override
    {
        cout << "save" << endl;
    }
};

class PayService : public BankWorker2
{
public:
    virtual void doBusiness() override
    {
        cout << "pay" << endl;
    }
};

class TransferService : public BankWorker2
{
public:
    virtual void doBusiness() override
    {
        cout << "transfer" << endl;
    }
};

// 模拟业务场景
// 高层模块（业务调用方）
void doBusiness(BankWorker2 * worker)
{   
    // doBusiness 只跟 BankWorker2 打交道 
    // 只依赖BankWorker2这个抽象接口
    // 不依赖细节 具体实现类

    worker->doBusiness();
}

/*
高层不依赖低层，两者都依赖抽象
doBusiness（高层）不依赖 SaveBankWorker（低层），
它们都依赖 BankWorker2（抽象）。

抽象不依赖细节，细节依赖抽象
BankWorker2 接口没有提到具体业务，而各个具体业务类必须实现它。

倒置的效果：扩展自由、修改封闭
加业务 = 加类，不改调用端代码，完美契合开闭原则。*/
void test1()
{
    unique_ptr<BankWorker2> saveService{ new SaveService() };
    //调用者不关心具体实现，只依赖抽象协作
    doBusiness(saveService.get());

    unique_ptr<BankWorker2> payService{ new PayService() };
    doBusiness(payService.get());

    unique_ptr<BankWorker2> transferService{ new TransferService() };
    doBusiness(transferService.get());
}

int main(int argc, char const *argv[])
{
    test1();

    return 0;
}
