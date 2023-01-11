#include <Pdh.h>
#include <PdhMsg.h>
#pragma comment(lib,"pdh.lib") 

#include <iostream>
using namespace std;

#include <IPHlpApi.h>
#pragma comment(lib, "IPHlpApi.lib")

string getNetInterface() {
    ULONG ulSize = 0;
    IP_ADAPTER_INFO* pAdapter = nullptr;
    if (GetAdaptersInfo(pAdapter, &ulSize) == ERROR_BUFFER_OVERFLOW) {
        pAdapter = (IP_ADAPTER_INFO*)new char[ulSize];
    }
    else {
        cout << "GetAdaptersInfo fail" << endl;
        return "";
    }

    if (GetAdaptersInfo(pAdapter, &ulSize) != ERROR_SUCCESS) {
        cout << "GetAdaptersInfo fail" << endl;
        return "";
    }

    IPAddr ipAddr = { 0 };
    DWORD dwIndex = -1;
    DWORD nRet = GetBestInterface(ipAddr, &dwIndex);
    if (NO_ERROR != nRet) {
        cout << "GetBestInterface fail: " << nRet << endl;
    }

    string strInterface;
    for (auto* pCur = pAdapter; pCur != NULL; pCur = pCur->Next) {
        //if (pCur->Type != MIB_IF_TYPE_ETHERNET)
        //  continue;

        if (pCur->Index == dwIndex) {
            cout << "Best Interface!! ";
            strInterface = pCur->Description;
        }

        cout << "Descrip: " << pCur->Description;
        cout << ", Name: " << pCur->AdapterName << endl;
        cout << "IP: " << pCur->IpAddressList.IpAddress.String;
        cout << ", Gateway: " << pCur->GatewayList.IpAddress.String << endl << endl;
    }

    delete pAdapter;
    return strInterface;
}

void getResourceCounter()
{
    HQUERY query;
    PDH_STATUS status = PdhOpenQuery(NULL, NULL, &query);
    if (status != ERROR_SUCCESS)
        cout << "Open Query Error" << endl;

    HCOUNTER cpuCounter, memCounter;
    HCOUNTER recvCounter, sentCounter;

    string strGet = getNetInterface();
    /*wstring strInterface = L"\\Network Interface(" + xugd::clib::XuStr::str2wstr(strGet) + L")\\";
    wcout << strInterface << endl;*/
    string strInterface = "\\Network Interface(" + strGet + ")\\";
    cout << strInterface << endl;

    status = PdhAddCounter(query, TEXT("\\Processor Information(_Total)\\% Processor Time"), NULL, &cpuCounter);
    if (status != ERROR_SUCCESS)
        cout << "Add CPU Counter Error" << endl;
    status = PdhAddCounter(query, TEXT("\\Memory\\Available MBytes"), NULL, &memCounter);
    if (status != ERROR_SUCCESS)
        cout << "Add Memory Counter Error" << endl;

    status = PdhAddCounterA(query, (strInterface + "Bytes Received/sec").c_str(), NULL, &recvCounter);
    if (status != ERROR_SUCCESS)
        cout << "Add Received Counter Error" << endl;
    status = PdhAddCounterA(query, (strInterface + "Bytes Sent/sec").c_str(), NULL, &sentCounter);
    if (status != ERROR_SUCCESS)
        cout << "Add Sent Counter Error" << endl;


    int nIndex = 0;
    //cout << setiosflags(ios::fixed) << setprecision(4);
    while (true) {
        PdhCollectQueryData(query);
        Sleep(1000);

        PdhCollectQueryData(query);

        PDH_FMT_COUNTERVALUE pdhValue;
        DWORD dwValue;

        status = PdhGetFormattedCounterValue(cpuCounter, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
        if (status != ERROR_SUCCESS)
            cout << "Get Value Error" << endl;
        cout << /*setw(3) <<*/ ++nIndex << " - CPU: " << pdhValue.doubleValue << "%";

        status = PdhGetFormattedCounterValue(memCounter, PDH_FMT_LONG, &dwValue, &pdhValue);
        if (status != ERROR_SUCCESS)
            cout << "Get Value Error" << endl;
        cout << "; \tMemory: " << pdhValue.longValue << "MB";

        status = PdhGetFormattedCounterValue(recvCounter, PDH_FMT_LONG, &dwValue, &pdhValue);
        if (status != ERROR_SUCCESS)
            cout << "Get Value Error" << endl;
        cout << "; \tRecv: " << pdhValue.longValue;
        status = PdhGetFormattedCounterValue(sentCounter, PDH_FMT_LONG, &dwValue, &pdhValue);
        if (status != ERROR_SUCCESS)
            cout << "Get Value Error" << endl;
        cout << "; \tSent: " << pdhValue.longValue << endl;

        Sleep(1000 * 10);
    }

    PdhCloseQuery(query);
}


void getRemoteResourceCounter()
{
    HQUERY query;
    PDH_STATUS status = PdhOpenQuery(NULL, NULL, &query);
    if (status != ERROR_SUCCESS)
        cout << "Open Query Error" << endl;

    HCOUNTER averageEncodingTime;
    status = PdhAddCounter(query, TEXT("\\RemoteFX Graphics(RDP-tcp 3)\\Average Encoding Time"), NULL, &averageEncodingTime);
    if (status != ERROR_SUCCESS)
        cout << "Add Average Encoding Time Error" << endl;

    HCOUNTER frameQuality;
    status = PdhAddCounter(query, TEXT("\\RemoteFX Graphics(RDP-tcp 3)\\Frame Quality"), NULL, &frameQuality);
    if (status != ERROR_SUCCESS)
        cout << "Add Frame Quality Error" << endl;

    HCOUNTER baseTCPRTT;
    status = PdhAddCounter(query, TEXT("\\RemoteFX Network(RDP-tcp 3)\\Base TCP RTT"), NULL, &baseTCPRTT);
    if (status != ERROR_SUCCESS)
        cout << "Add Base TCP RTT Error" << endl;

    HCOUNTER inputFrames;
    status = PdhAddCounter(query, TEXT("\\RemoteFX Graphics(RDP-tcp 3)\\Input Frames/Second"), NULL, &inputFrames);
    if (status != ERROR_SUCCESS)
        cout << "Add Input Frames/Second Error" << endl;

    HCOUNTER outputFrames;
    status = PdhAddCounter(query, TEXT("\\RemoteFX Graphics(RDP-tcp 3)\\Output Frames/Second"), NULL, &inputFrames);
    if (status != ERROR_SUCCESS)
        cout << "Add Input Frames/Second Error" << endl;

    int nIndex = 0;
    //cout << setiosflags(ios::fixed) << setprecision(4);
    while (true) {
        PdhCollectQueryData(query);
        Sleep(1000);

        PdhCollectQueryData(query);

        PDH_FMT_COUNTERVALUE pdhValue;
        DWORD dwValue;

        status = PdhGetFormattedCounterValue(averageEncodingTime, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
        if (status != ERROR_SUCCESS) {
            cout << endl << "Get Value Error" << endl;
        }
        else {
            cout << /*setw(3) <<*/ ++nIndex << " - Average Encoding Time: " << pdhValue.doubleValue << endl;
        }
           
        status = PdhGetFormattedCounterValue(frameQuality, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
        if (status != ERROR_SUCCESS) {
            cout << endl << "Get Value Error" << endl;
        }
        else {
            cout << /*setw(3) <<*/ ++nIndex << " - Frame Quality: " << pdhValue.doubleValue << endl;
        }

        status = PdhGetFormattedCounterValue(baseTCPRTT, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
        if (status != ERROR_SUCCESS) {
            cout << endl << "Get Value Error" << endl;
        }
        else {
            cout << /*setw(3) <<*/ ++nIndex << " - Base TCP RTT: " << pdhValue.doubleValue << endl;
        }

        status = PdhGetFormattedCounterValue(inputFrames, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
        if (status != ERROR_SUCCESS) {
            cout << endl << "Get Value Error" << endl;
        }
        else {
            cout << /*setw(3) <<*/ ++nIndex << " - Input Frames/Second: " << pdhValue.doubleValue << endl;
        }

        cout << endl << endl;
        Sleep(1000 * 10);
    }

    PdhCloseQuery(query);
}


#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>


//获取实例名称
void getInstanceName()
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    std::wstring wsProcessName;
    DWORD dwSessionId, dwPid, dwErr;
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            dwPid = entry.th32ProcessID;
            wsProcessName = entry.szExeFile;
            if (!ProcessIdToSessionId(dwPid, &dwSessionId))
            {
                dwErr = GetLastError();
                std::wcout << "Error " << wsProcessName.c_str() << "\t" << entry.th32ProcessID << "\t" << entry.th32ModuleID << "\t" << dwErr << "\n";
            }
            else
                std::wcout << wsProcessName.c_str() << "\t" << entry.th32ProcessID << "\t" << dwSessionId << "\n";
            dwSessionId = NULL;
            dwErr = NULL;
        }
    }
    CloseHandle(snapshot);
}



#pragma warning ( disable : 4996 )

int run_cmd(const char* cmd, string& strOut)
{
    char MsgBuff[1024];
    int MsgLen = 1020;
    FILE* fp;
    if (cmd == NULL)
    {
        return -1;
    }
    if ((fp = _popen("cmd /k ipconfig", "r")) == NULL)
    //if((fp = _popen("C:\\Program\" \"Files\\Internet\" \"Explorer\\iexplore.exe", "r")) == NULL)
    {
        return -2;
    }
    else
    {
        memset(MsgBuff, 0, MsgLen);

        //读取命令执行过程中的输出
        while (fgets(MsgBuff, MsgLen, fp) != NULL)
        {
            //printf("MsgBuff: %s\n", MsgBuff);
            strOut += MsgBuff;
            cout << MsgBuff;
        }
        //cout << strOut << endl;

        //关闭执行的进程
        if (_pclose(fp) == -1)
        {
            return -3;
        }
    }
    return 0;
}

void getUserCount() {
    int i = 0;
    string strResult;
    //定义存放结果的字符串数组   
    const char* cmd = "query user";
    char* cmd1 = new char[strlen(cmd) + 1];
    strcpy(cmd1, cmd);
    if (0 == run_cmd(cmd1, strResult)) {
        printf("%s\n", strResult.c_str());
        /*const char* d = " ,*";
        char* p;
        p = strtok(strResult.c_str(), d);
        printf(result);
        while (p)
        {
            i++;
            printf("%d==%s\n", i, p);
            p = strtok(NULL, d);
        }*/
    }
    //int count = ((i - 6) + 1) / 7;

    system("pause");
}

#include <windows.h>
#include <Lmcons.h>
void getUserName()
{
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserNameA(username, &username_len);
    cout << "username:" << username << endl;
}

#include <wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")
//获取当前会话名称 winstation name
//ref https://docs.microsoft.com/en-us/windows/win32/api/wtsapi32/ne-wtsapi32-wts_info_class?redirectedfrom=MSDN
void getCurrentSession()
{
    PWTS_SESSION_INFO pSessionInfo = 0;
    DWORD dwCount = 0;
    ::WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount);
    int session_id = 0;
    for (DWORD i = 0; i < dwCount; ++i)
    {
        WTS_SESSION_INFO si = pSessionInfo[i];
        if (WTSActive == si.State)
        {
            session_id = si.SessionId;
            wcout << L"SessionId:" << si.SessionId << endl;
            wcout << L"pWinStationName:" << si.pWinStationName << endl;
            break;
        }
    }

    ::WTSFreeMemory(pSessionInfo);
}

void main()
{
    //getResourceCounter();
    //getRemoteResourceCounter();
    //getInstanceName();


    //getUserCount();
    getCurrentSession();
}