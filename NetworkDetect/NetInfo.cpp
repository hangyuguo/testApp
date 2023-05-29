#include "NetInfo.h"
#include <winerror.h>


DWORD _cookie;
CComPtr<INetworkListManager> _pNLM;
CComPtr<IConnectionPointContainer> _pCpc;
CComPtr<IConnectionPoint> _pConnectionPoint;

static std::wstring GUIDToString(const GUID& guid)
{
    OLECHAR guidString[40] = { 0 };
    ::StringFromGUID2(guid, guidString, _countof(guidString));
    return guidString;
}

static std::string UnicodeToUTF8(const std::wstring& wstr)
{
    std::string ret;
    try
    {
        std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
        ret = wcv.to_bytes(wstr);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return ret;
}



void init() {
    CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);

    HRESULT hr = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_INetworkListManager, (LPVOID*)&_pNLM);
    hr = _pNLM->QueryInterface(IID_IConnectionPointContainer, (void**)&_pCpc);
    hr = _pCpc->FindConnectionPoint(IID_INetworkConnectionEvents, &_pConnectionPoint);
}

NetConnectAdaptInfo GetNetAdpaterInfo(const std::wstring& guid)
{
    unsigned long unSize = sizeof(IP_ADAPTER_INFO);
    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(unSize);
    bool find = false;
    unsigned long unResult = GetAdaptersInfo(reinterpret_cast<PIP_ADAPTER_INFO>(data.get()), &unSize);

    if (ERROR_BUFFER_OVERFLOW == unResult)
    {
        data = std::make_unique<uint8_t[]>(unSize);
        unResult = GetAdaptersInfo(reinterpret_cast<PIP_ADAPTER_INFO>(data.get()), &unSize);
    }
    if (ERROR_SUCCESS == unResult)
    {
        PIP_ADAPTER_INFO  pIpAdapterInfo = reinterpret_cast<PIP_ADAPTER_INFO>(data.get());

        //LOG_INFO(L"NetworkType::guid [%s]", guid.c_str());

        while (pIpAdapterInfo)
        {
            //LOG_INFO("pIpAdapterInfo->AdapterName [%s]", pIpAdapterInfo->AdapterName);

            if (UnicodeToUTF8(guid) == pIpAdapterInfo->AdapterName)
            {
                NetworkType networkType;
                if (pIpAdapterInfo->Type == MIB_IF_TYPE_ETHERNET || pIpAdapterInfo->Type == MIB_IF_TYPE_PPP) {
                    networkType = NetworkType::Ethernet;
                }
                else if (pIpAdapterInfo->Type == IF_TYPE_IEEE80211) {
                    networkType = NetworkType::Wlan;
                }
                else {
                    networkType = NetworkType::NotNetwork;
                }

                IP_ADDR_STRING ipAddrString = pIpAdapterInfo->IpAddressList;

                // 获取当前网络连通的适配器信息
                NetConnectAdaptInfo connectNetInfo;
                char cMacAddressFormat[50] = "\0";
                BYTE* pMacAdd = pIpAdapterInfo->Address;
                sprintf_s(cMacAddressFormat, _countof(cMacAddressFormat), "%.2X-%.2X-%.2X-%.2X-%.2X-%.2X", pMacAdd[0], pMacAdd[1], pMacAdd[2], pMacAdd[3], pMacAdd[4], pMacAdd[5]);
                connectNetInfo.macAddress = cMacAddressFormat;
                connectNetInfo.guid = UnicodeToUTF8(guid);
                connectNetInfo.gatewayIp = pIpAdapterInfo->GatewayList.IpAddress.String;
                connectNetInfo.ipAddress = pIpAdapterInfo->IpAddressList.IpAddress.String;
                connectNetInfo.networkType = networkType;

                //printf("connectNetInfo ipAddress:[%s] Type:[%d]", (connectNetInfo.ipAddress).c_str(), pIpAdapterInfo->Type);

                return connectNetInfo;
            }
            pIpAdapterInfo = pIpAdapterInfo->Next;
        }
    }

    //LOG_INFO(L"NetworkType::NotNetwork");
    NetConnectAdaptInfo connectNetInfo;
    connectNetInfo.networkType = NetworkType::NotNetwork;
    return connectNetInfo;
}

std::vector<ConnectionInfo> GetNetworkConnections()
{
    std::vector<ConnectionInfo> result;

    CComPtr<IEnumNetworkConnections> enumConnectons;

    // 获取网络连接的枚举器
    if (FAILED(_pNLM->GetNetworkConnections(&enumConnectons)))
    {
        DWORD lasterror = GetLastError();
        std::cerr << "GetNetworkConnections error : " << GetLastError() << std::endl;
        //LOG_INFO(L"GetNetworkConnections error [%d]", GetLastError());
        return result;
    }

    if (enumConnectons)
    {
        ULONG lFetch;
        INetworkConnection* connection = nullptr;
        while (SUCCEEDED(enumConnectons->Next(1, &connection, &lFetch)) && nullptr != connection) // 进行枚举活动网络连接
        {
            VARIANT_BOOL isConnectInternet = VARIANT_FALSE;
            connection->get_IsConnectedToInternet(&isConnectInternet); // 过滤掉没有连上网的连接
            if (isConnectInternet == VARIANT_FALSE)
            {
                continue;
            }
            ConnectionInfo item;
            GUID guid;

            connection->GetAdapterId(&guid); // 网络适配器的GUID
            item.guid = GUIDToString(guid);

            result.push_back(item);
        }
        if (connection)
        {
            connection->Release();
        }
    }

    for (auto it = result.begin(); it != result.end(); ++it)
    {
        // 通过GUID获取网络类型
        it->info = GetNetAdpaterInfo(it->guid);
    }
    // 优先显示有线
    std::partition(result.begin(), std::partition(result.begin(), result.end(), [](const ConnectionInfo& info)
    {
        return info.info.networkType != NetworkType::NotNetwork;
    }), [](const ConnectionInfo& info)
    {
        return info.info.networkType == NetworkType::Ethernet;
    });

    std::cout << "result size:" << result.size() << std::endl;

    for (auto it = result.begin(); it != result.end(); ++it)
    {
        std::cout << "connect network ip : " << it->info.ipAddress << " | type : " << it->info.networkType << std::endl;

        std::time_t now = std::time(nullptr);
        std::tm localTime;
        localtime_s(&localTime, &now);
        // 输出时间
        std::cout << "当前时间为：" << localTime.tm_year + 1900 << "-" << localTime.tm_mon + 1 << "-" << localTime.tm_mday
            << " " << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << std::endl;
    }
    return result;
}

void ShowNetworkInfo()
{
    NetworkType type;
    auto connections = GetNetworkConnections();
    if (connections.empty())
    {
        //LOG_INFO(L"connections size is empty");
        // 网络断开，将之前设置的清空
        NetConnectAdaptInfo connectNetInfo;
        //NetworkModuleController::GetInstance()->SetNetInfo(connectNetInfo);
        type = NetworkType::NotNetwork;
    }
    else {
        type = connections.front().info.networkType;

        //LOG_INFO(L"connections size is not empty : type [%d]", (int)type);
    }
}
