/**
* �������е�һ��ʵ��
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
    // ��ֹ��������
    CBlockQueue(CBlockQueue&) = delete;

    void push(T&& value);
    void push(const T& value);
    T take();
    size_t size() const;

private:
    // ʵ��ʹ�õ����ݽṹ����
    std::queue<T> m_data;

    // ��������
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
    // ��������������ǰҪ�ȼ���
    std::unique_lock<std::mutex> lock(m_mutex);
    m_data.push(value);
    // ����������������������
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
        // �ȴ�
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

