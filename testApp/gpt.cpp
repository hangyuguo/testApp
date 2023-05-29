#include "gpt.h"

void gptTest::deleteOldVersions(std::string version)
{
    /*std::string path = "C:\\Program Files (x86)\\XiaoYiZtnAgent\\";
    std::vector<std::string> subfolders;
    for (const auto& entry : std::experimental::filesystem::directory_iterator(path)) {
        if (entry.status().type() == std::experimental::filesystem::file_type::directory) {
            std::string folderName = entry.path().filename().string();
            if (folderName < version) {
                subfolders.push_back(folderName);
            }
        }
    }
    for (const auto& subfolder : subfolders) {
        std::string subfolderPath = path + subfolder;
        std::experimental::filesystem::remove_all(subfolderPath);
        cout << "delete:" << subfolderPath << endl;
    }*/
}

// ����һ�����������ڱȽ��������Ĵ�С�����ش��һ��	
int max(int num1, int num2) {
	// ����ֲ����� result
	int result;
	if (num1 > num2)
		result = num1;
	else
		result = num2;
	return result;
}

#include <iostream>
#include <Windows.h>
#include <Wtsapi32.h>

#pragma comment(lib, "Wtsapi32.lib")

int gptTest::getActiveUserCount()
{
	DWORD count;
	PWTS_SESSION_INFO pSessionInfo;
	WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &count);
	std::cout << "��ǰ��û�������" << count << std::endl;
	return count;
}

void gptTest::getMemoryUsage()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
	DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
	std::cout << "�ڴ�ʹ�������" << std::endl;
	std::cout << "�������ڴ棺" << totalPhysMem / 1024 / 1024 << "MB" << std::endl;
	std::cout << "��ʹ�������ڴ棺" << physMemUsed / 1024 / 1024 << "MB" << std::endl;
	float physMemUsedPercent = (float)physMemUsed / (float)totalPhysMem;
	std::cout << "�����ڴ�ʹ���ʣ�" << physMemUsedPercent * 100 << "%" << std::endl;

	std::cout << "�������ڴ棺" << totalVirtualMem / 1024 / 1024 << "MB" << std::endl;
	std::cout << "��ʹ�������ڴ棺" << virtualMemUsed / 1024 / 1024 << "MB" << std::endl;
	float virtualMemUsedPercent = (float)virtualMemUsed / (float)totalVirtualMem;
	std::cout << "�����ڴ�ʹ���ʣ�" << virtualMemUsedPercent * 100 << "%" << std::endl;
}

__int64 Filetime2Int64(const FILETIME& ftime)
{
    LARGE_INTEGER li;
    li.LowPart = ftime.dwLowDateTime;
    li.HighPart = ftime.dwHighDateTime;
    return li.QuadPart;
}

__int64 CompareFileTime2(const FILETIME& preTime, const FILETIME& nowTime)
{
    return Filetime2Int64(nowTime) - Filetime2Int64(preTime);
}

double gptTest::getCPUUsage()
{
    FILETIME preIdleTime;
    FILETIME preKernelTime;
    FILETIME preUserTime;
    GetSystemTimes(&preIdleTime, &preKernelTime, &preUserTime);

    Sleep(1000);

    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);

    auto idle = CompareFileTime2(preIdleTime, idleTime);
    auto kernel = CompareFileTime2(preKernelTime, kernelTime);
    auto user = CompareFileTime2(preUserTime, userTime);

    if (kernel + user == 0)
        return 0;

    auto ret = 1.0 * (kernel + user - idle) / (kernel + user);
    std::cout << "CPUʹ���ʣ�" << ret * 100 << "%" << std::endl;
    return ret;
}

void gptTest::setWindowTop(std::wstring windowName)
{
	//HWND hWnd = FindWindow(NULL, L"Bulk Crap Uninstaller v5.5 ��Я x64");
	HWND hWnd = FindWindow(NULL, windowName.c_str());

	HWND hForeWnd = ::GetForegroundWindow();
	DWORD dwForeID = GetWindowThreadProcessId(hForeWnd, NULL);
	DWORD dwCurID = GetCurrentThreadId();
	AttachThreadInput(dwCurID, dwForeID, TRUE);

	::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	::SetForegroundWindow(hWnd);

	SendMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 0);
	SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);

	AttachThreadInput(dwCurID, dwForeID, FALSE);
}

int gptTest::reboot()
{
	cout << "rebooting..." << endl;
	system("shutdown -r -t 0");

    //HANDLE hToken;
    //TOKEN_PRIVILEGES tkp;

    //// ��ȡ�������ƾ��
    //if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    //    return GetLastError();

    //// ��ȡ�ػ���Ȩ
    //LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    //tkp.PrivilegeCount = 1;
    //tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    //// ���ùػ���Ȩ
    //AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    //if (GetLastError() != ERROR_SUCCESS)
    //    return GetLastError();

    //// �ػ�
    //if (!ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_FLAG_PLANNED))
    //    return GetLastError();

    return 0;
}

#include "base64/base64.h"

void gptTest::base64Test()
{
    string str = "hello world";
    string strBase64 = base::encode64(str);
    cout << "strBase64:" << strBase64 << endl;

    string strDecode = base::decode64(strBase64);
    cout << "strDecode:" << strDecode << endl;
}

void gptTest::getDeviceName()
{
    wchar_t szComputerName[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
    DWORD dwMaxLength = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerNameW(szComputerName, &dwMaxLength);
    auto deviceName_ = szComputerName;
    std::wcout << L"deviceName_:" << deviceName_ << endl;
}

//BOOL WINAPI HandlerRoutine(DWORD dwCtrlType) {
//    // �����˳��¼�
//    return TRUE;
//}
//
//void WINAPI WTSNotificationRoutine(WTSSESSION_NOTIFICATION* pNotification) {
//    if (pNotification->dwSessionId == WTSGetActiveConsoleSessionId()) {
//        if (pNotification->dwNotificationType == WTS_SESSION_SWITCH) {
//            // ��⵽ϵͳ��¼�û��л��¼�
//
//        }
//    }
//}
//
//void userLogoutDetect() {
//    // ע�����̨�˳��¼�
//    SetConsoleCtrlHandler(HandlerRoutine, TRUE);
//
//    // ע��Ự֪ͨ
//    WTSRegisterSessionNotification(HWND_MESSAGE, NOTIFY_FOR_THIS_SESSION);
//
//    // ��Ϣѭ��
//    MSG msg;
//    while (GetMessage(&msg, NULL, 0, 0)) {
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//
//    // ע���Ự֪ͨ
//    WTSUnRegisterSessionNotification(HWND_MESSAGE);
//
//    return;
//}