// getSSID.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#include <iostream>
//#include <stdio.h>
//#include <Windows.h>
//#include <wlanapi.h>
//#pragma comment(lib, "wlanapi.lib")
//
//int main()
//{
//	int id = 0;
//	HANDLE ClientHandle;
//	DWORD nv, i, c;
//	PWLAN_INTERFACE_INFO_LIST ilist;
//	PWLAN_AVAILABLE_NETWORK_LIST nlist;
//	static char ssid[36];
//
//	if (WlanOpenHandle(1, 0, &nv, &ClientHandle) == 0)
//	{
//		if (WlanEnumInterfaces(ClientHandle, 0, &ilist) == 0)
//		{
//			if (ilist->dwNumberOfItems > 0) {
//				for (i = 0; i < ilist->dwNumberOfItems; i++)
//				{
//					wprintf(L"\n\n  %s%s\n\n", L"网卡:", ilist->InterfaceInfo[i].strInterfaceDescription);
//					if (WlanGetAvailableNetworkList(ClientHandle, &ilist->InterfaceInfo[i].InterfaceGuid, 0, 0, &nlist) == 0)
//					{
//						for (c = 0; c < nlist->dwNumberOfItems; c++)
//						{
//							memcpy(ssid, nlist->Network[c].dot11Ssid.ucSSID, nlist->Network[c].dot11Ssid.uSSIDLength);
//							ssid[nlist->Network[c].dot11Ssid.uSSIDLength] = 0;
//							printf("   %3d. SSID:  %-25s  信号强度¨¨:  %5d\n", id++, ssid, nlist->Network[c].wlanSignalQuality);
//							//可继续获取BSSID
//						}
//						WlanFreeMemory(nlist);
//					}
//				}
//			} else {
//				std::cout << "No Wlan Item exist." << std::endl;
//			}
//			
//			WlanFreeMemory(ilist);
//		}
//		WlanCloseHandle(ClientHandle, 0);
//	}
//	system("pause");
//	return true;
//}

#include <iostream>
#include <string>
#include <atlstr.h>

#include "CWlanInfo.h"

int wmain()
{
	CWlanInfo wlanInfo;
	auto csSsid = wlanInfo.getCurrentSSID();
	std::wcout << "csSsid:" << csSsid.GetString() << std::endl;
	
}