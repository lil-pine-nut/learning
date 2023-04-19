/*
atomic对int、char、bool等数据结构进行了原子性封装，在多线程环境中，对std::atomic对象的访问不会造成竞争-冒险。
利用std::atomic可实现数据结构的无锁设计。

所谓的原子操作，取的就是“原子是最小的、不可分割的最小个体”的意义，它表示在多个线程访问同一个全局资源的时候，
能够确保所有其他的线程都不在同一时间内访问相同的资源。也就是他确保了在同一时刻只有唯一的线程对这个资源进行访问。
这有点类似互斥对象对共享资源的访问的保护，但是原子操作更加接近底层，因而效率更高。

在以往的C++标准中并没有对原子操作进行规定，我们往往是使用汇编语言，或者是借助第三方的线程库，
例如intel的pthread来实现。在新标准C++11，引入了原子操作的概念，并通过这个新的头文件提供了多种原子操作数据类型，
例如，atomic_bool,atomic_int等等，如果我们在多个线程中对这些类型的共享资源进行操作，
编译器将保证这些操作都是原子性的，也就是说，确保任意时刻只有一个线程对这个资源进行访问，编译器将保证，
多个线程访问这个共享资源的正确性。从而避免了锁的使用，提高了效率。


*/

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>

std::atomic<size_t> count(0);

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
		count++;
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