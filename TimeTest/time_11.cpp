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
	// ����Ϊ5���ӱ��
	std::chrono::minutes minute1{ 5 }; // 5��1����
	std::chrono::duration<int, std::ratio<5 * 60, 1>> minute2{ 1 }; // 1��5����
	std::chrono::duration<double, std::ratio<2 * 60, 1>> minute3{ 2.5 }; // 2.5��2����

	std::cout << "minutes1 duration has " << minute1.count() << " ticks\n"
		<< "minutes2 duration has " << minute2.count() << " ticks\n"
		<< "minutes3 duration has " << minute3.count() << " ticks\n";

	// һ��Ϊ12Сʱ���
	std::chrono::hours hours1{ 12 }; // 12��1Сʱ
	std::chrono::duration<double, std::ratio<60 * 60 * 24, 1>> hours2{ 0.5 }; // 0.5��1��

	std::cout << "hours1 duration has " << hours1.count() << " ticks\n"
		<< "hours2 duration has " << hours2.count() << " ticks\n";

	// ʹ�� std::chrono::duration_cast<T> �����Ӽ��ת���ɱ�׼����
	std::cout << "minutes1 duration has " <<
	std::chrono::duration_cast<std::chrono::seconds>(minute1).count() << " seconds\n";
}

//system_clock
void system_clockTest()
{
	std::chrono::duration<int, std::ratio<60 * 60 * 24> > one_day(1);

	// ����ʱ�ӵõ�����ʱ��
	std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
	std::time_t time_t_today = std::chrono::system_clock::to_time_t(today);
	std::cout << "now time stamp is " << time_t_today << std::endl;
	std::cout << "now time is " << ctime(&time_t_today) << std::endl;


	// ���������ʱ��
	std::chrono::system_clock::time_point tomorrow = today + one_day;//������ + - �����
	std::time_t time_t_tomorrow = std::chrono::system_clock::to_time_t(tomorrow);
	std::cout << "tomorrow time stamp is " << time_t_tomorrow << std::endl;
	std::cout << "tomorrow time is " << ctime(&time_t_tomorrow) << std::endl;


	// �����¸�Сʱʱ��
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
	// �ȼ�¼��������ʱ��
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

	volatile int nDstVal = 1, nSrcVal = 0;
	for (int i = 0; i < 1000000000; ++i)
		nDstVal = nSrcVal;

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	// ����ֵ�����ʱ
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

	// ���epoch �� now ��ʱ���
	std::chrono::time_point<std::chrono::system_clock> epoch =
		std::chrono::time_point<std::chrono::system_clock>{};
	std::chrono::time_point<std::chrono::system_clock> now =
		std::chrono::system_clock::now();

	// ��ʾʱ����Ӧ�����ں�ʱ��
	time_t epoch_time = std::chrono::system_clock::to_time_t(epoch);
	std::cout << "epoch: " << std::ctime(&epoch_time);
	time_t today_time = std::chrono::system_clock::to_time_t(now);
	std::cout << "today: " << std::ctime(&today_time);

	// ��ʾduration��ֵ
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
	//����
	year y{ 2021 };
	month_weekday mwd = May / Thursday[3];

	year_month_day ProgrammerDay{ January / 7d / 2021 };
	cout << format("International Programmer's day:{%F}", ProgrammerDay);
}

//��ʱ������
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
//	// �Ƿ�ȴ���ʱ
//	bool boWaitResult = conditionVariable_.wait_for(scopeLock, rel_time, [this] {  return !queue_.empty(); });
//
//	if (boWaitResult) { // δ��ʱ
//		if (!queue_.empty()) {
//			if (pData)  *pData = std::move(queue_.front());
//		}
//		queue_.pop();
//		return true;
//	}
//	return false;
//}

//��ý�嶨ʱ�ص�
void CALLBACK MMTimerCB(UINT uID, UINT uMsg, DWORD dwUsers, DWORD dw1, DWORD dw2)
{
	//�����Ĵ���
}

//��ý�嶨ʱԭ��
//uDelay���Ժ���ָ��ʱ������
//Uresolution���Ժ���ָ����ʱ�ľ��ȣ���ֵԽС��ʱ���¼��ֱ���Խ�ߡ�ȱʡֵΪ1ms��
//LpTimeProc��ָ��һ���ص��������ûص�����������Ҫ��ʱִ�еĴ��롣
//DwUser������û��ṩ�Ļص����ݡ�
//FuEvent��ָ����ʱ���¼����ͣ�
//TIME_ONESHOT��uDelay�����ֻ����һ���¼���
//TIME_PERIODIC ��ÿ��uDelay���������Եز����¼���
//MMRESULT timeSetEvent(UINT uDelay, UINT uResolution, LPTIMECALLBACK lpTimeProc, WORD dwUser, UINT fuEvent)

//��ý�嶨ʱ������
void MMTimerTest()
{
	UINT TimerID = timeSetEvent(1, 1,(LPTIMECALLBACK)MMTimerCB, NULL, TIME_PERIODIC);
}

//chrono ��ʱ����
void OneMilliSecondProc()
{
	int64_t  t1 = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
	cout << "t1:" << t1 << endl;
}

// chrono ��ʱ������
int Timer11()
{
	int iCount = 0;
	chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();
	auto dt_micri = std::chrono::microseconds(10000);  // 10ms
	while (iCount<10000) {
		tp += dt_micri;
		this_thread::sleep_until(tp);

		//Ӧ��д���⺯����
		OneMilliSecondProc();
		iCount++;
	}

	return(0);
}

//chrono ��ʱ������
void testChronoTimer()
{
	thread thd(Timer11);  // �����߳�
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
	cout << "GetLocalTime:" << wYear << "��" << wMonth << "��" << wDay << "��" << wHour << "ʱ" << wMinute << "��" << wSecond << "��" << wMillSecond << "����" << endl << endl;

	SYSTEMTIME stUTCTime;
	GetSystemTime(&stUTCTime);
	cout << "GetSystemTime:" << stUTCTime.wYear << "��" << stUTCTime.wMonth << "��" << stUTCTime.wDay << "��" << stUTCTime.wHour
		<< "ʱ" << stUTCTime.wMinute << "��" << stUTCTime.wSecond << "��" << stUTCTime.wMilliseconds << "����" << endl << endl;
}



/*------------------------------GetTickCount------------------------------------*/
void getTickCountTest()
{
	//���Windows����ʱ�̵���ǰʱ�̵�ʱ�䣬��λΪ���� 
	//����32λ���޷���������Windows��������49.710��������ٴδ��㿪ʼ��ʱ��
	auto getTick = GetTickCount();
	cout << "getTick32:" << getTick << endl;

	//����һ��64λ���޷���������Windows��������5.8��������ʱ�Ż���㡣
	auto getTick64 = GetTickCount64();
	cout << "getTick64:" << getTick64 << endl;
}

/*------------------------------timeGetTime------------------------------------*/
//��ý�嶨ʱ������ timeGetTime�Ĳ���������ֵ��������GetTickCount��ȫһ�¡�ֻ�����ľ��ȱ�GetTickCountҪ��
void timeGetTimeTest()
{
	timeBeginPeriod(1);        //���timeGetTime�ľ���

	auto getTime = timeGetTime();
	cout << "getTime:" << getTime << endl;

	timeEndPeriod(1);         //������timeBeginPeriod�ɶԳ���
}


/*------------------------------QueryPerformanceFrequency------------------------------------*/

/*
* @name QueryPerformanceFrequency
* @brief Win32API ����Ӳ��֧�ֵĸ߾��ȼ�������Ƶ��
* @brief ��������LARGEINTEGER�ȿ�����һ����Ϊ8�ֽڳ���������Ҳ��������Ϊ����4�ֽڳ������������Ͻṹ��������÷����ݱ������Ƿ�֧��64λ������
* @return ����0��ʾ��֧�� ������ Windows XP ����߰汾��ϵͳ�ϣ��ú�����ʼ�ճɹ� 
*/

//��� ��ʱ
void queryQPCTest()
{
	//��ʼ����ʱ��
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime, nEndTime;
	double time = 0;

	//����ϵͳ������ͳ�Ʒֱ��ʣ�Ҳ����ÿ����ͳ�ƶ��ٴε���˼
	QueryPerformanceFrequency(&nFreq);
	cout << "nFreq:" << nFreq.QuadPart << endl;
	cout << "Accuracy:" << (double)(1 / (double)nFreq.QuadPart) << " S" << endl;

	//ȡ��ʼִ��ʱ��
	QueryPerformanceCounter(&nBeginTime);

	//������д��Ҫִ�еĴ���
	Sleep(5);

	//ȡ����ִ��ʱ��
	QueryPerformanceCounter(&nEndTime);
	//����ʱ���
	time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) *1000 / (double)nFreq.QuadPart;
	cout << "Time cost(ms):" << time << endl;
}

//��ʱִ��
void sleepTest()
{
	//����ʱ��������λ��ms
	const int nIntervalTime = 40;
	//��ʼ����ʱ��
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime, nEndTime;
	double time = 0, Residual = 0, dThisTime = nIntervalTime;
	QueryPerformanceFrequency(&nFreq);
	while (true)
	{
		//ȡ��ʼִ��ʱ��
		QueryPerformanceCounter(&nBeginTime);

		//������д��Ҫ����ִ�еĳ���
		//DoSomething
		
		do
		{
			//ȡ����ִ��ʱ��
			QueryPerformanceCounter(&nEndTime);
			//����ʱ���
			time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) * 1000 / (double)nFreq.QuadPart;
			//ͨ���̵߳ĵȴ�������ѭ���������ﵽ����CPUռ�õ�Ŀ��
			if (time + 1 < dThisTime)
				this_thread::sleep_for(chrono::milliseconds((int)(dThisTime - time - 1)));
		} while (time < dThisTime);//��ʱ��С�ڱ���Ԥ�Ƶ�ʱ��ʱ����ѭ��

		//����ʱ�����Ϊ��Ҳ����Ϊ��
		Residual = dThisTime - time;
		//��������ʱ��������һ��ִ�еĲ���ʱ�䣬������һ��ִ�е�ʱ��
		dThisTime = nIntervalTime + Residual;

	}
}

/*------------------------------���ȱȽ�------------------------------------*/
void accuracyCompare()
{
	timeBeginPeriod(1);        //���timeGetTime�ľ���
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
	timeEndPeriod(1);         //������timeBeginPeriod�ɶԳ���
}


/*------------------------------RDTSC��Read Time Stamp Counter��------------------------------------*/
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
	// ���� 5 ���߳�:
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

	// ���õ�����
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