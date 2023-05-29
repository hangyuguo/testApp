#include "ThreadTaskManager.h"

std::shared_ptr<ThreadPool> ThreadTaskManager::pThreadPool_;

void ThreadTaskManager::PostTask(std::shared_ptr<IThreadTask> pTask)
{
	pThreadPool_->enqueue([pTask]() {
		pTask->run();
	});
}

void ThreadTaskManager::PostDelayedTask(const StdClosure& task, std::chrono::milliseconds delay)
{
	std::thread th([=]() {
		std::this_thread::sleep_for(delay);
		pThreadPool_->enqueue(task);
	});
	th.detach();
}

void ThreadTaskManager::PostDelayedTask(std::shared_ptr<IThreadTask> pTask, std::chrono::milliseconds delay)
{
	std::thread th([pTask, delay]() {
		std::this_thread::sleep_for(delay);
		pThreadPool_->enqueue([pTask]() {
			pTask->run();
		});
	});
	th.detach();
}

void ThreadTaskManager::PostRepeatedTask(const WeakCallback<StdClosure>& task, const std::chrono::milliseconds& delay, int times)
{
	StdClosure closure = Bind(&ThreadTaskManager::runRepeatedly, task, delay, times);
	ThreadTaskManager::PostDelayedTask(closure, delay);
}

ThreadTaskManager::ThreadTaskManager()
{
	ThreadTaskManager::pThreadPool_ = std::make_shared<ThreadPool>(5);
}

void ThreadTaskManager::runRepeatedly(const WeakCallback<StdClosure>& task, const std::chrono::milliseconds& delay, int times)
{
	if (task.Expired()) {
		return;
	}
	task();
	if (task.Expired()) {
		return;
	}
	if (times != TIMES_FOREVER) {
		times--;
	}
	if (times != 0) {
		PostRepeatedTask(task, delay, times);
	}
}
