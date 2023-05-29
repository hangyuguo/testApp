#pragma once


//函数指针定义
typedef int (*Calc)(int a, int b);

int CalcValue(int a, int b, const Calc& func) {
	return func(a, b);
}

int Add(int a, int b) {
	return(a + b);
}

int Minus(int a, int b) {
	return(a - b);
}

void calcTest1() {
	int a = 5, b = 3;
	cout << "Add:" << CalcValue(a, b, Add) << endl;
	cout << "Minus:" << CalcValue(a, b, Minus) << endl;
}

//-----------sort
#include <vector>
#include <algorithm>

struct DataPool
{
	int value = 0;
	int date = 0;
	struct DataPool(int v, int d) : value(v), date(d) {};
};

bool SortCallBack(const DataPool& a, const DataPool& b) {
	return a.value < b.value;
};

void sortTest() {
	vector<DataPool> vec;
	vec.push_back(DataPool(2, 1));
	vec.push_back(DataPool(5, 2));
	vec.push_back(DataPool(1, 3));

	std::sort(vec.begin(), vec.end(), SortCallBack);

	for (auto i : vec) {
		cout << i.value << ":" << i.date << endl;
	}
}

//-----------bind function
//用于解决 回调类成员函数时，函数指针无法指向类成员函数的问题
#include <functional>

bool CompareInt(int a, int b) {
	return a > b;
}

std::function<bool(int, int)> compareFunc = CompareInt;

//使用function调用成员函数:bind
//1 绑定普通函数时，第一个参数是可调用的函数的引用，后续参数是参数 
//std::bind(&funcName, std::placeholders::_1, std::placeholders::_2);

//2 绑定类的成员函数时 第一个参数是可调用的类对象的成员函数的引用，第二个参数为类对象的指针，后续为参数
//std::bind(&className:funcName, classPtr, std::placeholders::_1, std::placeholders::_2)

class mathTest
{
public:
	int mathAdd(int a, int b) { return a + b; };
};

void bindTest() {
	int a = 4;
	int b = 6;
	std::shared_ptr<mathTest> math1 = std::make_shared<mathTest>();
	std::function<int(int, int)> addFunc = std::bind(&mathTest::mathAdd, math1, std::placeholders::_1, std::placeholders::_2);

	int c = addFunc(a, b);
	std::cout << "c Value: " << c << std::endl;
}


//----------------回调类-------------------
#include <Windows.h>

//1. 定义回调函数类型
typedef std::function<void(const char*, int, double)> call_back_fun_t;

//2. c++类成员变量回调函数
class cpp_callback_t
{
public:
	void cpp_callback_func(const char* s_arg, int i_arg, double d_arg) {
		printf("print from cpp_callback_func, s_arg = %s, i_arg = %d, d_arg = %lf\n",
			s_arg, i_arg, d_arg);
	}
};

//
int eventloop(call_back_fun_t cb_func) {
	int loop_times = 5;
	while (loop_times--)
	{
		if (cb_func)
			cb_func("STRING", 1, 0.1);
		Sleep(1000);
	}
	return 0;
}

void callbackTest() {
	//std::fcuntion类型的变量
	call_back_fun_t cb_func;

	cpp_callback_t cpp_cb;
	cb_func = std::bind(&cpp_callback_t::cpp_callback_func, cpp_cb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

}


//-----------------------------------

//回调参数
struct CallbackParam {
	string tt;
};

//抽象类 提供回调接口
class ICallback {
public:
	virtual void notifyCallback(CallbackParam pa) = 0;
};

class A {
public:
	void setCallback(ICallback* p) {
		p_cb = p;
		CallbackParam temp;
		temp.tt = "11111";
		p_cb->notifyCallback(temp);
	}
private:
	ICallback* p_cb;
};

class B :public ICallback {
public:
	void setTest() {
		a.setCallback(this);
	}
private:
	virtual void notifyCallback(CallbackParam pa) {
		cout << "notifyCallback:" << pa.tt << endl;
	}
	A a;
};

void classCallbackTest() {
	B b;
	b.setTest();
}