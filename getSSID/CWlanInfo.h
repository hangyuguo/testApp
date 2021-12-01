#pragma once

#include <atlstr.h>

class CWlanInfo
{
public:
	CWlanInfo() = default;
	~CWlanInfo() = default;

public:
	//��ȡ��ǰ���ӵ�WLAN SSID ��δ�����򷵻ؿ�
	CString getCurrentSSID();
private:
	bool acquireWLANInformation();
private:
	CString m_csSSID;
};

