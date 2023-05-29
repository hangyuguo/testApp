#pragma once
#include <string>
#include <Windows.h>
#include <netfw.h>

class CWinFirewall
{
public:
	CWinFirewall() = default;
	~CWinFirewall() = default;

	//增加防火墙网络访问拦截通过路径
	static void AddFirewall(std::wstring filePath, std::wstring fileName);

private:

	static HRESULT WindowsFirewallAppIsEnabled(IN INetFwProfile* fwProfile,
		IN const wchar_t* fwProcessImageFileName,
		OUT BOOL* fwAppEnabled);

	static HRESULT WindowsFirewallInitialize(OUT INetFwProfile** fwProfile);

	static void WindowsFirewallCleanup(IN INetFwProfile* fwProfile);

	static HRESULT WindwsFirewallRemove(IN INetFwProfile* fwProfile,
		IN const wchar_t* fwProcessImageFileName,
		IN const wchar_t* fwName);

	static HRESULT WindowsFirewallAddApp(IN INetFwProfile* fwProfile,
		IN const wchar_t* fwProcessImageFileName,
		IN const wchar_t* fwName);

	static void Check(HRESULT hr, const char* str, INetFwProfile* fwProfile, HRESULT comInit);

	
};

