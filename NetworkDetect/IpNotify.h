#pragma once

#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <windows.h>

#include "NetInfo.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

//通知ip变化
void IPChangeNotify()
{
	init();

	OVERLAPPED overlap;
	DWORD ret;
	HANDLE hand = NULL;

	overlap.hEvent = WSACreateEvent();

	while (true) {
		ret = NotifyAddrChange(&hand, &overlap);
		if (ret != NO_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				printf("NotifyAddrChange error...%d\n", WSAGetLastError());
				return;
			}
		}

		printf("Waitting for IP Address change event..\n");
		if (WaitForSingleObject(overlap.hEvent, INFINITE) == WAIT_OBJECT_0) {
			printf("IP Address table changed..\n");
			GetNetworkConnections();
			ResetEvent(overlap.hEvent);
		}
		Sleep(500);
	}
		
	CloseHandle(hand);
	hand = NULL;
}