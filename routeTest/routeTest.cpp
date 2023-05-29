#include <iostream>
#include <WinSock2.h>
#pragma comment (lib,"ws2_32.lib")

#include <Windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

using namespace std;

PMIB_IPFORWARDTABLE MyGetIpForwardTable(BOOL bOrder);
void PrintIpForwardTable();

void main() {
    PrintIpForwardTable();
    system("pause");
}


PMIB_IPFORWARDTABLE MyGetIpForwardTable(BOOL bOrder)
{
    PMIB_IPFORWARDTABLE pIpRouteTab = NULL;
    DWORD dwActualSize = 0;

    // 查询所需缓冲区的大小
    if (::GetIpForwardTable(pIpRouteTab, &dwActualSize, bOrder) == ERROR_INSUFFICIENT_BUFFER)
    {
        // 为MIB_IPFORWARDTABLE结构申请内存
        pIpRouteTab = (PMIB_IPFORWARDTABLE)::GlobalAlloc(GPTR, dwActualSize);
        // 获取路由表
        if (::GetIpForwardTable(pIpRouteTab, &dwActualSize, bOrder) == NO_ERROR)
            return pIpRouteTab;
        ::GlobalFree(pIpRouteTab);
    }
    return NULL;
}
void PrintIpForwardTable()
{
    PMIB_IPFORWARDTABLE pIpRouteTable = MyGetIpForwardTable(TRUE);
    if (pIpRouteTable != NULL)
    {
        DWORD i, dwCurrIndex;
        struct in_addr inadDest;
        struct in_addr inadMask;
        struct in_addr inadGateway;
        PMIB_IPADDRTABLE pIpAddrTable = NULL;
        IF_INDEX dwForwardIfIndex;

        char szDestIp[128];
        char szMaskIp[128];
        char szGatewayIp[128];

        printf("Active Routes:\n\n");

        printf("  Network Address          Netmask  Gateway Address        Interface  Metric  IfIndex\n");
        for (i = 0; i < pIpRouteTable->dwNumEntries; i++)
        {
            dwCurrIndex = pIpRouteTable->table[i].dwForwardIfIndex;

            // 目的地址
            inadDest.s_addr = pIpRouteTable->table[i].dwForwardDest;
            // 子网掩码
            inadMask.s_addr = pIpRouteTable->table[i].dwForwardMask;
            // 网关地址
            inadGateway.s_addr = pIpRouteTable->table[i].dwForwardNextHop;
            // ifindex
            dwForwardIfIndex = pIpRouteTable->table[i].dwForwardIfIndex;

            strcpy_s(szDestIp, inet_ntoa(inadDest));
            strcpy_s(szMaskIp, inet_ntoa(inadMask));
            strcpy_s(szGatewayIp, inet_ntoa(inadGateway));
            printf("  %15s %16s %16s %16d %7d\n %7d",
                szDestIp,
                szMaskIp,
                szGatewayIp,
                pIpRouteTable->table[i].dwForwardIfIndex,    // 可以在此调用GetIpAddrTable获取索引对应的IP地址
                pIpRouteTable->table[i].dwForwardMetric1),
                dwForwardIfIndex;
        }
        //MyFreeIpForwardTable(pIpRouteTable);
    }
}