#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

using namespace std;

/*
推模型
UP主直接将新视频的标题推送给粉丝，粉丝无需再向UP主查询。

拉模型:
粉丝主动向UP主查询最新视频内容 
*/

// 抽象观察者（粉丝）—— 推模型：接收推送的视频标题
class Fan {
public:
    virtual ~Fan() = default;
    virtual void onNewVideo(const std::string& videoTitle) = 0;
};

// 抽象主题（UP主）
class UpLoader {
    // 所有粉丝（弱引用）
    std::vector<std::weak_ptr<Fan>> fans;
public:
    // 粉丝关注
    void follow(const std::shared_ptr<Fan>& fan) {
        fans.push_back(fan);
    }
    // 粉丝取关
    void unfollow(const std::shared_ptr<Fan>& fan) {
        auto it = std::remove_if(fans.begin(), fans.end(),
            [&fan](const std::weak_ptr<Fan>& wp) {
                return wp.lock() == fan;
            });
        fans.erase(it, fans.end());
    }

    // 推模型通知：携带视频标题
    void notify(const std::string& title) {
        for (auto it = fans.begin(); it != fans.end(); ) {
            if (auto fan = it->lock()) {
                // 发送推送
                fan->onNewVideo(title);
                ++it;
            } else {
                it = fans.erase(it);  // 自动清理已销毁的粉丝
            }
        }
    }
};

// 具体UP主
class BilibiliUploader : public UpLoader {
    std::vector<std::string> videos;  // 所有已发布视频
public:
    void publishVideo(const std::string& title) {
        videos.push_back(title);
        std::cout << "[UP主] 发布新视频: 《" << title << "》\n";
        notify(title);   // 推送视频标题给所有粉丝 视频更新提醒
    }
};

// 具体粉丝类
class PushFan : public Fan {

public:
    PushFan(const string& n) : m_name(n) {}

    void onNewVideo(const std::string& title) override {
        std::cout << "  [粉丝 " << m_name << "] 收到推送: 《"
                  << title << "》\n";
    }
private:
    string m_name;
};

// --------- 测试 ---------
int main(int argc, char const *argv[])
{
    // 创建UP主对象 卢姥爷
    auto white = std::make_shared<BilibiliUploader>();

    auto fan1 = std::make_shared<PushFan>("小黑子");
    auto fan2 = std::make_shared<PushFan>("小白子");

    // 小黑子 小白子 关注 卢姥爷
    white->follow(fan1);
    white->follow(fan2);

    white->publishVideo("全体起立!!");   // 发布视频 并推送粉丝
    white->publishVideo("我和Faker 55开");

    // 小黑子 取关 卢姥爷
    white->unfollow(fan1);
}