#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>


using namespace std;

//ex1 thread join-----------------------------
void sayHello(string name) {
	this_thread::sleep_for(chrono::seconds(1));
	cout << "Hello 123" << name << endl;
}

void helloWorld() {
	this_thread::sleep_for(chrono::seconds(2));
	cout << "Hello world!456" << endl;
}

void thTest() {
	thread th1(sayHello, "Tom");
	thread th2(helloWorld);
	/*th1.join();
	th2.join();*/
}

//ex2 yield -----------------------------

typedef std::chrono::steady_clock                STEADY_CLOCK2;
typedef std::chrono::steady_clock::time_point    TIME_POINT2;

void print_diff_time2(const TIME_POINT2& t1, const TIME_POINT2& t2)
{
	std::chrono::duration<double, std::milli> dTimeSpan = std::chrono::duration<double, std::milli>(t2 - t1);

	std::cout << "time span : " << dTimeSpan.count() << "ms\n" << endl << endl;
}


std::mutex g_mutex;
std::atomic<bool> ready(false);

void count1m(int id)
{
	while (!ready)// wait until main() sets ready... 
	{
		//若线程还有没创建的，将当前线程分配的cpu时间片，让调度器安排给其他线程，
		//由于使用了yield函数，在 not Ready 情况下，避免了空循环，在一定程度上，可以提高cpu的利用率
		std::this_thread::yield();
	}

	for (int i = 0; i < 1000000; ++i) {}

	std::this_thread::sleep_for(chrono::milliseconds(1000));

	std::lock_guard<std::mutex> lock(g_mutex);
	std::cout << "thread : " << id << std::endl;
}

void yieldTest() {
	std::thread threads[10];

	std::cout << "race of 10 threads that count to 1 million:\n";

	TIME_POINT2 t11 = STEADY_CLOCK2::now();

	for (int i = 0; i < 10; ++i)
	{
		threads[i] = std::thread(count1m, i);
	}

	ready = true;               // go!

	for (auto& th : threads)
	{
		th.join();
	}

	std::cout << '\n';

	TIME_POINT2 t22 = STEADY_CLOCK2::now();

	print_diff_time2(t11, t22);
}

//ex3  call_once-----------------------------

void init() {
	cout << "Initialing..." << endl;
	// Do something...
}

void worker(once_flag* flag) {
	call_once(*flag, init);
}

void callOnceTest() {
	once_flag flag;

	thread t1(worker, &flag);
	thread t2(worker, &flag);
	thread t3(worker, &flag);

	t1.join();
	t2.join();
	t3.join();
}


//ex4  -----------------------------
#include <condition_variable>
condition_variable g_cv;


void task(int a, int b, int& ret) {
	unique_lock<std::mutex> lock(g_mutex);
	ret = a * a + b * b;
	g_cv.notify_one();
}

//int main()
//{
//	int ret = 0;
//	std::thread t(task, 1, 2, std::ref(ret));
//	//do something
//	unique_lock<std::mutex> lock(g_mutex);
//	g_cv.wait(lock);
//	cout << "ret:" << ret << endl;
//
//	t.join();
//	return 0;
//}

//ex5  -----------------------------
#include <future>


void task2(int a, int b, std::promise<int>& ret) {
	unique_lock<std::mutex> lock(g_mutex);
	ret.set_value(a * a + b * b);
}

void task3(int a, std::future<int>& b, std::promise<int>& ret) {
	unique_lock<std::mutex> lock(g_mutex);
	int b_in = b.get();
	ret.set_value(a * a + b_in * b_in);
}

//int main()
//{
//	int ret = 0;
//
//	std::promise<int> p;
//	std::future<int> f = p.get_future();
//
//	std::promise<int> p_in;
//	std::future<int> f_in = p_in.get_future();
//
//	//std::thread t1(task2, 1, 2, std::ref(p));
//	std::thread t2(task3, 1, std::ref(f_in), std::ref(p));
//
//	//std::this_thread::sleep_for(chrono::seconds(3));
//	p_in.set_value(3);
//
//	cout << "ret:" << f.get() << endl;
//
//	//t1.join();
//	t2.join();
//	
//
//	return 0;
//}

//ex6 -----------------------------

#include <algorithm>
#include <numeric>
#include <random>
#include <execution>

void generateRandomData(vector<double>& collection, int size) {
	random_device rd;
	mt19937 mt(rd());
	uniform_real_distribution<double> dist(1.0, 100.0);
	for (int i = 0; i < size; i++) {
		collection.push_back(dist(mt));
	}
}

int main() {
	vector<double> collection;
	generateRandomData(collection, 10e6); // ①

	vector<double> copy1(collection); // ②
	vector<double> copy2(collection);
	vector<double> copy3(collection);

	auto time1 = chrono::steady_clock::now(); // ③
	sort(execution::seq, copy1.begin(), copy1.end()); // ④
	auto time2 = chrono::steady_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(time2 - time1).count();
	cout << "Sequenced sort consuming " << duration << "ms." << endl; // ⑤

	auto time3 = chrono::steady_clock::now();
	sort(execution::par, copy2.begin(), copy2.end()); // ⑥
	auto time4 = chrono::steady_clock::now();
	duration = chrono::duration_cast<chrono::milliseconds>(time4 - time3).count();
	cout << "Parallel sort consuming " << duration << "ms." << endl;

	auto time5 = chrono::steady_clock::now();
	sort(execution::par_unseq, copy2.begin(), copy2.end()); // ⑦
	auto time6 = chrono::steady_clock::now();
	duration = chrono::duration_cast<chrono::milliseconds>(time6 - time5).count();
	cout << "Parallel unsequenced sort consuming " << duration << "ms." << endl;
}