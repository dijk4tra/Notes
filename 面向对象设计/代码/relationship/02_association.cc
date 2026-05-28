#include <iostream>
#include <vector>

using namespace std;

/*
关联关系
*/
class Teacher; // 前向声明

class Student {
private:
    string name;
    Teacher* teacher;  // 关联一个老师（单向关联）
public:
    Student(const string& n) : name(n), teacher(nullptr) {}
    void setTeacher(Teacher* t) { teacher = t; }
    void showTeacher();
};

class Teacher {
private:
    string name;
    vector<Student> students;
public:
    Teacher(const string& n) : name(n) {}
    string getName() const { return name; }
    void addStudent(Student s) { students.push_back(s); }
};

void Student::showTeacher() {
    if (teacher)
        cout << name << " 的导师是 " << teacher->getName() << endl;
    else
        cout << name << " 没有导师" << endl;
}

int main() {
    Teacher t("张教授");
    Student s("zs");
    s.setTeacher(&t);   // 关联
    s.showTeacher();
    return 0;
}