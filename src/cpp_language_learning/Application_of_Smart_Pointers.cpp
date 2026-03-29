/*
要求：

创建一个 Widget 类，使用 std::shared_ptr 管理
实现一个"观察者"列表，用 std::weak_ptr 存储
模拟循环引用问题，然后用 weak_ptr 解决
编写测试代码验证内存正确释放
*/
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Widget
{
public:
    Widget(int id) : id_(id) {
        std::cout << "Widget " << id_ << " created.\n";
    }
    ~Widget() {
        std::cout << "Widget " << id_ << " destroyed.\n";
    }
    int getId() const { return id_; }

private:
    int id_;
};

class Subject
{
public:
    explicit Subject(std::string name) : name_(std::move(name))
    {
        std::cout << "Subject " << name_ << " created.\n";
    }

    ~Subject()
    {
        std::cout << "Subject " << name_ << " destroyed.\n";
    }

    void addObserver(const std::shared_ptr<Widget>& observer)
    {
        observers_.push_back(observer);
    }

    void notifyAll() const
    {
        std::cout << "Notify from " << name_ << ": ";
        bool hasObserver = false;
        for (const auto& weakObserver : observers_)
        {
            if (auto observer = weakObserver.lock())
            {
                std::cout << "Widget(" << observer->getId() << ") ";
                hasObserver = true;
            }
        }
        if (!hasObserver)
        {
            std::cout << "no live observers";
        }
        std::cout << "\n";
    }

private:
    std::string name_;
    std::vector<std::weak_ptr<Widget>> observers_;
};

class PersonBad;
class PersonGood;

class TeamBad
{
public:
    explicit TeamBad(std::string name) : name_(std::move(name))
    {
        ++aliveCount;
        std::cout << "TeamBad " << name_ << " created.\n";
    }

    ~TeamBad()
    {
        --aliveCount;
        std::cout << "TeamBad " << name_ << " destroyed.\n";
    }

    void setMember(const std::shared_ptr<PersonBad>& member)
    {
        member_ = member;
    }

    static int aliveCount;

private:
    std::string name_;
    std::shared_ptr<PersonBad> member_;
};

class PersonBad
{
public:
    explicit PersonBad(std::string name) : name_(std::move(name))
    {
        ++aliveCount;
        std::cout << "PersonBad " << name_ << " created.\n";
    }

    ~PersonBad()
    {
        --aliveCount;
        std::cout << "PersonBad " << name_ << " destroyed.\n";
    }

    void setTeam(const std::shared_ptr<TeamBad>& team)
    {
        team_ = team;
    }

    static int aliveCount;

private:
    std::string name_;
    std::shared_ptr<TeamBad> team_;
};

int TeamBad::aliveCount = 0;
int PersonBad::aliveCount = 0;

class TeamGood
{
public:
    explicit TeamGood(std::string name) : name_(std::move(name))
    {
        ++aliveCount;
        std::cout << "TeamGood " << name_ << " created.\n";
    }

    ~TeamGood()
    {
        --aliveCount;
        std::cout << "TeamGood " << name_ << " destroyed.\n";
    }

    void setMember(const std::shared_ptr<PersonGood>& member)
    {
        member_ = member;
    }

    static int aliveCount;

private:
    std::string name_;
    // 使用 weak_ptr 打破强引用环。
    std::weak_ptr<PersonGood> member_;
};

class PersonGood
{
public:
    explicit PersonGood(std::string name) : name_(std::move(name))
    {
        ++aliveCount;
        std::cout << "PersonGood " << name_ << " created.\n";
    }

    ~PersonGood()
    {
        --aliveCount;
        std::cout << "PersonGood " << name_ << " destroyed.\n";
    }

    void setTeam(const std::shared_ptr<TeamGood>& team)
    {
        team_ = team;
    }

    static int aliveCount;

private:
    std::string name_;
    std::shared_ptr<TeamGood> team_;
};

int TeamGood::aliveCount = 0;
int PersonGood::aliveCount = 0;

void testObserverList()
{
    std::cout << "\n=== weak_ptr observer list demo ===\n";
    auto subject = std::make_shared<Subject>("EventHub");
    auto w1 = std::make_shared<Widget>(1);
    auto w2 = std::make_shared<Widget>(2);
    subject->addObserver(w1);
    subject->addObserver(w2);
    subject->notifyAll();

    w1.reset();
    std::cout << "After destroying Widget(1):\n";
    subject->notifyAll();
}

void testCycleLeak()
{
    std::cout << "\n=== cycle with shared_ptr (bad) ===\n";
    {
        auto team = std::make_shared<TeamBad>("Core");
        auto person = std::make_shared<PersonBad>("Alice");

        team->setMember(person);
        person->setTeam(team);
    }

    std::cout << "Alive after scope (expect leak): TeamBad="
              << TeamBad::aliveCount << ", PersonBad="
              << PersonBad::aliveCount << "\n";
}

void testCycleFixed()
{
    std::cout << "\n=== cycle fixed with weak_ptr (good) ===\n";
    {
        auto team = std::make_shared<TeamGood>("Core");
        auto person = std::make_shared<PersonGood>("Bob");

        team->setMember(person);
        person->setTeam(team);
    }

    std::cout << "Alive after scope (expect 0): TeamGood="
              << TeamGood::aliveCount << ", PersonGood="
              << PersonGood::aliveCount << "\n";
}

int main()
{
    testObserverList();
    testCycleLeak();
    testCycleFixed();

    return 0;
}