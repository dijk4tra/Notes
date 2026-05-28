#include <iostream>
#include <vector>

using namespace std;

/*
聚合关系  整体与局部的关系, 整体和局部相互独立
*/

class Player {
public:
    Player(const string& n) : name(n) {}
    string name;
};

class Team {
public:
    Team(const string& name) : teamName(name) {}
    void addPlayer(Player* p) { players.push_back(p); }
    void showPlayers() {
        cout << teamName << " 队员: ";
        for (auto p : players) {
            cout << p->name << " ";
        }
        cout << endl;
    }
private:
    string teamName;
    vector<Player*> players;  // 聚合：球队拥有球员，但球员可以离开球队继续存在
};

int main() {
    Player p1("Curry");
    Player p2("Durant");
    Team warriors("勇士队");
    warriors.addPlayer(&p1);
    warriors.addPlayer(&p2);
    warriors.showPlayers();
    // p1, p2 离开球队后依然存在（生命周期独立于球队）
    return 0;
}