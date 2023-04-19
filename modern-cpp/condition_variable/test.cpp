/*
std::condition_variable::wait() 介绍

unconditional (1)	void wait (unique_lock<mutex>& lck);
predicate (2)	    template <class Predicate>  void wait (unique_lock<mutex>& lck, Predicate pred);

std::condition_variable 提供了两种 wait() 函数。当前线程调用 wait() 后将被阻塞(此时当前线程应该获得了锁（mutex），
不妨设获得锁 lck)，直到另外某个线程调用 notify_* 唤醒了当前线程。

在线程被阻塞时，该函数会自动调用 lck.unlock() 释放锁，使得其他被阻塞在锁竞争上的线程得以继续执行。
另外，一旦当前线程获得通知(notified，通常是另外某个线程调用 notify_* 唤醒了当前线程)，
wait() 函数也是自动调用 lck.lock()，使得 lck 的状态和 wait 函数被调用时相同。

在第二种情况下（即设置了 Predicate），只有当 pred 条件为 false 时调用 wait() 才会阻塞当前线程，
并且在收到其他线程的通知后只有当 pred 为 true 时才会被解除阻塞。因此第二种情况类似以下代码：
*/

#include <iostream>           // std::cout
#include <thread>             // std::thread, std::this_thread::yield
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
// #include <sstream>

std::mutex mtx;
std::condition_variable cv;

int cargo = 0;
bool shipment_available()
{
    return cargo != 0;
}

// 消费者线程.
void consume(int n)
{
    for (int i = 0; i < n; ++i)
    {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck, shipment_available);
        std::cout << "i = " << i << ", cargo = " << cargo << '\n';
        cargo = 0;
    }
    // std::ostringstream oss;
    // oss << std::this_thread::get_id();
    // std::string stid = oss.str();
    // unsigned long long tid = std::stoull(stid);
    std::cout << "thread " << std::this_thread::get_id() << " end." << std::endl;
}

int main()
{
    std::thread consumer_thread(consume, 10); // 消费者线程.

    // 主线程为生产者线程, 生产 10 个物品.
    for (int i = 0; i < 10; ++i)
    {
        while (shipment_available())
            std::this_thread::yield(); //不用会导致consume的for跑不了10次
        std::unique_lock<std::mutex> lck(mtx);
        cargo = i + 1;
        std::cout << "cargo = i + 1, cargo = " << cargo << std::endl;
        cv.notify_one();
    }

    consumer_thread.join();

    return 0;
}