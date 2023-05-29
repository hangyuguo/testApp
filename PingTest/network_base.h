#pragma once

/******************************************************************************
*  @class     CNetworkBase
*  @file      network_base.h 2021\03\27 $
*  @author    lizheng1<lizheng1@corp.netease.com>
*  @brief     提供基础网络功能的类
******************************************************************************/
#include <Winsock2.h>
#include <Windows.h> 
#pragma comment (lib, "ws2_32.lib")

/*****************************************************************************/

// ICMP消息格式
typedef struct icmp_header
{
    unsigned char  icmp_type;			// 消息类型
    unsigned char  icmp_code;			// 代码
    unsigned short icmp_checksum;		// 校验和

    unsigned short icmp_id;				// 用来惟一标识此请求的ID号，通常设置为进程ID
	unsigned short icmp_uuid;			// 用来惟一标识此请求的ID号，通常设置为随机数（解决同时ping 不同服务器 无法区分问题）
    unsigned short icmp_sequence;		// 序列号
	unsigned long  icmp_timestamp;		// 时间戳
} ICMP_HEADER,*PICMP_HEADER;

#define ICMP_HEADER_SIZE	sizeof(ICMP_HEADER)
#define ICMP_ECHO_REQUEST	0x08
#define ICMP_ECHO_REPLY_CODE		0x00

class CInitSock
{
public:
	CInitSock(BYTE minorVer = 2, BYTE majorVer = 2)
	{
		// 初始化WS2_32.dll
		WSADATA wsaData;
		WORD sockVersion = MAKEWORD(minorVer, majorVer);
		if (::WSAStartup(sockVersion, &wsaData) != 0)
		{
			//exit(0);
		}
	}
	~CInitSock()
	{
		::WSACleanup();
	}
};

class CNetworkBase :public CInitSock
{
public:
    CNetworkBase();
    ~CNetworkBase();

	/**
	 * 该函数命令支持域名或IP地址
	 * @param[in] szDestIp ip地址
	 * @param[in] nPingCount ping的次数
	 * @return int -1/0：失败    >0：代表ping成功的次数
	 */
	int Ping(char* szDestIp, int nPingCount);

	/**
	 * 该函数命令支持域名或IP地址
	 * @param[in] szDestIp ip地址
	 * @param[in] nPingCount ping的次数
	 * @param[out] avgDelay 平均延迟
	 * @return int -1/0：失败    >0：代表ping成功的次数
	 */
	int Ping(char* szDestIp, int nPingCount, _Out_ int& avgDelay);

	// 获取本地可用端口号
	// 返回值：成功返回端口号，失败返回-1,最多尝试次数 20 次
	int GetAvailablePort();

private:
	USHORT chsum(USHORT* buffer, int bufferSize);

	// 获取本地可用端口号
	// 返回值：成功返回端口号，失败返回-1
	int GetAvailablePortInternal();
};

