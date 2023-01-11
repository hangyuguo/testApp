// windowTop.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <atlstr.h>
#include <string.h>
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")

#define SAFE_CLOSE_HANDLE(p) { if(p) { CloseHandle(p); (p) = nullptr; } }

// windows置顶窗体终极方案

BOOL OnForceShow(HWND hWnd)
{
	HWND hForeWnd = NULL;
	DWORD dwForeID = 0;
	DWORD dwCurID = 0;

	hForeWnd = ::GetForegroundWindow();
	dwCurID = ::GetCurrentThreadId();
	dwForeID = ::GetWindowThreadProcessId(hForeWnd, NULL);
	::AttachThreadInput(dwCurID, dwForeID, TRUE);
	::ShowWindow(hWnd, SW_SHOWNORMAL);
	::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	::SetForegroundWindow(hWnd);
	// 将前台窗口线程贴附到当前线程（也就是程序A中的调用线程）
	::AttachThreadInput(dwCurID, dwForeID, FALSE);

	return TRUE;
}

DWORD GetProcessIdByHWND(HWND handle)
{
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(handle, (LPDWORD)&lpdwProcessId);

	return lpdwProcessId;
}

/*
*   getProcessName
*   根据pid获取进程名称
*   Param[IN] DWORD dwPID 进程号
*   @return CString 进程名称
*/
CString getProcessName(_In_ DWORD dwPID)
{
	CString csProcessName;
	HANDLE hHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (hHandle) {
		TCHAR szPath[MAX_PATH];
		if (GetModuleFileNameEx(hHandle, NULL, szPath, MAX_PATH)) {
			csProcessName.Format(_T("%s"), szPath);
		}
		else {
		std::cout << "GetModuleFileName failed" << std::endl;
		}
	}
	SAFE_CLOSE_HANDLE(hHandle);
	int iPos = csProcessName.ReverseFind('\\');
	auto name = csProcessName.Mid(iPos + 1);
	return name;
}

void getTopWindowPID() {
	//Find the top-most window.
	HWND hWnd = GetForegroundWindow();

	int pid = GetProcessIdByHWND(hWnd);

	CString csProcessName = getProcessName(pid);

	std::cout << "GetTopWindow pid:" << pid << std::endl;
	std::wcout << L"GetTopWindow csProcessName:" << csProcessName.GetString() << std::endl;


	//Run from the highest z-order to the lowest by using GW_HWNDNEXT. We're using a do-while
	//here because we don't want to skip the top-most window in this loop.
	//..

	//do
	//{
	//	//Your code here..
	//	
	//} while (window = GetWindow(window, GW_HWNDNEXT));

}

int main(int argc, char* argv[])
{
	/*while (1) {
		HWND hWnd = ::GetForegroundWindow();
		if (!OnForceShow(hWnd)) {
			::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 100, 100, SWP_NOMOVE | SWP_NOSIZE);
		}
	}*/

	for (int i = 0; i < 1000; i++) {
		getTopWindowPID();
		Sleep(1000);
	}
	

	//system("pause");
	
	return 0;
}
