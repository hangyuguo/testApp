// InterfaceInfoTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <winsock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <string>

#pragma comment(lib, "iphlpapi.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x)) 
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

//1.遍历注册表 寻找与metric相同的网卡适配器，取得注册表节点名称

//2.通过GetInterfaceInfo接口查找与第1步中获取名称相等的网卡适配器Index

//3.通过powershell 命令Set-NetIPInterface 设置跃点数

#define SAFE_CLOSE_HANDLE(p) { if(p) { CloseHandle(p); (p) = nullptr; } }

void callPwShell(int nIndex)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	//%SYSTEMROOT%\System32\WindowsPowerShell\v1.0

	wstring strCmd = L"powershell -command \"Set-NetIPInterface -InterfaceIndex " + to_wstring(nIndex) + L" -InterfaceMetric 1\"";
	TCHAR cmd[1024] = { 0 };
	lstrcpy(cmd, strCmd.c_str());
	auto bSuccess = CreateProcess(NULL, // No module name (use command line).
		cmd, // Command line.
		NULL, // Process handle not inheritable.
		NULL, // Thread handle not inheritable.
		FALSE, // Set handle inheritance to FALSE.
		0, // No creation flags.
		NULL, // Use parent's environment block.
		NULL, // Use parent's starting directory.
		&si, // Pointer to STARTUPINFO structure.
		&pi); // Pointer to PROCESS_INFORMATION structure.

	SAFE_CLOSE_HANDLE(pi.hThread);
	SAFE_CLOSE_HANDLE(pi.hProcess);
}

void regSet()
{
	wstring wsIPIn;

	_wsetlocale(LC_ALL, _T("")); //设置或检索本地运行环境,避免wprintf打印中文乱码

	HKEY hKey = NULL; //保存注册表的句柄
	DWORD dwIndexs = 0; //需要返回子项的索引
	TCHAR keyName[MAX_PATH] = { 0 }; //保存子键的名称
	DWORD charLength = 256; //想要读取多少字节并返回实际读取到的字符长度
	TCHAR subKey[MAX_PATH] = _T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces");

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hKey))
	{
		while (ERROR_SUCCESS == RegEnumKeyEx(hKey, dwIndexs, keyName, &charLength, NULL, NULL, NULL, NULL))
		{
			wprintf(_T("%d : %s\n"), dwIndexs, keyName);

			HKEY hNodeKey = NULL; //对应具体网卡注册表子项的句柄
			TCHAR nodeKey[MAX_PATH] = { 0 };
			lstrcat(nodeKey, subKey);
			lstrcat(nodeKey, _T("\\"));
			lstrcat(nodeKey, keyName);
			cout << "info:" << keyName << endl;

			if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, nodeKey, 0, KEY_READ, &hNodeKey))
			{
				wstring wsIP;
				wchar_t szIP[64] = { 0 };
				DWORD dwLen = 64 * sizeof(wchar_t);
				DWORD dwType = REG_SZ;
				LONG lRet = RegQueryValueEx(hNodeKey, L"DhcpIPAddress", NULL, &dwType, (LPBYTE)szIP, &dwLen);
				if (ERROR_SUCCESS == lRet) {
					wsIP = szIP;

					//如果ip相等 则设置 metric 为 1 
					if (wsIPIn == wsIP) {
						DWORD dwValue = 0x1;
						auto setRet = RegSetValueEx(hNodeKey, L"InterfaceMetric", 0, REG_DWORD, (BYTE*)&dwValue, 4);

						break;
					}
				}
				if (hNodeKey != NULL)
				{
					RegCloseKey(hNodeKey);
				}
			}

			++dwIndexs;
			charLength = 256;
		}
	}
	if (hKey != NULL)
	{
		RegCloseKey(hKey);
	}
}

int main()
{
	callPwShell(23);

	return 0;
	regSet();

	std::cout << "Hello World!\n";
    std::cout << "Hello World!\n";
	// Declare and initialize variables
	PIP_INTERFACE_INFO pInfo = NULL;
	ULONG ulOutBufLen = 0;

	DWORD dwRetVal = 0;
	int iReturn = 1;

	int i;

	// Make an initial call to GetInterfaceInfo to get
	// the necessary size in the ulOutBufLen variable
	dwRetVal = GetInterfaceInfo(NULL, &ulOutBufLen);
	if (dwRetVal == ERROR_INSUFFICIENT_BUFFER) {
		pInfo = (IP_INTERFACE_INFO*)MALLOC(ulOutBufLen);
		if (pInfo == NULL) {
			printf
			("Unable to allocate memory needed to call GetInterfaceInfo\n");
			return 1;
		}
	}
	// Make a second call to GetInterfaceInfo to get
	// the actual data we need
	dwRetVal = GetInterfaceInfo(pInfo, &ulOutBufLen);
	if (dwRetVal == NO_ERROR) {
		printf("Number of Adapters: %ld\n\n", pInfo->NumAdapters);
		for (i = 0; i < pInfo->NumAdapters; i++) {
			printf("Adapter Index[%d]: %ld\n", i,
				pInfo->Adapter[i].Index);
			printf("Adapter Name[%d]: %ws\n\n", i,
				pInfo->Adapter[i].Name);
		}
		iReturn = 0;
	}
	else if (dwRetVal == ERROR_NO_DATA) {
		printf
		("There are no network adapters with IPv4 enabled on the local system\n");
		iReturn = 0;
	}
	else {
		printf("GetInterfaceInfo failed with error: %d\n", dwRetVal);
		iReturn = 1;
	}

	FREE(pInfo);
	return (iReturn);
}