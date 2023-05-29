#include "ThreadPool.h"
#include "ThreadTaskManager.h"
#include <iostream>
using namespace std;

class test1:public virtual SupportWeakCallback
{
public:
	test1() = default;
	~test1() = default;

	void test() {
		ThreadTaskManager::GetInstance()->PostDelayedTask(ToWeakCallback([]() {
			cout << "postDelayedTask 1000" << endl;
		}), chrono::seconds(1));

		ThreadTaskManager::GetInstance()->PostRepeatedTask(ToWeakCallback([]() {
			cout << "postRepeatTask 3000" << endl;
		}), chrono::seconds(3), 3);

		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
};


//bool VersionCheckTask::CheckAppVersion(UpdateData& updateData, int nTimeout /*= 3*/)
//{
//	bool bRet = false;
//	//调用后端接口实现 超时3s
//	ThreadTaskManager::GetInstance()->PostTask([&]() {
//		CheckUpdateResponse response;
//		getUpdateInfoFromServer(response);
//
//		std::unique_lock<std::mutex> lockWait(g_mutexWait);
//		g_condWait.notify_one();
//		if (response.code == 200) {
//			updateData = response.data;
//			bRet = updateData.isValid();
//		}
//	});
//
//	std::unique_lock<std::mutex> lockWait(g_mutexWait);
//	std::cv_status cvsts = g_condWait.wait_for(lockWait, std::chrono::seconds(nTimeout));
//	// 消息接收超时
//	if (cvsts == std::cv_status::timeout) {
//		//超时
//		return false;
//	}
//
//	return bRet;
//}


//class TestDelete:public  virtual SupportWeakCallback
//{
//public:
//	TestDelete() = default;
//
//	void Test2() {
//		std::cout << this->name_;
//
//		Test();
//
//	}
//
//	void Test() {
//		
//		//ThreadTaskManager::GetInstance()->postRepeatedTask(ToWeakCallback(std::bind(&TestDelete::Test2,this)), std::chrono::milliseconds(3000));
//		ThreadTaskManager::GetInstance()->postTask(std::bind(&TestDelete::Test2, this));
//		std::cout << "";
//	}
//
//	std::string name_ = "haha";
//}; 


class CheckTask :public IThreadTask
{
public:
	CheckTask() = default;
	~CheckTask() = default;

	virtual void run() {
		cout << "CheckTask running" << endl;
	}
};

void main() {
	
	/*ThreadTaskManager::GetInstance()->postTask([]() {
		for (int i = 0; i < 10; i++) {
			cout << i << endl;
		}
	});*/

	/*ThreadTaskManager::GetInstance()->postDelayedTask([]() {
		cout << "postDelayedTask 1000" << endl;
	},chrono::milliseconds(1000));

	ThreadTaskManager::GetInstance()->postRepeatedTask([]() {
		cout << "postRepeatTask 3000" << endl;
	}, chrono::milliseconds(3000));
	
	std::this_thread::sleep_for(std::chrono::milliseconds(100000));*/

	/*test1 test;
	test.test();*/

	auto pCheckTask = make_shared<CheckTask>();
	ThreadTaskManager::GetInstance()->PostTask(pCheckTask);

	

	system("pause");
}