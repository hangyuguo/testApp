#pragma once
#include "ThreadPool.h"
#include "callback.h"
#include <thread>
#include <memory>
#include <chrono>

//线程任务基类
class IThreadTask
{
public:
	IThreadTask() = default;
	~IThreadTask() = default;

	//执行任务
	virtual void run() = 0;
};

typedef std::function<void(void)> StdClosure;

//饿汉模式
#define SINGLETON_DEFINE_TH(TypeName)				\
static TypeName* GetInstance()					\
{												\
	static TypeName type_instance;				\
	return &type_instance;						\
}												\
												\
TypeName(const TypeName&) = delete;				\
TypeName& operator=(const TypeName&) = delete

class ThreadTaskManager
{
public:
	SINGLETON_DEFINE_TH(ThreadTaskManager);

	template<class F, class... Args>
	static auto PostTask(F&& f, Args&&... args)
		-> std::future<typename std::result_of<F(Args...)>::type>
	{
		return pThreadPool_->enqueue(std::forward<F>(f), std::forward<Args>(args)...);
	}

	static void PostTask(std::shared_ptr<IThreadTask> pTask);

	//向线程池增加一个延时执行的任务
	//第二个参数可以使用任何单位的 chrono::duration，会自动转换为 chrono::milliseconds。但是最高精度为 milliseconds
	static void PostDelayedTask(const StdClosure& task, std::chrono::milliseconds delay);
	static void PostDelayedTask(std::shared_ptr<IThreadTask> pTask, std::chrono::milliseconds delay);

	//向线程池增加一个循环执行的任务
	static void PostRepeatedTask(const WeakCallback<StdClosure>& task, const std::chrono::milliseconds& delay, int times = TIMES_FOREVER);

private:
	ThreadTaskManager();
	~ThreadTaskManager() = default;

	static const int TIMES_FOREVER = -1;
	static void runRepeatedly(const WeakCallback<StdClosure>& task, const std::chrono::milliseconds& delay, int times = TIMES_FOREVER);

private:
	static std::shared_ptr<ThreadPool> pThreadPool_;
};