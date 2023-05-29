// testApp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <string>
#include <random>

using namespace std;

std::string getRandomStr(int length);
std::string getRandomStr2(int length);
wstring getComputerName();

class A {
public:
	int a = 1;
	virtual void testa() = 0;
};

class B : public A {
public:
	int b = 1;
};

class C {
public:
	void test(A* a) {
		cout << "ccc" << endl;
	}
};

void testClass() {
	A* a{ nullptr };
	B* b{ nullptr };
	C c;

	A* ab = b;

	c.test(ab);
}

#include <shlobj.h>
#pragma comment(lib, "shell32.lib")


#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")

//初始化网络库
bool wsaInit()
{
	bool bRet = true;
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	auto err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		cout << "WSA startup failed!" << endl;
		bRet = false;
	}
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		bRet = false;
	}
	return bRet;
}

//反初始化网络库
void wsaUninit() 
{
	WSACleanup();
}

//获取第一个可用的端口号
int getAvailablePort()
{
	int iRetPort = -1;
	bool result = true;

	BOOL			bReuseAddr = TRUE;
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bReuseAddr, sizeof(bReuseAddr));
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
	int ret = ::bind(sock, (SOCKADDR*)&addr, sizeof addr);
	if (0 != ret) {
		result = false;
	}
	struct sockaddr_in connAddr;
	int len = sizeof connAddr;
	ret = getsockname(sock, (SOCKADDR*)&connAddr, &len);
	if (0 != ret) {
		result = false;
	}
	// 获取端口号
	iRetPort = ntohs(connAddr.sin_port);
	closesocket(sock);
	sock = INVALID_SOCKET;

	return iRetPort;
}

int getAvailableUDPPort()
{
	int iRetPort = -1;
	bool result = true;

	BOOL			bReuseAddr = TRUE;
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bReuseAddr, sizeof(bReuseAddr));
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = 0;
	int ret = ::bind(sock, (SOCKADDR*)&addr, sizeof addr);
	if (0 != ret) {
		result = false;
	}
	struct sockaddr_in connAddr;
	int len = sizeof connAddr;
	ret = getsockname(sock, (SOCKADDR*)&connAddr, &len);
	if (0 != ret) {
		result = false;
	}
	// 获取端口号
	iRetPort = ntohs(connAddr.sin_port);
	closesocket(sock);
	sock = INVALID_SOCKET;
	return iRetPort;
}

bool initUDPMessageListen(_In_ int iLogPort)
{
	//udp s_tunnel上报消息接收
	SOCKADDR_IN RecvAddr;
	//创建接收数据报的socket
	SOCKET m_sServer = socket(AF_INET, SOCK_DGRAM, 0);
	RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(iLogPort);

	if (::bind(m_sServer, (SOCKADDR*)&RecvAddr, sizeof(SOCKADDR)) != 0) {
		auto iErrorCode = WSAGetLastError();
		cout << "bind error！errorcode:" << endl;
		cout << iErrorCode << endl;
		return false;
	}

	cout << "bind OK" << endl;

	//释放端口
	closesocket(m_sServer);
	m_sServer = INVALID_SOCKET;

	return true;
}

class aaa {
public:
	int name;
	void setName(int i) {
		name = i;
	}

	void getName() const {
		
	}
};

std::unique_ptr<int>& fun1() {
	std::unique_ptr<int> up1(nullptr);
	return up1;
}

class CTestA {
public:
	int iNum{ 90 };
private:
	string strAA{ "Hello 123." };
public:
	inline void sayHello() { cout << strAA << endl; }
};


#include <stdio.h>
#include <io.h>
void getFileSize()
{
	string filename = "C:\\Program Files (x86)\\ITtools\\res\\html_test\\tp.html";
	FILE* file = nullptr;
	fopen_s(&file, filename.c_str(), "rb");
	if (file)
	{
		int size = _filelength(_fileno(file));
		cout << size << endl;
		fclose(file);
		file = nullptr;
	}
}

long long GetDirectorySize(string strSrcPath);

void getLang() {
	LANGID lang_id = GetUserDefaultUILanguage();//"win10>设置>语言>Windows显示语言"
	cout << "lang_id:" << lang_id << endl;
}

int main123()
{
	getLang();
	system("pause");
	return 0;
	//-------------
	string updateDir = "C:\\Users\\guohangyu\\AppData\\Local\\Netease\\XiaoYiHelper\\XiaoYiHelper\\Update";
	auto ret = GetDirectorySize(updateDir);
	cout << "Dir size:" << ret << endl;
	return 0;

	getFileSize();

	return 1;


	shared_ptr<CTestA> p2 = make_shared<CTestA>();

	p2->sayHello();

	//fun1();
	return 0;
	//-----------
	aaa aa;
	const aaa* ptraa = &aa;
	//ptraa->setName(5);

	ptraa->getName();

	aaa* ptrbb = &aa;
	ptrbb->setName(5);


	//-----------
	aaa a;
	auto ptra = shared_ptr<aaa>(&a);
	cout << ptra.use_count() << endl;
	//auto ptrb = shared_ptr<aaa>(&a);
	//cout << ptra.use_count() << endl;
	auto ptrc = ptra;
	cout<< ptra.use_count() << endl;
	cout << ptrc.use_count() << endl;
	system("pause");

	return 0;
	//-----------------
	wsaInit();
	int iPort;
	string aaa;
	/*while (true)*/ {
		iPort = getAvailableUDPPort();
		cout << "getAvailableUDPPort:" << iPort << endl;

		initUDPMessageListen(iPort);

		/*cout << "输入任意值测试下一个端口号" << endl;
		cin >> aaa;*/
	}
	
	wsaUninit();
	system("pause");
	return 0;
	//-----------------

	CHAR my_documents[MAX_PATH], public_documents[MAX_PATH];
	HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
	result = SHGetFolderPathA(NULL, CSIDL_COMMON_DOCUMENTS, NULL, SHGFP_TYPE_CURRENT, public_documents);
	

	if (result != S_OK)
		std::cout << "Error: " << result << "\n";
	else {
		std::cout << "my_documents Path: " << my_documents << "\n";
		std::cout << "public_documents Path: " << public_documents << "\n";
	}
		
	return 0;

	//-----------------
	testClass();

	system("pause");

	//-----------------
    std::cout << "Hello World!\n";
	for (int i = 0; i < 10; i++) {
		string a = getRandomStr2(10);
		Sleep(1000);
		cout << a << endl;
	}
	

	system("pause");
}

wstring getComputerName()
{
	TCHAR buf[MAX_COMPUTERNAME_LENGTH + 2];
	DWORD buf_size;
	buf_size = sizeof buf - 1;
	GetComputerName(buf, &buf_size);
	wstring a = buf;
	return a;
}

std::string getRandomStr(int length)
{
	string ans;

	default_random_engine e;
	e.seed((unsigned)time(NULL));
	uniform_int_distribution<int> u(0, 100);

	for (int i = 0; i < length; i++)
	{
		if (u(e) % 2 == 0)
		{
			ans += 'a' + u(e) % 26;
		} else
		{
			ans += 'A' + u(e) % 26;
		}
	}

	return ans;
}

std::string getRandomStr2(int length)
{
	string ans;

	srand((unsigned)time(NULL));
	for (int i = 0; i < length; i++)
	{
		if (rand() % 2 == 0)
		{
			ans += 'a' + rand() % 26;
		} else
		{
			ans += 'A' + rand() % 26;
		}
	}

	return ans;
}


long long GetDirectorySize(string strSrcPath)
{
	WIN32_FIND_DATAA data;
	string strPath;
	long long size = 0;
	if (strSrcPath.empty())
	{
		return 0;
	}
	strPath = strSrcPath;
	strPath += "\\*";
	HANDLE hFile = FindFirstFileA(strPath.c_str(), &data);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		while (FindNextFileA(hFile, &data))
		{
			if (FILE_ATTRIBUTE_DIRECTORY & data.dwFileAttributes)
			{
				if (L'.' != data.cFileName[0])
				{
					strPath = strSrcPath;
					strPath += "\\";
					strPath += data.cFileName;
					size += GetDirectorySize(strPath);
				}
			}
			else
			{
				LARGE_INTEGER li = { data.nFileSizeLow, (LONG)data.nFileSizeHigh };
				size += li.QuadPart;
			}
		}
		FindClose(hFile);
	}
	return size;
}



#include <stdlib.h>
#include <string>
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
//----------------------------------------------------------
std::mutex g_mutexWait;                 // 互斥锁
std::condition_variable g_condWait;     // 条件变量
//----------------------------------------------------------
///< 输入数据线程函数
void InputThread()
{
	std::cout << "请在10秒内输入任意字符：" << std::endl;
	// 等待手工输入
	std::string strInputData = "";
	std::cin >> strInputData;
	// 输入了字符，则发出通知
	if (strInputData != "")
	{
		g_condWait.notify_one();
	}
}
//----------------------------------------------------------
///< 主函数
int main333(int argc, char* argv[])
{
	try
	{
		// 启动线程输入数据
		std::thread threadInput(InputThread);
		// 取得当前时间
		time_t tmInputStart = time(NULL);

		// 使用条件变量，等待输入数据
		std::unique_lock<std::mutex> lockWait(g_mutexWait);
		std::cv_status cvsts = g_condWait.wait_for(lockWait, std::chrono::seconds(10));
		// 消息接收超时
		if (cvsts == std::cv_status::timeout)
		{
			std::cout << "您输入的太慢了！请输入任意字符退出程序！" << std::endl;
		}
		else // 接收到条件变量信号，未超时
		{
			time_t tmInputEnd = time(NULL);
			std::cout << "您输入的太快了！只用了" << (tmInputEnd - tmInputStart) << "秒！" << std::endl;
		}
		// 等待线程退出
		threadInput.join();
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	system("PAUSE");
	return 0;
}

//--------------------定时回调------------------------------------
void CALLBACK f(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime)
{
	printf("Hello");
}

int main455()
{
	MSG msg;

	SetTimer(NULL, 0, 1000, (TIMERPROC)&f);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

//-------------------------------------

class printTest
{
public:
	printTest(string str) {
		thread th([=]() {
			while (1) {
				printf("Hello: %s\n", str.c_str());
				Sleep(1000);
			}
		});
		th.detach();
	}
	
};

void main55() {
	auto p = new printTest("hahaha");
	Sleep(5000);
	delete p;
	p = new printTest("bbbbbbbbbbb");

	Sleep(10000);
}