#include <iostream>
using namespace std;

/*
SRP: 单一职责原则
一个类只负责一个职责    一个类的职责越少越好
*/

class SmartPhone
{
public:
    void call()
    {
        cout << "使用中国移动打电话" << endl;
    }
    void sendMessage()
    {
        cout << "使用4G网络发送短信" << endl;
    }

    void takePhoto()
    {
        cout << "使用徕卡镜头拍照" << endl;
    }
    void playMusic()
    {
        cout << "播放本地歌曲: " << endl;
    }
};

/*
当你要升级相机算法（改为 4800 万像素 AI 降噪）时，要碰这个类。
当网络从 2G 升级 4G/5G 时，也要改它。
类太大了，难以测试，任何一个功能的修改都可能意外影响其他功能。
*/

// 按照单一职责进行修改 每个类只负责一个职责
// 将手机的不同职责拆分到小而专注的类中，每个类只有一个变化的原因。

class PhoneCall
{
public:
    void call()
    {
        cout << "使用中国移动打电话" << endl;
    }
};

class PhoneMessage
{
public:
    void sendMessage()
    {
        cout << "使用4G网络发送短信" << endl;
    }
};

class PhonePhoto
{
public:
    void takePhoto()
    {
        cout << "使用徕卡镜头拍照" << endl;
    }
};

class PhoneMusic
{
public:
    void playMusic()
    {
        cout << "播放本地歌曲: " << endl;
    }
};

// 手机类本身作为"容器"，将不同的职责类作为成员变量进行组合。
class SmartPhone2
{
public:
    void call()
    {
        m_phoneCall.call();
    }
    void sendMessage()
    {
        m_phoneMessage.sendMessage();
    }
    void takePhoto()
    {
        m_phonePhoto.takePhoto();
    }
    void playMusic()
    {
        m_phoneMusic.playMusic();
    }

private:
    PhoneCall m_phoneCall;
    PhoneMessage m_phoneMessage;
    PhonePhoto m_phonePhoto;
    PhoneMusic m_phoneMusic;
};

void test1()
{
    SmartPhone2 phone2;
    phone2.call();
    phone2.sendMessage();
    phone2.takePhoto();
    phone2.playMusic();
}

int main(int argc, char const *argv[])
{
    test1();
    return 0;
}
