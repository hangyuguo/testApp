// async_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <mutex>
#include <memory>
#include <functional>

using namespace std;

void futureTest() {
    //查询future的状态
    std::future_status status;
    std::future<int> fu;
    do {
        status = fu.wait_for(std::chrono::seconds(1));
        if (status == std::future_status::deferred) {
            std::cout << "deferred\n";
        }
        else if (status == std::future_status::timeout) {
            std::cout << "timeout\n";
        }
        else if (status == std::future_status::ready) {
            std::cout << "ready!\n";
        }
    } while (status != std::future_status::ready);
}

void promiseTest() {
    std::promise<int> pr;
    std::thread t([](std::promise<int>& p) { p.set_value_at_thread_exit(9); }, std::ref(pr));
    std::future<int> f = pr.get_future();
    auto r = f.get();
    cout << r << endl;
}

void packaged_taskTest() {
    std::packaged_task<int()> task([]() { return 7; });
    std::thread t1(std::ref(task));
    std::future<int> f1 = task.get_future();
    auto r1 = f1.get();
    cout << r1 << endl;
}

/*
* std::async先将异步操作用std::packaged_task包 装起来，
* 然后将异步操作的结果放到std::promise中，这个过程就是创造未来的过程。
* 外面再通过future.get/wait来获取这个未来的 结果
* 原型：async(std::launch::async | std::launch::deferred, f, args...)
* std::launch::async 调用async时就开始创建线程 默认
* std::launch::deferred 延迟加载方式 调用async时不创建，调用 future的get或者wait时才创建线程
* f:线程函数
* args:线程函数的参数
*/
void asyncTest() {
    //1
    std::future<int> f1 = std::async(std::launch::async, []() {
        return 8;
    });
    cout << f1.get() << endl; //output: 8

    //2
    std::future<void> f2 = std::async(std::launch::async, []() {
        cout << 8 << endl;
    });
    f2.wait(); //output: 8

    //3
    std::future<int> future = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return 8;
    });

    std::cout << "waiting...\n";
    std::future_status status;
    do {
        status = future.wait_for(std::chrono::seconds(1));
        if (status == std::future_status::deferred) {
            std::cout << "deferred\n";
        }
        else if (status == std::future_status::timeout) {
            std::cout << "timeout\n";
        }
        else if (status == std::future_status::ready) {
            std::cout << "ready!\n";
        }
    } while (status != std::future_status::ready);

    std::cout << "result is " << future.get() << '\n';
}

//比较thread和 async的性能
typedef std::chrono::steady_clock                STEADY_CLOCK;
typedef std::chrono::steady_clock::time_point    TIME_POINT;

void print_diff_time(const TIME_POINT& t1, const TIME_POINT& t2)
{
    std::chrono::duration<double, std::milli> dTimeSpan = std::chrono::duration<double, std::milli>(t2 - t1);

    std::cout << "time span : " << dTimeSpan.count() << "ms\n" << endl << endl;
}

//-------------------------传统方式 没有多线程------------------------------
//计算A函数
int calculateA()
{
    //延迟1ms
    this_thread::sleep_for(std::chrono::milliseconds(1));
    return 1;
}

//计算B函数
int calculateB()
{
    //延迟2ms
    this_thread::sleep_for(std::chrono::milliseconds(2));
    return 2;
}

//计算C函数
int calculateC()
{
    //延迟3ms
    this_thread::sleep_for(std::chrono::milliseconds(3));
    return 3;
}

//统计三者的和 传统方式 没有多线程
void test_common_time()
{
    TIME_POINT t1 = STEADY_CLOCK::now();

    int c = calculateA() + (calculateB() + calculateC());

    TIME_POINT t2 = STEADY_CLOCK::now();

    cout << "test_common_time result is :" << c << "  ";
    print_diff_time(t1, t2);
}

//-------------------------多线程方式------------------------------
//防止数据竞争
std::mutex g_mtu;

void calA(int& sum)
{
    this_thread::sleep_for(std::chrono::milliseconds(1));
    //多线程加锁保护
    lock_guard<mutex> lk(g_mtu);
    sum += 1;
}

void calB(int& sum)
{
    this_thread::sleep_for(std::chrono::milliseconds(2));
    lock_guard<mutex> lk(g_mtu);
    sum += 2;
}

void calC(int& sum)
{
    this_thread::sleep_for(std::chrono::milliseconds(3));
    lock_guard<mutex> lk(g_mtu);
    sum += 3;
}

//case2
void test_threads_time()
{
    int sum = 0;

    TIME_POINT t11 = STEADY_CLOCK::now();
    //启动三个线程，进行运算
    thread t1(calA, ref(sum));
    thread t2(calB, ref(sum));
    thread t3(calC, ref(sum));
    //等待计算完成
    t1.join();
    t2.join();
    t3.join();
    //计算完成
    TIME_POINT t22 = STEADY_CLOCK::now();
    cout << "test_threads_time result is :" << sum << "  ";
    print_diff_time(t11, t22);

}




//-------------------------async方式------------------------------

//case3
void test_feture_time()
{
    int sum = 0;

    TIME_POINT t11 = STEADY_CLOCK::now();

    //异步调用
    future<int> f1 = std::async(calculateA);
    future<int> f2 = std::async(calculateB);
    future<int> f3 = std::async(calculateC);
    //get()函数会阻塞，直到结果返回
    sum = f1.get() + f2.get() + f3.get();

    TIME_POINT t22 = STEADY_CLOCK::now();

    cout << "test_feture_time result is :" << sum << "  ";

    print_diff_time(t11, t22);
}


void asyncCompare() {
    cout << "不使用多线程：顺序执行，效率不高" << endl;
    test_common_time();

    cout << "使用多线程：效率高，引入锁机制，实现相对复杂" << endl;
    test_threads_time();

    cout << "使用async：效率高，实现简洁（async封装了异步实现，没有显示引入线程，锁，缓冲区等概念）" << endl;
    test_feture_time();

    cout << "总结：async异步调用函数，通过启动一个新线程或者复用一个它认为合适的已有线程实现，async指代语义上的并发，使用者并不需要关心具体的调用策略。" << endl;
    
}


int fun(int x) {
    x++;
    x *= 10;
    std::cout << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return x;
}


//int main()
//{
//    std::packaged_task<int(int)> pt(fun);         // 将函数打包起来
//    std::future<int> fu = pt.get_future();        // 并将结果返回给future
//    std::thread t(std::ref(pt), 1);
//    std::cout << fu.get() << std::endl;
//    std::cout << std::this_thread::get_id() << std::endl;
//    t.join();
//    return 0;
//}




//int main()
//{
//    std::cout << "Hello World!\n";
//
//    asyncCompare();
//}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
