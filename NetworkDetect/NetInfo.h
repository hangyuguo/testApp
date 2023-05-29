#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <functional>
#include <Windows.h>
#include <netlistmgr.h>
#include <atlbase.h>
#include <objbase.h>
#include <wtypes.h>
#include <wlanapi.h>
#include <IPHlpApi.h>


#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "Iphlpapi.lib")
using std::string;


enum NetworkType
{
	NotNetwork,
	Ethernet,
	Wlan,
};

// 当前连通的网络适配器信息
struct NetConnectAdaptInfo
{
	NetConnectAdaptInfo() :ipAddress(""), guid(""), macAddress(""), gatewayIp(""), networkType(NetworkType::NotNetwork) {

	}
	string ipAddress;	// Ip Address
	string guid;		// GUDI
	string macAddress;	// Mac Address
	string gatewayIp;	// Default GateWay
	NetworkType networkType;
};

struct ConnectionInfo
{
	std::wstring guid;
	NetConnectAdaptInfo info;
};

std::wstring GUIDToString(const GUID& guid);
std::string UnicodeToUTF8(const std::wstring& wstr);

NetConnectAdaptInfo GetNetAdpaterInfo(const std::wstring& guid);


void init();
std::vector<ConnectionInfo> GetNetworkConnections();


void ShowNetworkInfo();

//----------------------------------------------------------------
