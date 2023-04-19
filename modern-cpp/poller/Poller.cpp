// =====================================================================================
//
//       Filename:  Poller.cpp
//
//    Description:
//
//        Version:  1.0
//        Created:  05/20/2022 04:15:35 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  zhongshupeng (), media_zsp@qq.com
//        Company:  shinra
//
// =====================================================================================
#include "Poller.h"
#include <iostream>
#include <exception>
#include <unistd.h>
#include <sys/time.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>

static uint64_t getMicroseconds()
{
//两种方式获取运行时间
#if 1
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t now = tv.tv_sec * 1000000LL + tv.tv_usec;
    static uint64_t timeorigin = now;
    return (now - timeorigin);
#else
    auto time_now = chrono::system_clock::now();
    auto duration_in_micro = chrono::duration_cast<chrono::microseconds>(time_now.time_since_epoch());
    return duration_in_micro.count();
#endif
}
static uint64_t getMilliseconds()
{
    return getMicroseconds() / 1000;
}

Poller::Ptr Poller::Instance()
{
    static Ptr s_instance(new Poller());
    return s_instance;
}

Poller::Poller()
{
    event_fd_ = eventfd(0, 0);
    if (event_fd_ == -1)
    {
        perror("eventfd");
        exit(EXIT_FAILURE);
    }
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = event_fd_;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, event_fd_, &ev) == -1)
    {
        perror("epoll_ctl: ");
        exit(EXIT_FAILURE);
    }

    loop_thread_id_ == this_thread::get_id();
    loop_thread_ = new thread(&Poller::runLoop, this);
    cout << "Poller: " << this << endl;
}

Poller::~Poller()
{
    asyncTask([]()
              { throw ExitException(); });
    if (loop_thread_)
    {
        try
        {
            loop_thread_->join();
        }
        catch (...)
        {
        }
        delete loop_thread_;
        loop_thread_ = nullptr;
    }
    if (event_fd_ != -1)
    {
        close(event_fd_);
        event_fd_ = -1;
    }
    if (epoll_fd_ != -1)
    {
        close(epoll_fd_);
        epoll_fd_ = -1;
    }
    cout << "~Poller: " << this << endl;
}

void Poller::doDelayTask(float second,
                         const function<bool()> &cb,
                         bool continueWhenException)
{
    auto task = [cb, second, continueWhenException]()
    {
        try
        {
            if (cb())
            {
                //重复任务
                return (uint64_t)(1000 * second);
            }
            //不重复
            return (uint64_t)0;
        }
        catch (std::exception &ex)
        {
            cout << "delay task error: " << ex.what() << endl;
            return continueWhenException ? (uint64_t)(1000 * second) : 0;
        }
    };

    uint64_t time_line = getMilliseconds() + (uint64_t)(1000 * second);
    asyncTask([time_line, task, this]()
              { delay_task_map_.emplace(time_line, task); });
}

void Poller::runLoop()
{
    loop_thread_id_ == this_thread::get_id();
    uint64_t min_delay;
    struct epoll_event event;
    while (!exit_flag_)
    {
        // doTask
        min_delay = getMinDelay();
        // usleep(min_delay ? (min_delay*1000) : 10000);//TODO 添加任务要能打断这个sleep
        int ret = epoll_wait(epoll_fd_, &event, 1, min_delay ? min_delay : -1);
        if (ret <= 0)
        {
            cout << "timeline..." << endl;
            continue;
        }
        if (event.data.fd == event_fd_)
        {
            cout << "addTask..." << endl;
            uint64_t u;
            read(event_fd_, &u, sizeof(uint64_t));
        }

        // addTask
        decltype(add_task_list_) list_swap;
        {
            lock_guard<mutex> lck(mtx_add_task_);
            list_swap.swap(add_task_list_);
        }
        for (auto it = list_swap.begin(); it != list_swap.end(); ++it)
        {
            try
            {
                (*it)();
            }
            catch (ExitException &)
            {
                exit_flag_ = true;
                break;
            }
            catch (std::exception &ex)
            {
                cout << "add task error: " << ex.what() << endl;
            }
        }
    }
    cout << "runLoop end~" << endl;
}

void Poller::asyncTask(const function<void()> &cb)
{
    if (isCurrentThread())
    {
        cb();
        return;
    }
    lock_guard<mutex> lck(mtx_add_task_);
    add_task_list_.emplace_back(cb);

    //写数据到eventfd,唤醒主线程
    uint64_t u = 1;
    write(event_fd_, &u, sizeof(uint64_t));
}

uint64_t Poller::getMinDelay()
{
    auto it = delay_task_map_.begin();
    if (it == delay_task_map_.end())
    {
        return 0;
    }
    auto now_time = getMilliseconds();
    if (it->first > now_time)
    {
        return it->first - now_time;
    }
    return flushDelayTask(now_time);
}

uint64_t Poller::flushDelayTask(uint64_t now_time)
{
    decltype(delay_task_map_) task_swap;
    task_swap.swap(delay_task_map_);
    for (auto it = task_swap.begin(); it != task_swap.end() && it->first <= now_time; it = task_swap.erase(it))
    {
        //已到期的任务
        auto next_delay = (it->second)();
        if (next_delay)
        {
            //可重复任务,更新时间截止线
            delay_task_map_.emplace(next_delay + now_time, move(it->second));
        }
    }
    task_swap.insert(delay_task_map_.begin(), delay_task_map_.end());
    task_swap.swap(delay_task_map_);

    auto it = delay_task_map_.begin();
    if (it == delay_task_map_.end())
    {
        //没有剩余的定时器了
        return 0;
    }
    //最近一个定时器的执行延时
    return it->first - now_time;
}

bool Poller::isCurrentThread()
{
    return loop_thread_id_ == this_thread::get_id();
}
