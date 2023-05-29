#pragma once

/******************************************************************************
*  @class     CNetworkBase
*  @file      network_base.h 2021\03\27 $
*  @author    lizheng1<lizheng1@corp.netease.com>
*  @brief     �ṩ�������繦�ܵ���
******************************************************************************/
#include <Winsock2.h>
#include <Windows.h> 
#pragma comment (lib, "ws2_32.lib")

/*****************************************************************************/

// ICMP��Ϣ��ʽ
typedef struct icmp_header
{
    unsigned char  icmp_type;			// ��Ϣ����
    unsigned char  icmp_code;			// ����
    unsigned short icmp_checksum;		// У���

    unsigned short icmp_id;				// ����Ωһ��ʶ�������ID�ţ�ͨ������Ϊ����ID
	unsigned short icmp_uuid;			// ����Ωһ��ʶ�������ID�ţ�ͨ������Ϊ����������ͬʱping ��ͬ������ �޷��������⣩
    unsigned short icmp_sequence;		// ���к�
	unsigned long  icmp_timestamp;		// ʱ���
} ICMP_HEADER,*PICMP_HEADER;

#define ICMP_HEADER_SIZE	sizeof(ICMP_HEADER)
#define ICMP_ECHO_REQUEST	0x08
#define ICMP_ECHO_REPLY_CODE		0x00

class CInitSock
{
public:
	CInitSock(BYTE minorVer = 2, BYTE majorVer = 2)
	{
		// ��ʼ��WS2_32.dll
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
	 * �ú�������֧��������IP��ַ
	 * @param[in] szDestIp ip��ַ
	 * @param[in] nPingCount ping�Ĵ���
	 * @return int -1/0��ʧ��    >0������ping�ɹ��Ĵ���
	 */
	int Ping(char* szDestIp, int nPingCount);

	/**
	 * �ú�������֧��������IP��ַ
	 * @param[in] szDestIp ip��ַ
	 * @param[in] nPingCount ping�Ĵ���
	 * @param[out] avgDelay ƽ���ӳ�
	 * @return int -1/0��ʧ��    >0������ping�ɹ��Ĵ���
	 */
	int Ping(char* szDestIp, int nPingCount, _Out_ int& avgDelay);

	// ��ȡ���ؿ��ö˿ں�
	// ����ֵ���ɹ����ض˿ںţ�ʧ�ܷ���-1,��ೢ�Դ��� 20 ��
	int GetAvailablePort();

private:
	USHORT chsum(USHORT* buffer, int bufferSize);

	// ��ȡ���ؿ��ö˿ں�
	// ����ֵ���ɹ����ض˿ںţ�ʧ�ܷ���-1
	int GetAvailablePortInternal();
};

