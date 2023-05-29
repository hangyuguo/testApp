#include "HardwareInfo.h"

#include <iostream>
#include <Windows.h>
#include <stdio.h>

extern "C" {
#include <Powrprof.h>
}
#pragma comment(lib, "Powrprof.lib")

#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")
#include <cfgmgr32.h>

using namespace std;


//判断台式机还是笔记本
void computerType()
{
	SYSTEM_POWER_STATUS a;
	GetSystemPowerStatus(&a);

	if (a.BatteryFlag == 128 || a.BatteryFlag == 255)
	{
		printf("台式电脑\n");
	}
	else
	{
		printf("笔记本电脑\n");
	}
}

void computerType2() {
	SYSTEM_POWER_CAPABILITIES  oPowerCapabilities;

	CallNtPowerInformation(POWER_INFORMATION_LEVEL::SystemPowerCapabilities, NULL, 0, &oPowerCapabilities, sizeof(oPowerCapabilities));

	if (oPowerCapabilities.LidPresent == 1)
	{
		cout << "Laptop" << endl;
	}
	else
	{
		cout << "PC" << endl;
	}
}

//#using "OpenHardwareMonitorLib.dll"  //DLL所在的路径
//using namespace OpenHardwareMonitor;
//using namespace OpenHardwareMonitor::Hardware;
//
//void getTemp()
//{
//	float cpu_temperature_celcius;
//	Computer^ computer = gcnew Computer();
//	computer->CPUEnabled = true;
//	computer->Open();
//
//	//OpenHardwareMonitor::Hardware::IHardware^ hardware = computer->Hardware[0];
//
//	/*for (auto hardware : computer->Hardware)
//	{
//		if (hardware.HardwareType == HardwareType.CPU) {
//			for (auto sensor : hardware.Sensors) {
//				if (sensor.SensorType == SensorType.Temperature)
//				{
//					cpu_temperature_celcius = sensor.Value;
//					cout << cpu_temperature_celcius << endl;
//				}
//			}
//		}
//			
//	}*/
//
//	system("pause");
//			
//}



//判断系统版本 7/8/10
void getSysVersion()
{
	std::string vname;
	//先判断是否为win8.1或win10
	typedef void(__stdcall* NTPROC)(DWORD*, DWORD*, DWORD*);
	HINSTANCE hinst = LoadLibrary(L"ntdll.dll");
	DWORD dwMajor, dwMinor, dwBuildNumber;
	NTPROC proc = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers");
	proc(&dwMajor, &dwMinor, &dwBuildNumber);
	if (dwMajor == 6 && dwMinor == 1)	//win 7 
	{
		vname = "Microsoft Windows 7";
		printf_s("此电脑的版本为:%s\n", vname.c_str());
		return;
	}
	if (dwMajor == 6 && dwMinor == 2)	//vista
	{
		vname = "Microsoft Windows vista";
		printf_s("此电脑的版本为:%s\n", vname.c_str());
		return;
	}
	if (dwMajor == 6 && dwMinor == 3)	//win 8.1
	{
		vname = "Microsoft Windows 8.1";
		printf_s("此电脑的版本为:%s\n", vname.c_str());
		return;
	}
	if (dwMajor >= 10)	//win 10 以上
	{
		vname = "Microsoft Windows 10";
		printf_s("此电脑的版本为:%s\n", vname.c_str());
		return;
	}

}

//获取屏幕放大比
double GetZoom()
{
	auto desktopDc1 = GetDC(NULL);
	double dpiA = (double)GetDeviceCaps(desktopDc1, DESKTOPHORZRES) / GetDeviceCaps(desktopDc1, HORZRES);
	double dpiB = (double)GetDeviceCaps(desktopDc1, LOGPIXELSX) / 0.96 / 100.0;

	if (dpiA == 1)
		return dpiB;
	else if (dpiB == 1)
		return dpiA;
	else if (dpiA == dpiB)
		return dpiA;
	else
		return 0;

}

//获取放大比 某些场景下无效 对比GetZoom()
void getScreenScale()
{
	// 获取窗口当前显示的监视器
	// 使用桌面的句柄.
	//HWND hWnd = GetDesktopWindow();
	//HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

	POINT pt;
	pt.x = 100;
	pt.y = 100;
	HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

	// 获取监视器逻辑宽度与高度
	MONITORINFOEX miex;
	miex.cbSize = sizeof(miex);
	GetMonitorInfo(hMonitor, &miex);
	int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);
	int cyLogical = (miex.rcMonitor.bottom - miex.rcMonitor.top);

	// 获取监视器物理宽度与高度
	DEVMODE dm;
	dm.dmSize = sizeof(dm);
	dm.dmDriverExtra = 0;
	EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
	int cxPhysical = dm.dmPelsWidth;
	int cyPhysical = dm.dmPelsHeight;

	// 缩放比例计算  实际上使用任何一个即可
	double horzScale = ((double)cxPhysical / (double)cxLogical);
	double vertScale = ((double)cyPhysical / (double)cyLogical);
	cout << "horzScale:" << horzScale << endl;
	cout << "vertScale:" << vertScale << endl;
}

// VC++得到计算机名称和用户名称  
// http://blog.csdn.net/morewindows/article/details/8659417
//By MoreWindows-(http://blog.csdn.net/MoreWindows)  
void getSysInfo()
{
	printf("    VC++得到计算机名称和用户名称 \n");
	printf(" -- By MoreWindows( http://blog.csdn.net/MoreWindows ) --\n\n");

	const int MAX_BUFFER_LEN = 500;
	TCHAR  szBuffer[MAX_BUFFER_LEN];
	DWORD dwNameLen;

	dwNameLen = MAX_BUFFER_LEN;
	if (!GetComputerName(szBuffer, &dwNameLen))
		printf("Error  %d\n", GetLastError());
	else {
		wstring wsComputerName = szBuffer;
		wcout << "计算机名为:" << wsComputerName << endl;
	}
		

	dwNameLen = MAX_BUFFER_LEN;
	if (!GetUserName(szBuffer, &dwNameLen))
		printf("Error  %d\n", GetLastError());
	else {
		wstring wsUserName = szBuffer;
		wcout << "当前用户名为:" << wsUserName << endl;
	}
		
}

#include <NetCon.h>
void ChangeNetState(bool bEnable)
{
	CoInitialize(NULL);
	INetConnectionManager* pNetManager;
	INetConnection* pNetConnection;
	IEnumNetConnection* pEnum;

	if (S_OK != CoCreateInstance(CLSID_ConnectionManager, NULL, CLSCTX_SERVER, IID_INetConnectionManager, (void**)&pNetManager))
	{
		return;
	}

	pNetManager->EnumConnections(NCME_DEFAULT, &pEnum);
	pNetManager->Release();
	if (NULL == pEnum)
	{
		return;
	}

	ULONG celtFetched;
	while (pEnum->Next(1, &pNetConnection, &celtFetched) == S_OK)
	{
		NETCON_PROPERTIES* properties;
		pNetConnection->GetProperties(&properties);
		wstring name = properties->pszwName; //网络连接的名称
		wstring DeviceName = properties->pszwDeviceName; //网卡名称
		

		if (DeviceName.find(L"ZTN") != wstring::npos) {
			if (properties->Status == NCS_DISCONNECTED) {
				cout << "ZTN网卡被禁用" << endl;
				pNetConnection->Connect();
			}
		}

		//cout << pNetConnection->Disconnect() << endl; //禁用网卡
		//cout << pNetConnection->Connect() << endl;    //启用网卡
	}
	CoUninitialize();
	return;
}


bool driverRestart()
{
	clock_t start, end;
	//等待vpn初始化完成信息
	start = clock();

	bool bResult = false;

	CoInitialize(NULL);
	INetConnectionManager* pNetManager;
	INetConnection* pNetConnection;
	IEnumNetConnection* pEnum;

	if (S_OK != CoCreateInstance(CLSID_ConnectionManager, NULL, CLSCTX_SERVER, IID_INetConnectionManager, (void**)&pNetManager))
	{
		cout << "ZTN网卡状态检测失败" << endl;
		return bResult;
	}

	pNetManager->EnumConnections(NCME_DEFAULT, &pEnum);
	pNetManager->Release();
	if (NULL == pEnum)
	{
		cout << "ZTN网卡状态检测失败" << endl;
		return bResult;
	}

	ULONG celtFetched;
	while (pEnum->Next(1, &pNetConnection, &celtFetched) == S_OK)
	{
		wstring name, deviceName;
		NETCON_PROPERTIES* properties;
		pNetConnection->GetProperties(&properties);
		if (properties->pszwName) {
			name = properties->pszwName; //网络连接的名称
		}
		if (properties->pszwDeviceName) {
			deviceName = properties->pszwDeviceName; //网卡名称
		}

		if (deviceName.find(L"ZTN") != wstring::npos) {
			//如果找到 ztn网卡 先默认为启用状态
			bResult = true;
			if (properties->Status == NCS_MEDIA_DISCONNECTED) {
				cout << "先禁用ZTN网卡" << endl;
				pNetConnection->Disconnect();
			}
			cout << "启用ZTN网卡" << endl;
			pNetConnection->Connect();
		}
	}
	CoUninitialize();


	end = clock();
	double dTimeStamp = (double)(end - start) / CLOCKS_PER_SEC;
	cout << "driverRestart TimeStamp:" << dTimeStamp << endl;

	return bResult;
}


#include <atlcomcli.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

#pragma comment(lib, "comsuppw.lib")

bool DetectMoniter(std::vector<CAPITAL_HARDWARE>& moniter_arr)
{
	InitComLibrary init;
	WmicQuery query;
	query.ExecQuery(L"SELECT * FROM WmiMonitorID", L"ROOT\\WMI");
	IEnumWbemClassObject* pEnum = query.pEnum_;
	if (pEnum == NULL)
	{
		return false;
	}

	bool bRet = false;

	do {
		CComPtr<IWbemClassObject> pObj;
		DWORD dwReturn = 0;
		if (S_OK != pEnum->Next(WBEM_INFINITE, 1, &pObj, &dwReturn)) {
			break;
		}

		CAPITAL_HARDWARE moniterItem;
		CComVariant varValue;
		if (pObj->Get(L"UserFriendlyName", 0, &varValue, NULL, 0) == S_OK && varValue.vt > 8192)
		{
			LONG l = 0;
			LONG h = 0;
			std::wstring strTemp;
			SafeArrayGetLBound(varValue.parray, 1, &l);
			SafeArrayGetUBound(varValue.parray, 1, &h);
			long Size = h - l + 1;
			for (long Idx = l; Idx < Size; ++Idx)
			{
				int nVl = 0;
				SafeArrayGetElement(varValue.parray, &Idx, &nVl);
				strTemp.push_back((wchar_t)nVl);
			}

			moniterItem.strName = strTemp;
		}

		varValue.Clear();

		if (moniterItem.strName.empty()) {
			break;
		}


		if (pObj->Get(L"SerialNumberID", 0, &varValue, NULL, 0) == S_OK)
		{
			LONG l = 0;
			LONG h = 0;
			std::wstring strTemp;
			SafeArrayGetLBound(varValue.parray, 1, &l);
			SafeArrayGetUBound(varValue.parray, 1, &h);
			long Size = h - l + 1;
			for (long Idx = l; Idx < Size; ++Idx)
			{
				int nVl = 0;
				SafeArrayGetElement(varValue.parray, &Idx, &nVl);
				strTemp.push_back((wchar_t)nVl);
			}

			moniterItem.strSerialNumber = strTemp;
		}
		varValue.Clear();
		if (pObj->Get(L"InstanceName", 0, &varValue, NULL, 0) == S_OK)
		{
			moniterItem.strDeviceInstanceName = varValue.bstrVal;
		}
		varValue.Clear();

		int nWeek = 0;
		if (pObj->Get(L"WeekOfManufacture", 0, &varValue, NULL, 0) == S_OK)
		{
			nWeek = varValue.uiVal;
		}
		varValue.Clear();

		int nYear = 0;
		if (pObj->Get(L"YearOfManufacture", 0, &varValue, NULL, 0) == S_OK)
		{
			nYear = varValue.uintVal;
		}

		wchar_t szProductDate[64] = { 0 };
		swprintf_s(szProductDate, L"%d年%d周", nYear, nWeek);
		moniterItem.mapProporties[L"productDate"] = szProductDate;

		varValue.Clear();
		if (pObj->Get(L"ManufacturerName", 0, &varValue, NULL, 0) == S_OK)
		{
			LONG l = 0;
			LONG h = 0;
			std::wstring strTemp;
			SafeArrayGetLBound(varValue.parray, 1, &l);
			SafeArrayGetUBound(varValue.parray, 1, &h);
			long Size = h - l + 1;
			for (long Idx = l; Idx < Size; ++Idx)
			{
				int nVl = 0;
				SafeArrayGetElement(varValue.parray, &Idx, &nVl);
				strTemp.push_back((wchar_t)nVl);
			}

			moniterItem.strManufacturer = strTemp;
		}

		moniterItem.hardType = L"MONITOR";
		moniter_arr.push_back(moniterItem);
		bRet = true;
	} while (TRUE);

	return bRet;
}

#include <comutil.h>

WmicQuery::WmicQuery()
{
	pEnum_ = NULL;
}

WmicQuery::~WmicQuery()
{
	ClearResult();
}

IEnumWbemClassObject* WmicQuery::ExecQuery(const wchar_t* pQuery, const wchar_t* pRoot)
{
	ClearResult();
	IWbemServices* pService = service_.GetService(pRoot);
	if (pService == nullptr)
	{
		return nullptr;
	}

	if (FAILED(pService->ExecQuery(_bstr_t("WQL"), _bstr_t(pQuery),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnum_)))
	{
		return nullptr;
	}

	return pEnum_;
}

void WmicQuery::ClearResult()
{
	if (pEnum_)
	{
		pEnum_->Release();
		pEnum_ = NULL;
	}
}

WmiService::WmiService()
{
	pLoc_ = NULL;
	pSvc_ = NULL;
}

WmiService::~WmiService()
{
	Clear();
}

IWbemServices* WmiService::GetService(const wchar_t* pRoot)
{
	if (pSvc_ && pLoc_)
	{
		return pSvc_;
	}

	Clear();

	HRESULT hRes = CoInitializeSecurity(NULL, -1, NULL, NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL, EOAC_NONE, NULL);

	if ((RPC_E_TOO_LATE != hRes) && FAILED(hRes))
	{
		return nullptr;
	}

	if (FAILED(CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc_)))
	{
		return nullptr;
	}

	if (FAILED(pLoc_->ConnectServer(_bstr_t(pRoot), NULL, NULL, 0, NULL, 0, 0, &pSvc_)))
	{
		return nullptr;
	}

	if (FAILED(CoSetProxyBlanket(pSvc_,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE)))
	{

		return nullptr;
	}

	return pSvc_;
}

void WmiService::Clear()
{
	if (pSvc_)
	{
		pSvc_->Release();
		pSvc_ = nullptr;
	}

	if (pLoc_)
	{
		pLoc_->Release();
		pLoc_ = nullptr;
	}
}
