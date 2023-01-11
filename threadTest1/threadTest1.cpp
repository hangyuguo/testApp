// threadTest1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <thread>
#include <mutex>
#include <assert.h>
#include <Windows.h>

using namespace std;
mutex g_mutex;

int g_sum = 0;

//使用普通的锁
void doSomething() {
	int i = 0;
	while (i < 1000) {
		i++;
		g_mutex.lock();
		g_sum++;
		cout << "doSomething:" << g_sum << endl;
		g_mutex.unlock();
	}
}

//使用 RAII 
void doSomethingRAII() {
	int i = 0;
	while (i < 1000) {
		{
			i++;
			lock_guard<mutex> myLockguard(g_mutex);
			g_sum++;
			cout << "doSomethingRAII:" << g_sum << endl;
		}
	}
}

void thTest2() {
	thread t(doSomething);
	thread t1(doSomethingRAII);
	t.join();
	t1.join();
}



void add(int a, int b) {
    g_mutex.try_lock();
    g_sum = a + b;
    g_mutex.unlock();
    cout << a << "+" << b << "=" << g_sum << endl;

}

void thTest1() {
    thread t(add, 1, 2);
    thread t1(add, 2, 3);
    t.join();
    t1.join();
}

void getNextMeetingInfoCheckTime()
{
	int delay_minute = 0, delay_sec = 0;
	int minutePoint[] = { 25, 55, 85 };
	struct tm* nowtime;
	time_t long_time;
	time(&long_time); //Get time as long integer
	nowtime = localtime(&long_time);
	int time_minute = nowtime->tm_min;//得到当前时间的分钟
	int time_sec = nowtime->tm_sec;//得到当前时间的秒数

	for (int i = 0; i < 3; i++) {
		if (time_minute < minutePoint[i]) {
			delay_minute = minutePoint[i] - time_minute;
			break;
		}
	}

	delay_sec = delay_minute * 60 - time_sec;
	delay_sec = -1;

	assert(delay_sec > 0);

	Sleep(10000);

	cout << "delay_sec:" << delay_sec;
}


#include<queue>

queue<int> q;
std::mutex mu;

void producer() {
	int products = 10;
	while (products > 0) {
		unique_lock<mutex> _mu(mu);
		q.push(products);
		_mu.unlock();
		this_thread::sleep_for(chrono::seconds(1));
		products--;
	}
}

void customer() {
	int data = 0;
	// 最后一个产品序号是 1
	while (data != 1) {
		std::unique_lock<std::mutex> locker(mu);
		if (!q.empty()) {
			data = q.front();
			q.pop();
			locker.unlock();
			std::cout << "t2 got a value from t1: " << data << std::endl;
		}
		else {
			locker.unlock();
		}
	}
}


//任务队列
queue<int> products;
mutex Mutex;
condition_variable cond;
bool notify = false;	// 库中还有无产品
bool done = false;		// 生产者是否还继续生产

void producerCV() {
	int i;
	for (i = 0; i < 100; i++) {
		//主动让出cpu，不参与cpu 的本次调度，让其他线程使用,等一秒后再参与调度
		this_thread::sleep_for(chrono::seconds(1));


		unique_lock<mutex> locker(Mutex);

		//如果仓库中还有产品,就等待消费者消费完后在生产
		while (notify || !products.empty()) {
			cond.wait(locker);	//被唤醒后，将向下继续执行。
		}

		//当前仓库里面没有东西了,就将产品装入仓库
		products.push(i);
		cout << "producer..." << i << endl;
		//设置有产品的通知
		notify = true;
		//通知消费者可以取产品了
		cond.notify_all();

	}

	//通知消费者端不生产了
	done = true;
	cond.notify_all();
	return;
}

void customerCV() {

	while (!done) {
		unique_lock<mutex> locker(Mutex);
		//等待生产者者通知有资源
		while (!notify) {
			cond.wait(locker);
		}

		//要是队列不为空的话
		while (!products.empty()) {
			cout << "consumer..." << _threadid << ":" << products.front() << endl;

			products.pop();
		}

		//通知生产者仓库容量不足,生产产品
		notify = false;
		// 唤醒 cond 中 wait 的线程
		cond.notify_one();
	}
}


int main()
{
	
	return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
