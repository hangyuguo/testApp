/**
* 阻塞队列的一个实现
*/

#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <assert.h>

template <typename T>
class CBlockQueue
{
public:
    CBlockQueue();
    ~CBlockQueue() = default;
    // 禁止拷贝构造
    CBlockQueue(CBlockQueue&) = delete;

    void push(T&& value);
    void push(const T& value);
    T take();
    size_t size() const;

private:
    // 实际使用的数据结构队列
    std::queue<T> m_data;

    // 条件变量
    std::mutex m_mutex;
    std::condition_variable m_condition;
};


template<typename T>
CBlockQueue<T>::CBlockQueue()
{
}

template<typename T>
void CBlockQueue<T>::push(T&& value)
{
    // 往队列中塞数据前要先加锁
    std::unique_lock<std::mutex> lock(m_mutex);
    m_data.push(value);
    // 唤醒正在阻塞的条件变量
    m_condition.notify_all();
}

template<typename T>
void CBlockQueue<T>::push(const T& value)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_data.push(value);
    m_condition.notify_all();
}

template<typename T>
T CBlockQueue<T>::take()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_data.empty())
    {
        // 等待
        m_condition.wait(lock);
    }
    assert(!m_data.empty());
    T value(std::move(m_data.front()));
    m_data.pop();

    return value;
}

template<typename T>
size_t CBlockQueue<T>::size() const
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_data.size();
}

