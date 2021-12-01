#include<ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <chrono>
#include <iostream>
#include <Windows.h>
#include <thread>

#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

using namespace std;
using namespace chrono;

std::wstring calcSessionID()
{
	wstring wsSessionID;
	time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	cout << "t:" << t << endl;

	int64_t  t1 = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	cout << "t1:" << t1 << endl;


	std::stringstream ss;
	ss << std::put_time(std::localtime(&t), "%F %X");
	string strSessionID = ss.str() + "";

	cout << "get time stamp:" << ss.str() << endl;

	return wsSessionID;
}

//duration
void durationTest()
{
	// 以下为5分钟表达
	std::chrono::minutes minute1{ 5 }; // 5个1分钟
	std::chrono::duration<int, std::ratio<5 * 60, 1>> minute2{ 1 }; // 1个5分钟
	std::chrono::duration<double, std::ratio<2 * 60, 1>> minute3{ 2.5 }; // 2.5个2分钟

	std::cout << "minutes1 duration has " << minute1.count() << " ticks\n"
		<< "minutes2 duration has " << minute2.count() << " ticks\n"
		<< "minutes3 duration has " << minute3.count() << " ticks\n";

	// 一下为12小时表达
	std::chrono::hours hours1{ 12 }; // 12个1小时
	std::chrono::duration<double, std::ratio<60 * 60 * 24, 1>> hours2{ 0.5 }; // 0.5个1天

	std::cout << "hours1 duration has " << hours1.count() << " ticks\n"
		<< "hours2 duration has " << hours2.count() << " ticks\n";

	// 使用 std::chrono::duration_cast<T> 将分钟间隔转化成标准秒间隔
	std::cout << "minutes1 duration has " <<
	std::chrono::duration_cast<std::chrono::seconds>(minute1).count() << " seconds\n";
}

//system_clock
void system_clockTest()
{
	std::chrono::duration<int, std::ratio<60 * 60 * 24> > one_day(1);

	// 根据时钟得到现在时间
	std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
	std::time_t time_t_today = std::chrono::system_clock::to_time_t(today);
	std::cout << "now time stamp is " << time_t_today << std::endl;
	std::cout << "now time is " << ctime(&time_t_today) << std::endl;


	// 看看明天的时间
	std::chrono::system_clock::time_point tomorrow = today + one_day;//重载了 + - 运算符
	std::time_t time_t_tomorrow = std::chrono::system_clock::to_time_t(tomorrow);
	std::cout << "tomorrow time stamp is " << time_t_tomorrow << std::endl;
	std::cout << "tomorrow time is " << ctime(&time_t_tomorrow) << std::endl;


	// 计算下个小时时间
	std::chrono::system_clock::time_point next_hour = today + std::chrono::hours(1);
	std::time_t time_t_next_hour = std::chrono::system_clock::to_time_t(next_hour);
	std::chrono::system_clock::time_point next_hour2 = std::chrono::system_clock::from_time_t(time_t_next_hour);

	std::time_t time_t_next_hour2 = std::chrono::system_clock::to_time_t(next_hour2);
	std::cout << "next hour time stamp is " << time_t_next_hour2 << std::endl;
	std::cout << "next hour time is " << ctime(&time_t_next_hour2) << std::endl;
}

//steady_clock
void steady_clockTest()
{
	// 先记录程序运行时间
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

	volatile int nDstVal = 1, nSrcVal = 0;
	for (int i = 0; i < 1000000000; ++i)
		nDstVal = nSrcVal;

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	// 做差值计算耗时
	std::chrono::duration<double> duration_cost = std::chrono::duration_cast<
		std::chrono::duration<double>>(end - start);
	std::cout << "total cost " << duration_cost.count() << " seconds." << std::endl;

	
	typedef std::chrono::duration<int, std::milli> duration_ms;

	duration_ms cost = std::chrono::duration_cast<duration_ms>(end - start);
	std::cout << "total cost " << cost.count() << " ms." << std::endl;
	if (cost < (duration_ms)500) {
		cout << "< 500" << endl;
	}
}

//time_point
void time_pointTest()
{
	//system_clock::time_point
	system_clock::time_point now1 = system_clock::now();
	auto systemNow = system_clock::now();
	cout << chrono::duration_cast<chrono::hours>(systemNow.time_since_epoch()).count() << endl;

	//steady_clock::time_point
	steady_clock::time_point now2 = steady_clock::now();
	auto steadyNow = steady_clock::now();
	cout << chrono::duration_cast<chrono::hours>(steadyNow.time_since_epoch()).count() << endl;

	// 获得epoch 和 now 的时间点
	std::chrono::time_point<std::chrono::system_clock> epoch =
		std::chrono::time_point<std::chrono::system_clock>{};
	std::chrono::time_point<std::chrono::system_clock> now =
		std::chrono::system_clock::now();

	// 显示时间点对应的日期和时间
	time_t epoch_time = std::chrono::system_clock::to_time_t(epoch);
	std::cout << "epoch: " << std::ctime(&epoch_time);
	time_t today_time = std::chrono::system_clock::to_time_t(now);
	std::cout << "today: " << std::ctime(&today_time);

	// 显示duration的值
	std::cout << "seconds since epoch: "
		<< std::chrono::duration_cast<std::chrono::seconds>(epoch.time_since_epoch()).count()
		<< std::endl;

	std::cout << "today, ticks since epoch: "
		<< now.time_since_epoch().count()
		<< std::endl;

	std::cout << "today, hours since epoch: "
		<< std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count()
		<< std::endl;

}

//c++20
void test20() {
	//日历
	year y{ 2021 };
	month_weekday mwd = May / Thursday[3];

	year_month_day ProgrammerDay{ January / 7d / 2021 };
	cout << format("International Programmer's day:{%F}", ProgrammerDay);
}

//计时器混用
void mixUseError()
{
	while (true) {
		auto t1 = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
		auto t2 = time(0);

		if (t1/1000 != t2) cout << t1 << " " << t2 << endl;
	}
}

#include <condition_variable>
//bool pop(unsigned int timeout_ms, T* pData = nullptr)
//{
//	std::unique_lock<std::mutex> scopeLock(mutex_);
//	auto rel_time = std::chrono::milliseconds(timeout_ms);
//
//	// 是否等待超时
//	bool boWaitResult = conditionVariable_.wait_for(scopeLock, rel_time, [this] {  return !queue_.empty(); });
//
//	if (boWaitResult) { // 未超时
//		if (!queue_.empty()) {
//			if (pData)  *pData = std::move(queue_.front());
//		}
//		queue_.pop();
//		return true;
//	}
//	return false;
//}

//多媒体定时回调
void CALLBACK MMTimerCB(UINT uID, UINT uMsg, DWORD dwUsers, DWORD dw1, DWORD dw2)
{
	//添加你的代码
}

//多媒体定时原型
//uDelay：以毫秒指定时的周期
//Uresolution：以毫秒指定延时的精度，数值越小定时器事件分辨率越高。缺省值为1ms。
//LpTimeProc：指向一个回调函数，该回调函数包含需要定时执行的代码。
//DwUser：存放用户提供的回调数据。
//FuEvent：指定定时器事件类型：
//TIME_ONESHOT：uDelay毫秒后只产生一次事件。
//TIME_PERIODIC ：每隔uDelay毫秒周期性地产生事件。
//MMRESULT timeSetEvent(UINT uDelay, UINT uResolution, LPTIMECALLBACK lpTimeProc, WORD dwUser, UINT fuEvent)

//多媒体定时器测试
void MMTimerTest()
{
	UINT TimerID = timeSetEvent(1, 1,(LPTIMECALLBACK)MMTimerCB, NULL, TIME_PERIODIC);
}

//chrono 定时触发
void OneMilliSecondProc()
{
	int64_t  t1 = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
	cout << "t1:" << t1 << endl;
}

// chrono 定时器函数
int Timer11()
{
	int iCount = 0;
	chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();
	auto dt_micri = std::chrono::microseconds(10000);  // 10ms
	while (iCount<10000) {
		tp += dt_micri;
		this_thread::sleep_until(tp);

		//应用写在这函数里
		OneMilliSecondProc();
		iCount++;
	}

	return(0);
}

//chrono 定时器测试
void testChronoTimer()
{
	thread thd(Timer11);  // 创建线程
	thd.detach();
}

/*------------------------------GetLocalTime------------------------------------*/
#include <atlstr.h>
void GetLocalTimeTest()
{
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	WORD wYear = stTime.wYear;
	WORD wMonth = stTime.wMonth;
	WORD wDay = stTime.wDay;
	WORD wHour = stTime.wHour;
	WORD wMinute = stTime.wMinute;
	WORD wSecond = stTime.wSecond;
	WORD wMillSecond = stTime.wMilliseconds;
	cout << endl;
	cout << "GetLocalTime:" << wYear << "年" << wMonth << "月" << wDay << "日" << wHour << "时" << wMinute << "分" << wSecond << "秒" << wMillSecond << "毫秒" << endl << endl;

	SYSTEMTIME stUTCTime;
	GetSystemTime(&stUTCTime);
	cout << "GetSystemTime:" << stUTCTime.wYear << "年" << stUTCTime.wMonth << "月" << stUTCTime.wDay << "日" << stUTCTime.wHour
		<< "时" << stUTCTime.wMinute << "分" << stUTCTime.wSecond << "秒" << stUTCTime.wMilliseconds << "毫秒" << endl << endl;
}



/*------------------------------GetTickCount------------------------------------*/
void getTickCountTest()
{
	//获得Windows启动时刻到当前时刻的时间，单位为毫秒 
	//返回32位的无符号整数，Windows连续运行49.710天后，它将再次从零开始计时。
	auto getTick = GetTickCount();
	cout << "getTick32:" << getTick << endl;

	//返回一个64位的无符号整数。Windows连续运行5.8亿年后，其计时才会归零。
	auto getTick64 = GetTickCount64();
	cout << "getTick64:" << getTick64 << endl;
}

/*------------------------------timeGetTime------------------------------------*/
//多媒体定时器函数 timeGetTime的参数、返回值、作用与GetTickCount完全一致。只是它的精度比GetTickCount要高
void timeGetTimeTest()
{
	timeBeginPeriod(1);        //提高timeGetTime的精度

	auto getTime = timeGetTime();
	cout << "getTime:" << getTime << endl;

	timeEndPeriod(1);         //必须与timeBeginPeriod成对出现
}


/*------------------------------QueryPerformanceFrequency------------------------------------*/

/*
* @name QueryPerformanceFrequency
* @brief Win32API 返回硬件支持的高精度计数器的频率
* @brief 数据类型LARGEINTEGER既可以是一个作为8字节长的整数，也可以是作为两个4字节长的整数的联合结构，其具体用法根据编译器是否支持64位而定。
* @return 返回0表示不支持 在运行 Windows XP 或更高版本的系统上，该函数将始终成功 
*/

//秒表 计时
void queryQPCTest()
{
	//初始化计时器
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime, nEndTime;
	double time = 0;

	//操作系统的性能统计分辨率，也就是每秒钟统计多少次的意思
	QueryPerformanceFrequency(&nFreq);
	cout << "nFreq:" << nFreq.QuadPart << endl;
	cout << "Accuracy:" << (double)(1 / (double)nFreq.QuadPart) << " S" << endl;

	//取开始执行时间
	QueryPerformanceCounter(&nBeginTime);

	//这里填写需要执行的代码
	Sleep(5);

	//取结束执行时间
	QueryPerformanceCounter(&nEndTime);
	//计算时间差
	time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) *1000 / (double)nFreq.QuadPart;
	cout << "Time cost(ms):" << time << endl;
}

//定时执行
void sleepTest()
{
	//设置时间间隔，单位：ms
	const int nIntervalTime = 40;
	//初始化计时器
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime, nEndTime;
	double time = 0, Residual = 0, dThisTime = nIntervalTime;
	QueryPerformanceFrequency(&nFreq);
	while (true)
	{
		//取开始执行时间
		QueryPerformanceCounter(&nBeginTime);

		//这里填写需要周期执行的程序
		//DoSomething
		
		do
		{
			//取结束执行时间
			QueryPerformanceCounter(&nEndTime);
			//计算时间差
			time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) * 1000 / (double)nFreq.QuadPart;
			//通过线程的等待来降低循环次数，达到减少CPU占用的目的
			if (time + 1 < dThisTime)
				this_thread::sleep_for(chrono::milliseconds((int)(dThisTime - time - 1)));
		} while (time < dThisTime);//当时间小于本次预计的时间时继续循环

		//残余时间可能为正也可能为负
		Residual = dThisTime - time;
		//根据设置时间间隔和上一次执行的残余时间，计算下一次执行的时间
		dThisTime = nIntervalTime + Residual;

	}
}

/*------------------------------精度比较------------------------------------*/
void accuracyCompare()
{
	timeBeginPeriod(1);        //提高timeGetTime的精度
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime, nEndTime;
	QueryPerformanceFrequency(&nFreq);
	typedef std::chrono::duration<int, std::micro> duration_micro;
	typedef std::chrono::duration<int, std::nano> duration_nano;

	int iCount = 10;
	while (iCount--) {
		auto getTick0 = GetTickCount();
		auto getTime0 = timeGetTime();
		QueryPerformanceCounter(&nBeginTime);
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

		Sleep(10);

		auto getTick1 = GetTickCount();
		auto getTime1 = timeGetTime();
		QueryPerformanceCounter(&nEndTime);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		duration_micro cost_micro = std::chrono::duration_cast<duration_micro>(end - start);
		duration_nano cost_nano = std::chrono::duration_cast<duration_nano>(end - start);

		cout << "GetTickCount:" << getTick1 - getTick0 << endl;
		cout << "timeGetTime:" << getTime1 - getTime0 << endl;
		cout << "QueryPerformanceCounter:" << (double)(nEndTime.QuadPart - nBeginTime.QuadPart) * 1000 / (double)nFreq.QuadPart << endl;
		cout << "chrono micro:" << (double)cost_micro.count()/1000 << endl;
		cout << "chrono nano:" << (double)cost_nano.count() / 1000000 << endl;
		cout << "--------------------------------------" << endl;
		cout << "--------------------------------------" << endl;
	}
	timeEndPeriod(1);         //必须与timeBeginPeriod成对出现
}


/*------------------------------RDTSC（Read Time Stamp Counter）------------------------------------*/
//inline unsigned __int64 GetCycleCount()
//{
//	__asm _emit 0x0F
//	__asm _emit 0x31
//}
//
//void RDTSCTest()
//{
//	auto c1 = GetCycleCount();
//	Sleep(5);
//	auto c2 = GetCycleCount();
//
//	cout << "time count:" << (c2 - c1) << endl;
//}

/*------------------------------Sleep------------------------------------*/


void mySleep(int iSleepTime)
{
	this_thread::sleep_for(chrono::milliseconds(iSleepTime));
}

void mySleepTest()
{
	auto start = std::chrono::steady_clock::now().time_since_epoch();
	cout << "start time:" << start << endl;
	mySleep(50);
	auto stop = std::chrono::steady_clock::now().time_since_epoch();
	cout << "stop time:" << stop << endl;
	cout << "mySleep cost time:" << std::chrono::duration_cast<chrono::milliseconds>(stop - start) << endl;

	start = std::chrono::steady_clock::now().time_since_epoch();
	Sleep(50);
	stop = std::chrono::steady_clock::now().time_since_epoch();
	cout << "Sleep cost time:" << std::chrono::duration_cast<chrono::milliseconds>(stop - start) << endl;
}


/*------------------------------timed_mutex ------------------------------------*/
#include <mutex>
std::mutex mtx;
std::timed_mutex timedMtx;

void fireworks() {
	// waiting to get a lock: each thread prints "-" every 200ms:
	while (!timedMtx.try_lock_for(std::chrono::milliseconds(200))) {
		std::cout << "-";
	}
	// got a lock! - wait for 1s, then this thread prints "*"
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout << "*\n";
	timedMtx.unlock();
}

void fireworksTest()
{
	std::thread threads[5];
	// 开启 5 个线程:
	for (int i = 0; i < 5; ++i)
		threads[i] = std::thread(fireworks);

	for (auto& th : threads) 
		th.join();

}


std::timed_mutex MtxUntil;

void f() {
	auto now = steady_clock::now();
	if (MtxUntil.try_lock_until(now + 5s)) {
		std::cout << "get lock." << std::endl;
	}
	else {
		std::cout << "try_lock_util timeout." << std::endl;
	}
}

void lockUntilTest()
{
	// try_lock_until
	auto start = steady_clock::now();
	std::lock_guard<std::timed_mutex> lock(MtxUntil);
	std::thread th(f);
	th.join();
	auto end = steady_clock::now();
	std::cout << "time use: " << duration_cast<milliseconds>(end - start).count()
		<< "ms." << std::endl;
}

int main()
{
	//durationTest();
	//system_clockTest();
	//steady_clockTest();
	//time_pointTest();

	//test20();

	//mixUseError();

	// 调用的例子
	//testChronoTimer();

	//getTickCountTest();

	//timeGetTimeTest();

	//GetLocalTimeTest();

	//queryQPCTest();

	accuracyCompare();

	//mySleepTest();

	//fireworksTest();

	//lockUntilTest();

	system("pause");
	return 0;
}