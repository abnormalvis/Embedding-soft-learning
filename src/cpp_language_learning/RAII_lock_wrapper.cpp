/*
要求：

创建一个类，在构造时自动获取 mutex 锁
析构时自动释放锁
支持 RAII 模式（作用域结束时自动释放）
用它实现一个线程安全的队列
*/

#include <deque>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class lock_wrapper
{
public:
    explicit lock_wrapper(std::mutex& m) : m_(m)
    {
        // RAII: construct and lock immediately.
        m_.lock();
    }

    lock_wrapper(const lock_wrapper&) = delete;
    lock_wrapper& operator=(const lock_wrapper&) = delete;

    ~lock_wrapper()
    {
        // RAII: unlock automatically when leaving scope.
        m_.unlock();
    }

private:
    std::mutex& m_;
};

template <typename T>
class thread_safe_queue
{
public:
    int push(const T& data)
    {
        // Use lock_wrapper as a scoped guard inside each operation.
        lock_wrapper lock(mutex_);
        try {
            q_.push_back(data);
            return 0;
        } catch (...) {
            return -1;
        }
    }

    bool pop(T& out)
    {
        lock_wrapper lock(mutex_);
        if (q_.empty()) {
            return false;
        }
        out = q_.front();
        q_.pop_front();
        return true;
    }

    std::size_t size() const
    {
        // const member function still needs locking, so mutex_ is mutable.
        lock_wrapper lock(mutex_);
        return q_.size();
    }

private:
    std::deque<T> q_;       // Fix: member itself is not a template declaration.
    mutable std::mutex mutex_;
};

int main()
{
    thread_safe_queue<int> q;
    const int num_threads = 4;
    const int pushes_per_thread = 1000;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&q, pushes_per_thread]() {
            for (int j = 0; j < pushes_per_thread; ++j) {
                q.push(j);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    const std::size_t expected = static_cast<std::size_t>(num_threads * pushes_per_thread);
    std::cout << "Final size: " << q.size() << std::endl;
    std::cout << "Expected size: " << expected << std::endl;

    if (q.size() == expected) {
        std::cout << "Thread-safe queue demo passed." << std::endl;
    } else {
        std::cout << "Thread-safe queue demo failed." << std::endl;
    }
}