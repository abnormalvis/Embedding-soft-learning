/*
使用 std::atomic 实现一个无锁计数器
实现一个基于 std::mutex 的计数器（对比）
创建多个线程同时增加计数，验证结果正确性
预期输出： 两个计数器最终值应该相同。
*/
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
/*
定义变量：使用 std::atomic<int> count。
原子操作：直接调用 count.fetch_add(1) 或使用自增运算符 count++。
底层原理：
    Fetch-and-Add：硬件直接支持的原子加法。
    CAS 循环：如果操作更复杂，会读取当前值，计算新值，然后通过指令检查：“如果当前值还没变，就更新它；如果变了，就重新读取并重试。”
*/
class AtomicCounter
{
public:
    AtomicCounter() : count_(0) {}

    void increment()
    {
        count_.fetch_add(1, std::memory_order_relaxed);
    }

    int getCount() const
    {
        return count_.load(std::memory_order_relaxed);
    }

private:
    std::atomic<int> count_;
};

/*
定义变量：一个普通的整型变量（如 int count）和一个 std::mutex mtx。
上锁：线程尝试调用 mtx.lock()。如果锁已被占用，线程将进入阻塞状态。
操作：获取锁后，执行 count++。
释放：执行完成后调用 mtx.unlock()，唤醒其他等待锁的线程。
建议：在 C++ 中通常使用 std::lock_guard 或 std::unique_lock 来自动管理锁的生命周期，防止死锁。
*/
class MutexCounter
{
public:
    MutexCounter() : count_(0) {}

    void increment()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ++count_;
    }

    int getCount() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }

private:
    mutable std::mutex mutex_;
    int count_;
};

int main()
{
    AtomicCounter atomicCounter;
    MutexCounter mutexCounter;

    const int numThreads = 10;
    const int incrementsPerThread = 100;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    for (int i = 0; i < numThreads; ++i)
    {
        /*&atomicCounter：按引用捕获，线程里操作的是同一个原子计数器对象
        &mutexCounter：按引用捕获，线程里操作的是同一个加锁计数器对象
        incrementsPerThread：按值捕获，每个线程拿到一份拷贝（这里是 int，拷贝开销极小）
        如果按值捕获，每个线程会改自己的副本，主线程看到的值不会累加到同一个对象上。
        循环次数按值捕获更稳妥，即使外部变量后续变化，线程里仍使用创建线程那一刻的固定值，行为更可预期。
        */
        threads.emplace_back([&atomicCounter, &mutexCounter, incrementsPerThread]() {
            for (int j = 0; j < incrementsPerThread; ++j)
            {
                atomicCounter.increment();
                mutexCounter.increment();
            }
        });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    std::cout << "AtomicCounter: " << atomicCounter.getCount() << std::endl;
    std::cout << "MutexCounter: " << mutexCounter.getCount() << std::endl;
}