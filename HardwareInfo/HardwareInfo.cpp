#include <iostream>
using namespace std;

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

#include <Windows.h>
#include <stdio.h>

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

int main(int argc, char** argv)
{
	//computerType();
	//getSysVersion();
	//getScreenScale();
	//GetZoom();
	getSysInfo();
	system("pause");
	return 0;

}
