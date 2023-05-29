#include "network_base.h"
#include <time.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ws2tcpip.h>

using std::string;

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


CNetworkBase::CNetworkBase()
{
    srand((unsigned)time(NULL));
}

CNetworkBase::~CNetworkBase()
{
}

// 计算校验和 
USHORT CNetworkBase::chsum(USHORT* buffer, int bufferSize)
{
    unsigned long cksum = 0;

    // 将数据以字为单位累加到cksum中
    while (bufferSize > 1)
    {
        cksum += *(buffer++);
        bufferSize -= sizeof(USHORT);
    }

    // 如果为奇数，将最后一个字节扩展到双字，再累加到cksum中
    if (bufferSize) {
        cksum += *( unsigned char* ) buffer;
    }

    // 将cksum的高16 位和低16位相加，取反后得到校验和
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);

    return ( USHORT ) ~cksum;
}

int CNetworkBase::Ping(char* szDestIp, int nPingCount)
{
    if (nPingCount <= 0) {
        return 0;
    }

    
    int respNum = 0;
    unsigned long minTime = 0, maxTime = 0, sumTime = 0;
    int nTimeOut = 500;//1s  

    // 1、创建原始套接字
    SOCKET sRaw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    // 2、设置接收超时
    //SetTimeout(sRaw,1000,TRUE);
    setsockopt(sRaw, SOL_SOCKET, SO_RCVTIMEO, ( char const* ) &nTimeOut, sizeof(nTimeOut));

    // 3、设置目的地址
    sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(0);

    // 判断是域名还是ip地址
    struct hostent* host;
    unsigned long inaddr;
    char* pPingIp;
    string pingIp = "";
    if (inaddr = inet_addr(szDestIp) == INADDR_NONE) {
        // 是域名
        if ((host = gethostbyname(szDestIp)) == NULL) {
            //printf("gethostbyname() error \n");
            if (sRaw) {
                closesocket(sRaw);
                sRaw = NULL;
            }
            return -1;
        }

        pPingIp = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
        //printf("\n正在 Ping %s [%s] 具有 32 字节的数据:\n", host->h_name, pPingIp);

        dest_addr.sin_addr.S_un.S_addr = inet_addr(pPingIp);
        pingIp = pPingIp;
    } else {
        // 是ip地址
        //printf("\n正在 Ping %s 具有 32 字节的数据:\n", szDestIp);
        pPingIp = szDestIp;
        pingIp = szDestIp;
        dest_addr.sin_addr.S_un.S_addr = inet_addr(szDestIp);
    }

    // 4、构造ICMP封包
    char icmp_data[32] = { 0 }; //填充32字节数据
    char szBuff[ICMP_HEADER_SIZE + 32] = { 0 };
    ICMP_HEADER* pIcmp = ( ICMP_HEADER* ) szBuff;
    USHORT uuid  = rand() % 1000;

    // 5、填写ICMP封包数据
    pIcmp->icmp_type = ICMP_ECHO_REQUEST;   // 请求一个ICMP回显
    pIcmp->icmp_code = 0;
    pIcmp->icmp_id = (USHORT)::GetCurrentProcessId();
    pIcmp->icmp_uuid = uuid;
    pIcmp->icmp_checksum = 0;
    pIcmp->icmp_sequence = 0;
    pIcmp->icmp_timestamp = (unsigned long)::GetTickCount64();

    // 6、填充数据，可以任意 
    memcpy((szBuff + ICMP_HEADER_SIZE), "abcdefghijklmnopqrstuvwabcdefghi", 32);

    // 7、准备接受包buffer
    char szRecvBuff[1024];
    sockaddr_in from_addr;
    int nLen = sizeof(from_addr);
    USHORT nSeq = 0;

    for (int iCount = 0; iCount < nPingCount; iCount++) {
        uuid = rand() % 1000;
        pIcmp->icmp_uuid = uuid;
        pIcmp->icmp_checksum = 0;
        pIcmp->icmp_sequence = nSeq++;
        pIcmp->icmp_timestamp = (unsigned long)::GetTickCount64();
        // 计算校验和
        pIcmp->icmp_checksum = chsum(( USHORT* ) szBuff, sizeof(szBuff));

        // 8、发送数据和接受数据
        int ret;
        ret = sendto(sRaw, szBuff, sizeof(szBuff), 0, ( SOCKADDR* ) &dest_addr, sizeof(SOCKADDR));
        if (ret == SOCKET_ERROR) {
            //printf("ret = %d ,errorCode:%d\n", ret, WSAGetLastError());
            if (sRaw) {
                closesocket(sRaw);
                sRaw = NULL;
            }
            return -1;
        }

        ret = recvfrom(sRaw, szRecvBuff, 1024, 0, ( SOCKADDR* ) &from_addr, &nLen);
        if (ret == SOCKET_ERROR) {
            if (::WSAGetLastError() == WSAETIMEDOUT) {
                //printf("time out :%d\n", WSAGetLastError());
            }

            //printf("recvfrom() failed:%d\n", WSAGetLastError());
            if (sRaw) {
                closesocket(sRaw);
                sRaw = NULL;
            }
            return -1;
        }

        struct sockaddr_in* sock = (struct sockaddr_in*)&from_addr;
        int port = ntohs(sock->sin_port);
        string recvIp = inet_ntoa(sock->sin_addr);
       
        if (recvIp != pingIp) {
            OutputDebugStringA(recvIp.c_str());
            OutputDebugStringA(pingIp.c_str());
            OutputDebugString(L"接受的报文和发送的ip不一致");
        }

        // 9、解析收到的ICMP封包
        // ip头部的第一个字节前4位 表示 ip协议版本号，后4位 表示IP 头部长度(单位为4字节)
        char ipInfo = szRecvBuff[0];

        int ipVer = ipInfo >> 4;                            // 版本号
        int ipHeadLen = (( char ) (ipInfo << 4) >> 4) * 4;  //IHL
        if (ipVer == 4) {
            //ipv4 
            // 跨过ip协议头，得到ICMP协议头的位置
            // 网络字节序 是大端模式 低地址 高位字节 高地址 低位字节。-> 转换为 本地字节序 小端模式 高地址高字节 低地址低字节   
            icmp_header* icmp_rep = ( icmp_header* ) (szRecvBuff + ipHeadLen);

            unsigned short checksum_host = icmp_rep->icmp_checksum;
            unsigned short sendSequence = icmp_rep->icmp_sequence;
            unsigned long Sendtimestap = icmp_rep->icmp_timestamp;
            unsigned long  recvTime = ( unsigned long )GetTickCount64();
            unsigned long time = recvTime - Sendtimestap;

            sumTime += time;
            if (minTime > time)
                minTime = time;
            if (maxTime < time)
                maxTime = time;

            // ipv4头部的第9个字节为TTL的值
            char ttl = szRecvBuff[8];

            if (icmp_rep->icmp_type == ICMP_ECHO_REPLY_CODE && 
                icmp_rep->icmp_id == (USHORT)::GetCurrentProcessId() &&
                sendSequence == pIcmp->icmp_sequence) { //回显应答报文 
                respNum++;
                //printf("来自 %s 的回复：字节=32 时间=%2dms TTL=%d checksum=0x%x \n", pPingIp, time, ttl, checksum_host);
            } else {
                //printf("非回显类型。type = %d\n", icmp_rep->icmp_type);
                char s[MAX_PATH];
                sprintf(s, "---> 非回显类型 send uid : %d   recv uid: %d\n", uuid, icmp_rep->icmp_uuid);
                OutputDebugStringA(s);
            }
        } else {
            // ipv6 icmpv6 和 icmpv4 不一样，要做对应的处理 
            //printf("ipv6\n");
        }

        Sleep(100);
    }

    //printf("\n%s 的 Ping 统计信息:\n", pPingIp);
    //printf("    数据包: 已发送 = %d，已接收 = %d，丢失 = %d (%d%% 丢失)，\n", nPingCount, respNum, nPingCount - respNum, (nPingCount - respNum) * 100 / nPingCount);

    //printf("往返行程的估计时间(以毫秒为单位):\n");
    //if (respNum > 0)
        //printf("    最短 = %dms，最长 = %dms，平均 = %dms\n", minTime, maxTime, sumTime / respNum);

    if (sRaw) {
        closesocket(sRaw);
        sRaw = NULL;
    }
   
    return respNum;
}

int CNetworkBase::GetAvailablePortInternal()
{
    int port = -1;
  
    int control_while_count_ = 1;
    while (control_while_count_--) {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = 0;

        struct sockaddr_in connAddr;
        int len = sizeof connAddr;

        if (::bind(sock, (SOCKADDR*)&addr, sizeof addr) != 0) {
            break;
        }

        if (getsockname(sock, (SOCKADDR*)&connAddr, &len) != 0) {
            break;
        }

        port = ntohs(connAddr.sin_port);
        if (closesocket(sock) != 0) {
            break;
        }
        break;
    }

    return port;
}

int CNetworkBase::Ping(char* szDestIp, int nPingCount, int& avgDelay)
{
    if (nPingCount <= 0) {
        return 0;
    }


    int respNum = 0;
    unsigned long minTime = 0, maxTime = 0, sumTime = 0;
    int nTimeOut = 500;//1s  

    // 1、创建原始套接字
    SOCKET sRaw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    // 2、设置接收超时
    //SetTimeout(sRaw,1000,TRUE);
    setsockopt(sRaw, SOL_SOCKET, SO_RCVTIMEO, (char const*)&nTimeOut, sizeof(nTimeOut));

    // 3、设置目的地址
    sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(0);

    // 判断是域名还是ip地址
    struct hostent* host;
    unsigned long inaddr;
    char* pPingIp;
    string pingIp = "";
    if (inaddr = inet_addr(szDestIp) == INADDR_NONE) {
        // 是域名
        if ((host = gethostbyname(szDestIp)) == NULL) {
            //printf("gethostbyname() error \n");
            if (sRaw) {
                closesocket(sRaw);
                sRaw = NULL;
            }
            return -1;
        }

        pPingIp = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
        //printf("\n正在 Ping %s [%s] 具有 32 字节的数据:\n", host->h_name, pPingIp);

        dest_addr.sin_addr.S_un.S_addr = inet_addr(pPingIp);
        pingIp = pPingIp;
    }
    else {
        // 是ip地址
        //printf("\n正在 Ping %s 具有 32 字节的数据:\n", szDestIp);
        pPingIp = szDestIp;
        pingIp = szDestIp;
        dest_addr.sin_addr.S_un.S_addr = inet_addr(szDestIp);
    }

    // 4、构造ICMP封包
    char icmp_data[32] = { 0 }; //填充32字节数据
    char szBuff[ICMP_HEADER_SIZE + 32] = { 0 };
    ICMP_HEADER* pIcmp = (ICMP_HEADER*)szBuff;
    USHORT uuid = rand() % 1000;

    // 5、填写ICMP封包数据
    pIcmp->icmp_type = ICMP_ECHO_REQUEST;   // 请求一个ICMP回显
    pIcmp->icmp_code = 0;
    pIcmp->icmp_id = (USHORT)::GetCurrentProcessId();
    pIcmp->icmp_uuid = uuid;
    pIcmp->icmp_checksum = 0;
    pIcmp->icmp_sequence = 0;
    pIcmp->icmp_timestamp = (unsigned long)::GetTickCount64();

    // 6、填充数据，可以任意 
    memcpy((szBuff + ICMP_HEADER_SIZE), "abcdefghijklmnopqrstuvwabcdefghi", 32);

    // 7、准备接受包buffer
    char szRecvBuff[1024];
    sockaddr_in from_addr;
    int nLen = sizeof(from_addr);
    USHORT nSeq = 0;

    for (int iCount = 0; iCount < nPingCount; iCount++) {
        uuid = rand() % 1000;
        pIcmp->icmp_uuid = uuid;
        pIcmp->icmp_checksum = 0;
        pIcmp->icmp_sequence = nSeq++;
        pIcmp->icmp_timestamp = (unsigned long)::GetTickCount64();
        // 计算校验和
        pIcmp->icmp_checksum = chsum((USHORT*)szBuff, sizeof(szBuff));

        // 8、发送数据和接受数据
        int ret;
        ret = sendto(sRaw, szBuff, sizeof(szBuff), 0, (SOCKADDR*)&dest_addr, sizeof(SOCKADDR));
        if (ret == SOCKET_ERROR) {
            //printf("ret = %d ,errorCode:%d\n", ret, WSAGetLastError());
            if (sRaw) {
                closesocket(sRaw);
                sRaw = NULL;
            }
            return -1;
        }

        ret = recvfrom(sRaw, szRecvBuff, 1024, 0, (SOCKADDR*)&from_addr, &nLen);
        if (ret == SOCKET_ERROR) {
            if (::WSAGetLastError() == WSAETIMEDOUT) {
                //printf("time out :%d\n", WSAGetLastError());
            }

            //printf("recvfrom() failed:%d\n", WSAGetLastError());
            if (sRaw) {
                closesocket(sRaw);
                sRaw = NULL;
            }
            return -1;
        }

        struct sockaddr_in* sock = (struct sockaddr_in*)&from_addr;
        int port = ntohs(sock->sin_port);
        string recvIp = inet_ntoa(sock->sin_addr);

        if (recvIp != pingIp) {
            OutputDebugStringA(recvIp.c_str());
            OutputDebugStringA(pingIp.c_str());
            OutputDebugString(L"接受的报文和发送的ip不一致");
        }

        // 9、解析收到的ICMP封包
        // ip头部的第一个字节前4位 表示 ip协议版本号，后4位 表示IP 头部长度(单位为4字节)
        char ipInfo = szRecvBuff[0];

        int ipVer = ipInfo >> 4;                            // 版本号
        int ipHeadLen = ((char)(ipInfo << 4) >> 4) * 4;  //IHL
        if (ipVer == 4) {
            //ipv4 
            // 跨过ip协议头，得到ICMP协议头的位置
            // 网络字节序 是大端模式 低地址 高位字节 高地址 低位字节。-> 转换为 本地字节序 小端模式 高地址高字节 低地址低字节   
            icmp_header* icmp_rep = (icmp_header*)(szRecvBuff + ipHeadLen);

            unsigned short checksum_host = icmp_rep->icmp_checksum;
            unsigned short sendSequence = icmp_rep->icmp_sequence;
            unsigned long Sendtimestap = icmp_rep->icmp_timestamp;
            unsigned long  recvTime = (unsigned long)GetTickCount64();
            unsigned long time = recvTime - Sendtimestap;

            sumTime += time;
            if (minTime > time)
                minTime = time;
            if (maxTime < time)
                maxTime = time;

            // ipv4头部的第9个字节为TTL的值
            char ttl = szRecvBuff[8];

            if (icmp_rep->icmp_type == ICMP_ECHO_REPLY_CODE &&
                icmp_rep->icmp_id == (USHORT)::GetCurrentProcessId() &&
                sendSequence == pIcmp->icmp_sequence) { //回显应答报文 
                respNum++;
                //printf("来自 %s 的回复：字节=32 时间=%2dms TTL=%d checksum=0x%x \n", pPingIp, time, ttl, checksum_host);
            }
            else {
                //printf("非回显类型。type = %d\n", icmp_rep->icmp_type);
                char s[MAX_PATH];
                sprintf(s, "---> 非回显类型 send uid : %d   recv uid: %d\n", uuid, icmp_rep->icmp_uuid);
                OutputDebugStringA(s);
            }
        }
        else {
            // ipv6 icmpv6 和 icmpv4 不一样，要做对应的处理 
            //printf("ipv6\n");
        }

        Sleep(100);
    }

    //printf("\n%s 的 Ping 统计信息:\n", pPingIp);
    //printf("    数据包: 已发送 = %d，已接收 = %d，丢失 = %d (%d%% 丢失)，\n", nPingCount, respNum, nPingCount - respNum, (nPingCount - respNum) * 100 / nPingCount);

    //printf("往返行程的估计时间(以毫秒为单位):\n");
    //if (respNum > 0)
        //printf("    最短 = %dms，最长 = %dms，平均 = %dms\n", minTime, maxTime, sumTime / respNum);

    if (sRaw) {
        closesocket(sRaw);
        sRaw = NULL;
    }

    avgDelay = sumTime / respNum;

    return respNum;
}

int CNetworkBase::GetAvailablePort()
{
    const int kMaxTryCount = 20;
    for (unsigned int i = 0; i < kMaxTryCount; i++) {
        int port = GetAvailablePortInternal();
        if (port != -1) {
            return port;
        }
    }

    return -1;
}