// coroutine_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 参考 https://juejin.cn/post/7152038652682436645
//

#include <iostream>
#include <coroutine>

#include "coro_ret.h"

/*
* 返回值类型 coro_ret 
* 协程本身的代码中并没有通过return返回这个类型的数据，这就是C++20里实现协程的一个关键点:
* 协程的返回类型T中，必须有T::promise_type这个类型定义，这个类型要实现几个接口
*/
coro_ret<int> number_generator(int begin, int count) {
    std::cout << "number_generator invoked." << std::endl;
    for (int i = begin; i < count; ++i) {
        co_yield i;//执行到这里就会挂起，控制权回到调用者，直到调用者调用resume才继续从这里执行
    }
    co_return;
}

int main()
{
    auto g = number_generator(1, 10);
    std::cout << "begin to run!" << std::endl;
    while (!g.resume()) {
        std::cout << "got number:" << g.get() << std::endl;
    }
    std::cout << "coroutine done, return value:" << g.get() << std::endl;
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
