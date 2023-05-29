#ifndef CPING_H
#define CPING_H

#include <windows.h>
#include <conio.h>
#include <winnt.h>
#include <string>

#define PING_TIMES 2 //ping ´ÎÊý

typedef struct _IPINFO
{
	unsigned char Ttl;				// Time To Live
	unsigned char Tos;				// Type Of Service
	unsigned char IPFlags;			// IP flags
	unsigned char OptSize;			// Size of options data
	unsigned char FAR* Options;		// Options data buffer
}IPINFO;

typedef IPINFO* PIPINFO;

typedef struct _ICMPECHO
{
	unsigned long Source;			// Source address
	unsigned long Status;			// IP status
	unsigned long RTTime;			// Round trip time in milliseconds
	unsigned long DataSize;			// Reply data size
	unsigned long Reserved;			// Unknown
	void FAR* pData;				// Reply data buffer
	IPINFO	ipInfo;					// Reply options
}ICMPECHO;

typedef ICMPECHO* PICMPECHO;

class CPing
{
public:
	CPing();
	~CPing();
	int Ping(char* strHost);
	int Ping(std::string strHost);
private:
	// ICMP.DLL Export Function Pointers
	HANDLE(WINAPI* pIcmpCreateFile)(VOID);
	BOOL(WINAPI* pIcmpCloseHandle)(HANDLE);
	DWORD(WINAPI* pIcmpSendEcho) (HANDLE, DWORD, LPVOID, WORD, PIPINFO, LPVOID, DWORD, DWORD);
	HANDLE hndlIcmp; // LoadLibrary() handle to ICMP.DLL
	BOOL bValid; // if it doesn't construct properly, it won't be valid
};

#endif