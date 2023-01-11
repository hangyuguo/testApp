// BlockQueue.h
#pragma  once
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

using namespace std;

#define TASK_NUM 8

class CBlockQueue
{
	private:
    mutex _mt;
    condition_variable _cv_con;
    condition_variable _cv_prod;
    queue<int>   _tasks;
    atomic<bool> _stopped;

    const int _capacity;

    bool stopped()
    {
        return _stopped.load();
    }

    bool empty()
    {
        return _tasks.size() == 0 ? true : false;
    }

    bool full()
    {
        return _tasks.size() == _capacity ? true : false;
    }

public:
    CBlockQueue();
    ~CBlockQueue();
    void stop()
    {
        _stopped.store(true);
        _cv_con.notify_all();
    }
    
    bool available()
    {
        return !stopped() || !empty();
    }

    void push(const int& data);
    void pop(int& data);
};