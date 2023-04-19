// =====================================================================================
//
//       Filename:  Poller.h
//
//    Description:
//
//        Version:  1.0
//        Created:  05/20/2022 06:39:16 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  zhongshupeng (), media_zsp@qq.com
//   Organization:
//
// =====================================================================================
#include <mutex>
#include <thread>
#include <map>
#include <list>
#include <functional>

using namespace std;

class Poller
{
public:
    using Ptr = std::shared_ptr<Poller>;
    static Ptr Instance();
    Poller();
    ~Poller();
    void doDelayTask(float second,
                     const function<bool()> &cb,
                     bool continueWhenException = true);

private:
    class ExitException : public std::exception
    {
    public:
        ExitException() {}
        ~ExitException() {}
    };

private:
    thread *loop_thread_ = nullptr;
    thread::id loop_thread_id_;
    bool exit_flag_;
    mutex mtx_add_task_;
    int event_fd_;
    int epoll_fd_;
    multimap<uint64_t, function<uint64_t()>> delay_task_map_;
    list<function<void()>> add_task_list_;
    void runLoop();
    void asyncTask(const function<void()> &cb);
    uint64_t getMinDelay();
    uint64_t flushDelayTask(uint64_t now_time);
    bool isCurrentThread();
};
