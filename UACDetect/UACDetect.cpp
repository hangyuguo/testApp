// UACDetect.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#ifndef _WIN32_WINNT
//#define _WIN32_WINNT 0x0500
//#endif

//#ifndef WINVER
//#define WINVER 0x0501
//#endif

#include "conio.h"
#include <windows.h>
#include <iostream>
#include <time.h>
#include <atlstr.h>
#include <string.h>
#include <thread>
#include <future>
#include <ShlObj.h>
#include <WtsApi32.h>
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")
#pragma comment( lib, "Wtsapi32.lib" )
#include "lm.h"
#include "tlhelp32.h"
#pragma comment(lib,"netapi32.lib")


// Global variable.
HWINEVENTHOOK g_hook;

// Prototype
void CALLBACK HandleWinEvent(HWINEVENTHOOK, DWORD, HWND,
    LONG, LONG,
    DWORD, DWORD);

bool isProcessRunning(_In_ const std::string strProcessName);

// Callback function that handles events.
//
void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
    LONG idObject, LONG idChild,
    DWORD dwEventThread, DWORD dwmsEventTime)
{
    //std::cout << std::hex << event; // desperate attempt to see if any event is caught 

    if (event == EVENT_SYSTEM_SWITCHSTART)
    {
        std::cout << "Begin";
    }
    else if (event == EVENT_SYSTEM_SWITCHEND)
    {
        std::cout << "End ";
    }
    else if (event == EVENT_SYSTEM_DESKTOPSWITCH)
    {
        std::wcout << "监听到桌面切换消息！" << std::endl << std::endl;
        std::cout << "UAC pop-up " << std::endl;
        if (isProcessRunning("consent.exe")) {
            std::cout << "Warning! consent.exe is running!" << std::endl;
            std::wcout << L"-----------------Warning!!!----UAC-----------------" << std::endl << std::endl;
        }
    }
}

// Initializes COM and sets up the event hook.
//
void InitializeMSAA()
{
    CoInitialize(NULL);
    g_hook = SetWinEventHook(
        EVENT_MIN, EVENT_MAX,  // Range of events .
        NULL,                                          // Handle to DLL.
        HandleWinEvent,                                // The callback.
        0, 0,              // Process and thread IDs of interest (0 = all)
        WINEVENT_OUTOFCONTEXT); // Flags.
}

// Unhooks the event and shuts down COM.
//
void ShutdownMSAA()
{
    UnhookWinEvent(g_hook);
    CoUninitialize();
}

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p) = nullptr; } }
#define SAFE_CLOSE_HANDLE(p) { if(p) { CloseHandle(p); (p) = nullptr; } }

/*
* @brief Cstring转string
* @param[IN] CString 字符串
* @return string 转换结果，string字符串
*/
std::string CStringtoString(CString cs)
{
    CT2CA pszConvertedAnsiString(cs);
    std::string strStd(pszConvertedAnsiString);
    return strStd;
}


//检测名称为strProcess的进程是否正在运行
bool isProcessRunning(_In_ const std::string strProcessName)
{
    bool bExist = false;//是否找到该进程
    HANDLE handleTerminate = nullptr; //定义要结束进程句柄
    HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//获得系统快照句柄
    if (nullptr == handle) {
        return bExist;
    }

    //定义PROCESSENTRY32结构字指针
    PROCESSENTRY32* info = new(std::nothrow) PROCESSENTRY32;
    if (nullptr != info) {
        info->dwSize = sizeof(PROCESSENTRY32);

        //调用一次 Process32First 函数，从快照中获取进程列表,然后重复调用 Process32Next，直到函数返回 FALSE 为止
        bool bResult = Process32First(handle, info);
        while (bResult) {
            //获取进程名称
            std::string strName = CStringtoString(CString(info->szExeFile));
            if (strName == strProcessName) {
                bExist = true;
                break;
            }
            bResult = Process32Next(handle, info);
        }
        SAFE_DELETE(info)
    }
    else {
        std::cout << "PROCESSENTRY32 new error." << std::endl;
    }
    SAFE_CLOSE_HANDLE(handle);
    return bExist;
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

DWORD GetProcessIdByHWND(HWND handle)
{
    DWORD lpdwProcessId;
    GetWindowThreadProcessId(handle, (LPDWORD)&lpdwProcessId);

    return lpdwProcessId;
}

void getTopWindowPID() {
    //Find the top-most window.
    HWND hWnd = GetForegroundWindow();

    int pid = GetProcessIdByHWND(hWnd);

    CString csProcessName = getProcessName(pid);

    std::cout << "GetTopWindow pid:" << pid << std::endl;
    std::wcout << L"GetTopWindow csProcessName:" << csProcessName.GetString() << std::endl << std::endl;

    if (csProcessName.Find(_T("SmartPlayerWin")) == -1) {
        std::wcout << L"-----------------Warning!!!---------------------" << std::endl << std::endl;
    }
    else {
        std::wcout << L"***************OK**************" << std::endl << std::endl;
    }
}

void printTopWindowInfo() {
    while (true) {
        getTopWindowPID();
        Sleep(2000);
    }
}


int main()
{
    std::wcout << "开始检测置顶窗口信息" << std::endl;

    //线程函数 2s打印一次最顶端窗口信息
    std::thread th1(printTopWindowInfo);
    th1.detach();
    //std::async(std::launch::async, printTopWindowInfo);

    //开始监听窗口消息
    InitializeMSAA();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    //while (getchar() != 'q') { ; }
    ShutdownMSAA();
    return 0;
}