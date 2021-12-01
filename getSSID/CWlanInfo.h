#pragma once

#include <atlstr.h>

class CWlanInfo
{
public:
	CWlanInfo() = default;
	~CWlanInfo() = default;

public:
	//获取当前连接的WLAN SSID 若未连接则返回空
	CString getCurrentSSID();
private:
	bool acquireWLANInformation();
private:
	CString m_csSSID;
};

