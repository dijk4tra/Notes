#include <iostream>
#include <vector>
using namespace std;

/*
LOD迪米特法则: 最少知识原则
一个对象应该对其他对象有尽可能少的了解，
只和“直接朋友”通信，不和“陌生人”说话。

*/

class Building
{
public:
    virtual void sale() = 0;
    virtual string getQuality() = 0;
    virtual ~Building(){}
};

class WankeBuilding : public Building
{
public:
    virtual void sale() override
    {
        cout << "sale" << endl;
    }
    virtual string getQuality() override
    {
        return "高品质";
    }
};

class HengdaBuilding : public Building
{
public:
    virtual void sale() override
    {
        cout << "sale" << endl;
    }
    virtual string getQuality() override
    {
        return "低品质";
    }
};

void test1()
{

    Building * pa = new WankeBuilding();
    Building * pb = new HengdaBuilding();
    if(pa->getQuality() == "高品质")
    {
        pa->sale();
    }
    if(pb->getQuality() == "低品质")
    {
        pb->sale();
    }   
    delete pa;
    delete pb;
}


// 中介类
class Agent
{
public:
    Agent()
    {
        m_buildings.push_back(new WankeBuilding{});
        m_buildings.push_back(new HengdaBuilding{});
    }
    ~Agent()
    {
        for(auto & building : m_buildings)
        {
            delete building;
        }
    }

    Building * findBuilding(const string & demand)
    {
        for(auto & building : m_buildings)  
        {
            if(building->getQuality() == demand)
            {
                return building;
            }
        }
        return nullptr;
    }
private:
    // 中介有很多房源
    vector<Building*> m_buildings;
};

void test2()
{
    Agent agent;
    string demand = "高品质";
    Building * building = agent.findBuilding(demand);
    if(building)
    {
        building->sale();
    }else{
        cout << "没有合适的房" << endl;
    }

}
int main(int argc, char const *argv[])
{
    
    return 0;
}
