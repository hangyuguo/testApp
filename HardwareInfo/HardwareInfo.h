#pragma once
#include <vector>
#include <string>
#include <map>

void computerType();

void computerType2();

void getSysVersion();

double GetZoom();

void getScreenScale();

void getSysInfo();

//获取网卡状态
void netDriverStatus();

//---------------------------------------------------------------------------
//功能:ChangeNetState ZTN网卡的启用与禁用
//参数:Enabled   true = 启用   false = 禁用
//返回值
//---------------------------------------------------------------------------
void ChangeNetState(bool bEnable);

//重启ztn驱动
bool driverRestart();


//---------------------------------------------------------------------------

#include <comutil.h>
class InitComLibrary
{
public:
	InitComLibrary() {
		bSuc_ = true;
		if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) {
			bSuc_ = false;
		}
	};
	~InitComLibrary() {
		if (bSuc_) {
			CoUninitialize();
		}
	};
private:
	bool bSuc_;
};

#include <wbemcli.h>
class WmiService
{
public:
	WmiService();
	~WmiService();
public:
	IWbemServices* GetService(const wchar_t* pRoot = L"ROOT\\CIMV2");
private:
	void Clear();
private:
	IWbemLocator* pLoc_;
	IWbemServices* pSvc_;
};
class WmicQuery
{
public:
	WmicQuery();
	~WmicQuery();
public:
	IEnumWbemClassObject* ExecQuery(const wchar_t* pQuery, const wchar_t* pRoot = L"ROOT\\CIMV2");
	void ClearResult();
private:
	WmiService service_;
public:
	IEnumWbemClassObject* pEnum_;
};

typedef struct _CAPITAL_HARDWARE
{
	std::wstring hardType;
	std::wstring strName;
	std::wstring strModel;
	std::wstring strManufacturer;
	std::wstring strSerialNumber;
	std::map<std::wstring, std::wstring> mapProporties;

	std::wstring strDeviceInstanceName;
}CAPITAL_HARDWARE;

//获取显示器序列号
bool DetectMoniter(std::vector<CAPITAL_HARDWARE>& moniter_arr);