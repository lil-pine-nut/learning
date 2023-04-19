#include <iostream>
#include <string>
#include "ThreadPool.h"
using namespace std;

void fun1(int slp)
{
	printf("  hello, fun1 !  %d\n" ,std::this_thread::get_id());
	if (slp>0) {
		printf(" ======= fun1 sleep %d  =========  %d\n",slp, std::this_thread::get_id());
		std::this_thread::sleep_for(std::chrono::milliseconds(slp));
		//Sleep(slp );
	}
}

int main()
{
    std::mutex mtx;
    try
    {
        zl::ThreadPool tp;
        std::vector<std::future<int>> v;
        std::vector<std::future<void>> v1;
        std::future<void> ff = tp.add(fun1, 10);
        for (int i = 0; i <= 10; ++i)
        {
            auto ans = tp.add([&mtx](int answer) 
            { 
                return answer; 
            }, i);
            v.push_back(std::move(ans));
        }
        for (int i = 0; i <= 5; ++i)
        {
            auto ans = tp.add([&mtx](const std::string& str1, const std::string& str2)
            {
                std::lock_guard<std::mutex> lg(mtx);
                std::cout << (str1 + str2) << std::endl;
                return;
            }, "hello ", "world");
            v1.push_back(std::move(ans));
        }
        for (size_t i = 0; i < v.size(); ++i)
        {
            std::lock_guard<std::mutex> lg(mtx);
            cout << "v[" << i << "].get() = " << v[i].get() << endl;
        }
        for (size_t i = 0; i < v1.size(); ++i)
        {
            std::cout<<"####################v1[i].get()"<<std::endl;
            // std::lock_guard<std::mutex> lg(mtx);//这里加锁会导致死锁：相当于加锁后再加锁打印hello world。
            v1[i].get();//此运行总是在线程池打印hello world线程之前
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

}