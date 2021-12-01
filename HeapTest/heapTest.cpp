#include <windows.h>
#include <iostream>
#include <intsafe.h>

using namespace std;
const char* GetHeapTypeString(HANDLE pHandle)
{
	ULONG ulHeapInfo;
	HeapQueryInformation(pHandle,
		HeapCompatibilityInformation,
		&ulHeapInfo,
		sizeof(ulHeapInfo),
		NULL);
	switch (ulHeapInfo)
	{
	case 0:
		return "Standard";
	case 1:
		return "Look Aside List";
	case 2:
		return "Low Fragmentation";
	}
	return "Unknow type";
}

void PrintAllHeaps()
{

	DWORD dwNumHeap = GetProcessHeaps(0, NULL);
	if (dwNumHeap == 0)
	{
		cout << "No Heap!" << endl;
		return;
	}

	PHANDLE pHeaps;
	SIZE_T  uBytes;
	HRESULT Result = SIZETMult(dwNumHeap, sizeof(*pHeaps), &uBytes);
	if (Result != S_OK) {
		return;
	}

	pHeaps = (PHANDLE)malloc(uBytes);
	dwNumHeap = GetProcessHeaps(dwNumHeap, pHeaps);
	cout << "Process has heaps: " << dwNumHeap << endl;
	for (int i = 0; i < dwNumHeap; ++i)
	{
		cout << "Heap Address: " << pHeaps[i]
			<< ", Heap Type: " << GetHeapTypeString(pHeaps[i]) << endl;
	}

	return;
}

void HeapCorruptionFunction()
{
	char* pStr1 = new char[5];
	char* pStr2 = new char[5];
	printf("%p %p\n", pStr1, pStr2);
	strcpy(pStr1, "This is a heap corruption test");
	delete[]pStr2;
	delete[]pStr1;
}


int main()
{
	/*cout << "========================" << endl;
	PrintAllHeaps();
	cout << "========================" << endl;

	HANDLE hDefaultHeap = GetProcessHeap();
	cout << "Default Heap: " << hDefaultHeap
		<< ", Heap Type: " << GetHeapTypeString(hDefaultHeap) << endl;*/

	getchar();
	HeapCorruptionFunction();

	return 0;
}
