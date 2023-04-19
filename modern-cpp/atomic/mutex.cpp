#include <iostream>
#include <mutex>
#include <vector>
#include <thread>

std::mutex mtx;
size_t count = 0;

#include <time.h>
#define MICRO_IN_SECOND 1000000
#define NANOS_IN_SECOND 1000000000

/**
 * @brief  获取当前毫秒数
 *
 * @return double 返回当前的毫秒数
 */
double currentTimeInMiliSeconds()
{
	struct timespec res;
	double ret = 0;
	clock_gettime(CLOCK_MONOTONIC, &res);
	ret = (double)(res.tv_sec * NANOS_IN_SECOND + res.tv_nsec) / MICRO_IN_SECOND;

	return ret;
}

void threadFun()
{
	for (int i = 0; i < 1000000; i++)
	{
		// 防止多个线程同时访问同一资源
		std::unique_lock<std::mutex> lock(mtx);
		count++;
	}
}

int main(void)
{
	double start_time = currentTimeInMiliSeconds();

	// 启动多个线程
	std::vector<std::thread> threads;
	for (int i = 0; i < 10; i++)
		threads.push_back(std::thread(threadFun));
	for (auto &thad : threads)
		thad.join();

	// 检测count是否正确 1000000*10 = 10000000
	std::cout << "count number:" << count << std::endl;

	double end_time = currentTimeInMiliSeconds();
	std::cout << "耗时：" << end_time - start_time << "ms" << std::endl;

	return 0;
}