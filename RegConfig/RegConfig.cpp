// RegConfig.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//注册表相关操作测试工程

#include "RegConfig.h"
#include <iostream>
#include <windows.h>
#include <tchar.h>

using namespace std;
int execmd(char* cmd, char* result) {
	char buffer[128];                         // 缓冲区                        
	FILE* pipe = _popen(cmd, "r");            // 管道 

	// 管道打开失败
	if (!pipe) { return 0; }

	// 检测管道中的结束符，0表示没有结束
	while (!feof(pipe)) {
		// 从管道中读取数据
		if (fgets(buffer, 128, pipe)) {
			// 拼接 char
			strcat_s(result, 128, buffer);
		}
	}

	//关闭管道 
	_pclose(pipe);

	return 1;
}

void querySysLog()
{
	char result[0x7ffff] = "";        // 存放结果

	string strCmd = "wevtutil qe System /c:300 /rd:true /f:text";

		// 获取命令行返回值（保险起见这里获取 300 条日志信息）
	if (execmd((char*)strCmd.c_str(), result) == 1) {
		cout << result << endl;
	}

	// 查找关键数据
	string s = result;
	while ((s.find("igfx")) != -1) {
		cout << "找到了 igfx " << endl;
		break;
	}

	system("pause");
}


int main()
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
						//优先级设置为最高
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
	system("pause");
	return 0;
}

