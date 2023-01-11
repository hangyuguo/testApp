#pragma once
#include <coroutine>
using namespace std;

/*
* coro_ret �Ǹ��Զ���Ľṹ��Ϊ������ΪЭ�̵ķ���ֵ����Ҫ����һ��promise_type�����������Ҫʵ�����½ӿڣ�
* coro_ret get_return_object() ����ӿ�Ҫ����promise�Լ���ʵ�������һ��Э�̵ķ���ֵ������Э����������ǰ���е��ã�����ӿڵķ���ֵ����ΪЭ�̵ķ���ֵ��
* awaiter initial_suspend() ����ӿڻ���Э�̱�������Ҳ���ǵ�һ�ε��ã�
* awaiter yield_value(T v) ����ӿڻ��� co_yield v ʱ������
* void return_value(T v) ����ӿڻ��� co_return v ʱ������
* void return_void() ��� co_return ����û�н��κ�ֵ����ô�ͻ��������ӿ� return_void��return_valueֻ��ѡ��һ��ʵ�֣�����ᱨ�������
* awaiter final_suspend() ��Э������˳�����õĽӿ�
* void unhandled_exception()���Э���ڵĴ����׳����쳣����ô����ӿڻᱻ���á�
*/

//!coro_ret Э�̺����ķ���ֵ���ڲ�����promise_type����ŵ����
template <typename T>
struct coro_ret
{
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    //! Э�̾��
    handle_type coro_handle_;

    //!promise_type���ǳ�ŵ���󣬳�ŵ��������Э�����⽻��
    struct promise_type
    {
        promise_type() {
            std::cout << "promise constructor invoded." << std::endl;
        }
        ~promise_type() = default;

        //!����Э�̷���ֵ
        auto get_return_object()
        {
            std::cout << "get_return_object invoked." << std::endl;
            return coro_ret<T>{handle_type::from_promise(*this)};
        }

        //! ע���������,���صľ���awaiter
        //! �������std::suspend_never{}���Ͳ�����
        //! ����std::suspend_always{} ����
        //! ��Ȼ��Ҳ���Է�������awaiter
        auto initial_suspend()
        {
            //return std::suspend_never{};
            std::cout << "initial_suspend invoked." << std::endl;
            return std::suspend_always{};
        }
        //!co_return ����������ᱻ����
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

        //! ��Э������˳�����õĽӿڡ�
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
        //����ֵ
        T return_data_;
    };

    coro_ret(handle_type h)
        : coro_handle_(h)
    {
    }
    ~coro_ret()
    {
        //!��������
        if (coro_handle_)
        {
            coro_handle_.destroy();
        }
    }

    //!�ָ�Э�̣������Ƿ����
    bool resume()
    {
        if (!coro_handle_.done()) {  //! ����Ѿ�done�ˣ��ٵ���resume���ᵼ��coredump
            coro_handle_.resume();
        }
        return coro_handle_.done();
    }

    bool done() const
    {
        return coro_handle_.done();
    }

    //!ͨ��promise��ȡ���ݣ�����ֵ
    T get()
    {
        return coro_handle_.promise().return_data_;
    }

};