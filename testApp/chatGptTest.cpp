#include <windows.h>
#include <wininet.h>
#include <iostream>

#include "gpt.h"
//
//#pragma comment(lib,"Wininet.lib")

using std::wstring;
using std::string;
//
//using std::cout; 
//using std::endl;
//void WINAPI OnStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
//{
//	switch (dwInternetStatus)
//	{
//	case INTERNET_STATUS_CONNECTING_TO_SERVER:
//		cout << "Connecting to server\n" << endl;
//		break;
//	case INTERNET_STATUS_CONNECTED_TO_SERVER:
//		cout << "Connected to server\n" << endl;
//		break;   
//	case INTERNET_STATUS_CONNECTION_CLOSED:
//		cout << "Disconnected from server\n" << endl;
//		break;
//	}
//}

wstring StringToWString(const string& str)
{
	wstring wstr;
	size_t size;
	wstr.resize(str.length());
	mbstowcs_s(&size, &wstr[0], wstr.size() + 1, str.c_str(), str.size());
	return wstr;
}

string WStringToString(const wstring& wstr)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char* buffer = new char[size];
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer, size, NULL, NULL);
	std::string str(buffer);
	delete[] buffer;
	return str;
}

int main2222()
{
	//gptTest::deleteOldVersions("1.6.5");
	//string str = "hahah 込込込";
	//wstring wstr;
	//wstr = StringToWString(str);
	//std::wcout <<"wstr"<< wstr << std::endl;
	////test
	//std::cout << "wstring::::" << std::endl;
	////
	//wstr = L"hahah込込3232";
	//str = WStringToString(wstr);
	//std::cout << "str:"<<str << std::endl;
	//std::cout << std::endl;


	//-----------------------------------
	/*int i = 0;
	while (i < 100) {
		i++;
		gptTest::getActiveUserCount();
		gptTest::getMemoryUsage();
		gptTest::getCPUUsage();
		std::cout << "------------------" << i << "-----------------" << endl;
		std::cout << "------------------" << i << "-----------------" << endl;
		Sleep(1000);
	}*/
	//-----------------------------------


	system("pause");
	return 0;
}



#include <Windows.h>
#include <WtsApi32.h>

#pragma comment(lib, "Wtsapi32.lib")

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) {
	switch (dwCtrlType) {
	case CTRL_LOGOFF_EVENT:
		// user is logging off
		std::cout << "CTRL_LOGOFF_EVENT" << endl;
		break;
	case CTRL_SHUTDOWN_EVENT:
		// system is shutting down
		std::cout << "CTRL_SHUTDOWN_EVENT" << endl;
		break;
	}
	return TRUE;
}

int main() {
	//gptTest::setWindowTop(L"Bulk Crap Uninstaller v5.5 宴亊 x64");.
	//gptTest::reboot();
	//gptTest::base64Test();
	gptTest::getDeviceName();


	system("pause");
	std::cout << "return" << endl;
	return 0;
}