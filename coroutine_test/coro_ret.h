#pragma once
#include <coroutine>
using namespace std;

/*
* coro_ret 是个自定义的结构，为了能作为协程的返回值，需要定义一个promise_type，这个类型需要实现如下接口：
* coro_ret get_return_object() 这个接口要能用promise自己的实例构造出一个协程的返回值，会在协程正在运行前进行调用，这个接口的返回值会作为协程的返回值。
* awaiter initial_suspend() 这个接口会在协程被创建（也就是第一次调用）
* awaiter yield_value(T v) 这个接口会在 co_yield v 时被调用
* void return_value(T v) 这个接口会在 co_return v 时被调用
* void return_void() 如果 co_return 后面没有接任何值，那么就会调用这个接口 return_void和return_value只能选择一个实现，否则会报编译错误。
* awaiter final_suspend() 在协程最后退出后调用的接口
* void unhandled_exception()如果协程内的代码抛出了异常，那么这个接口会被调用。
*/

//!coro_ret 协程函数的返回值，内部定义promise_type，承诺对象
template <typename T>
struct coro_ret
{
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    //! 协程句柄
    handle_type coro_handle_;

    //!promise_type就是承诺对象，承诺对象用于协程内外交流
    struct promise_type
    {
        promise_type() {
            std::cout << "promise constructor invoded." << std::endl;
        }
        ~promise_type() = default;

        //!生成协程返回值
        auto get_return_object()
        {
            std::cout << "get_return_object invoked." << std::endl;
            return coro_ret<T>{handle_type::from_promise(*this)};
        }

        //! 注意这个函数,返回的就是awaiter
        //! 如果返回std::suspend_never{}，就不挂起，
        //! 返回std::suspend_always{} 挂起
        //! 当然你也可以返回其他awaiter
        auto initial_suspend()
        {
            //return std::suspend_never{};
            std::cout << "initial_suspend invoked." << std::endl;
            return std::suspend_always{};
        }
        //!co_return 后这个函数会被调用
        /*
        void return_value(const T&amp; v)
        {
            return_data_ = v;
            return;
        }
        */

        void return_void()
        {
            std::cout << "return void invoked." << std::endl;
        }

        //!
        auto yield_value(const T& amp; v)
        {
            std::cout << "yield_value invoked." << std::endl;
            return_data_ = v;
            return std::suspend_always{};
            //return std::suspend_never{};
        }

        //! 在协程最后退出后调用的接口。
        bool final_suspend() noexcept
        {
            std::cout << "final_suspend invoked." << std::endl;
            return std::suspend_always{};
        }

        //
        void unhandled_exception()
        {
            std::cout << "unhandled_exception invoked." << std::endl;
            std::exit(1);
        }
        //返回值
        T return_data_;
    };

    coro_ret(handle_type h)
        : coro_handle_(h)
    {
    }
    ~coro_ret()
    {
        //!自行销毁
        if (coro_handle_)
        {
            coro_handle_.destroy();
        }
    }

    //!恢复协程，返回是否结束
    bool resume()
    {
        if (!coro_handle_.done()) {  //! 如果已经done了，再调用resume，会导致coredump
            coro_handle_.resume();
        }
        return coro_handle_.done();
    }

    bool done() const
    {
        return coro_handle_.done();
    }

    //!通过promise获取数据，返回值
    T get()
    {
        return coro_handle_.promise().return_data_;
    }

};