/*
要求：

使用 std::condition_variable 实现线程同步
生产者线程生成数据放入队列
消费者线程从队列取出数据处理
队列为空时消费者应等待，不占用 CPU
支持优雅退出
*/

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

// 线程安全阻塞队列：空队列时消费者阻塞等待；close 后可优雅退出。
template <typename T>
class BlockingQueue {
public:
    void push(const T& value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (closed_) {
                return;
            }
            queue_.push(value);
        }
        cv_.notify_one();
    }

    // 返回 false 表示队列已关闭且无数据，消费者可以退出。
    bool wait_and_pop(T& out) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return closed_ || !queue_.empty(); });

        if (queue_.empty()) {
            return false;
        }

        out = queue_.front();
        queue_.pop();
        return true;
    }

    void close() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        cv_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool closed_ = false;
};

class Producer {
public:
    Producer(BlockingQueue<int>& queue, int startValue, int count)
        : queue_(queue), startValue_(startValue), count_(count) {}

    void start() {
        thread_ = std::thread(&Producer::run, this);
    }

    void join() {
        if (thread_.joinable()) {
            thread_.join();
        }
    }

private:
    void run() {
        for (int i = 0; i < count_; ++i) {
            queue_.push(startValue_ + i);
        }
    }

    BlockingQueue<int>& queue_;
    int startValue_;
    int count_;
    std::thread thread_;
};

class Consumer {
public:
    Consumer(BlockingQueue<int>& queue, std::atomic<int>& consumedCount)
        : queue_(queue), consumedCount_(consumedCount) {}

    void start() {
        thread_ = std::thread(&Consumer::run, this);
    }

    void join() {
        if (thread_.joinable()) {
            thread_.join();
        }
    }

private:
    void run() {
        int value = 0;
        while (queue_.wait_and_pop(value)) {
            consume(value);
        }
    }

    void consume(int value) {
        // 这里模拟处理数据；示例里只做统计和轻量打印。
        ++consumedCount_;
        if (value % 500 == 0) {
            std::cout << "Consumed: " << value << std::endl;
        }
    }

    BlockingQueue<int>& queue_;
    std::atomic<int>& consumedCount_;
    std::thread thread_;
};

int main() {
    BlockingQueue<int> queue;
    std::atomic<int> consumedCount{0};

    const int producerCount = 2;
    const int itemsPerProducer = 1000;

    Producer p1(queue, 0, itemsPerProducer);
    Producer p2(queue, 100000, itemsPerProducer);

    Consumer c1(queue, consumedCount);
    Consumer c2(queue, consumedCount);

    c1.start();
    c2.start();
    p1.start();
    p2.start();

    p1.join();
    p2.join();

    // 生产结束后关闭队列，唤醒等待中的消费者并让其退出。
    queue.close();

    c1.join();
    c2.join();

    const int expected = producerCount * itemsPerProducer;
    std::cout << "Expected consumed: " << expected << std::endl;
    std::cout << "Actual consumed: " << consumedCount.load() << std::endl;

    return consumedCount.load() == expected ? 0 : 1;
}
