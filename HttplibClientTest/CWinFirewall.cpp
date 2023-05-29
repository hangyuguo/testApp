#include "CWinFirewall.h"
#include <iostream>
using namespace std;

HRESULT CWinFirewall::WindowsFirewallAppIsEnabled(
	IN INetFwProfile* fwProfile,
	IN const wchar_t* fwProcessImageFileName,
	OUT BOOL* fwAppEnabled
)
{
	HRESULT hr = S_OK;
	BSTR fwBstrProcessImageFileName = NULL;
	VARIANT_BOOL fwEnabled;
	INetFwAuthorizedApplication* fwApp = NULL;
	INetFwAuthorizedApplications* fwApps = NULL;

	_ASSERT(fwProfile != NULL);
	_ASSERT(fwProcessImageFileName != NULL);
	_ASSERT(fwAppEnabled != NULL);

	*fwAppEnabled = FALSE;

	// Retrieve the authorized application collection.
	hr = fwProfile->get_AuthorizedApplications(&fwApps);
	if (FAILED(hr))
	{
		printf("get_AuthorizedApplications failed: 0x%08lx\n", hr);
		goto error;
	}

	// Allocate a BSTR for the process image file name.
	fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
	if (fwBstrProcessImageFileName == NULL)
	{
		hr = E_OUTOFMEMORY;
		printf("SysAllocString failed: 0x%08lx\n", hr);
		goto error;
	}

	// Attempt to retrieve the authorized application.
	hr = fwApps->Item(fwBstrProcessImageFileName, &fwApp);
	if (SUCCEEDED(hr))
	{
		// Find out if the authorized application is enabled.
		hr = fwApp->get_Enabled(&fwEnabled);

		BSTR name;
		hr = fwApp->get_Name(&name);

		BSTR imageFileName;
		hr = fwApp->get_ProcessImageFileName(&imageFileName);

		if (FAILED(hr))
		{
			printf("get_Enabled failed: 0x%08lx\n", hr);
			goto error;
		}

		if (fwEnabled != VARIANT_FALSE)
		{
			// The authorized application is enabled.
			*fwAppEnabled = TRUE;

			printf(
				"Authorized application %lS is enabled in the firewall.\n",
				fwProcessImageFileName
			);
		}
		else
		{
			printf(
				"Authorized application %lS is disabled in the firewall.\n",
				fwProcessImageFileName
			);
		}
	}
	else
	{
		// The authorized application was not in the collection.
		hr = S_OK;

		printf(
			"Authorized application %lS is disabled in the firewall.\n",
			fwProcessImageFileName
		);
	}

error:

	// Free the BSTR.
	SysFreeString(fwBstrProcessImageFileName);

	// Release the authorized application instance.
	if (fwApp != NULL)
	{
		fwApp->Release();
	}

	// Release the authorized application collection.
	if (fwApps != NULL)
	{
		fwApps->Release();
	}

	return hr;
}

HRESULT CWinFirewall::WindowsFirewallInitialize(OUT INetFwProfile** fwProfile)
{
	HRESULT hr = S_OK;
	INetFwMgr* fwMgr = NULL;
	INetFwPolicy* fwPolicy = NULL;

	_ASSERT(fwProfile != NULL);

	*fwProfile = NULL;

	// Create an instance of the firewall settings manager.
	hr = CoCreateInstance(
		__uuidof(NetFwMgr),
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(INetFwMgr),
		(void**)&fwMgr
	);
	if (FAILED(hr))
	{
		printf("CoCreateInstance failed: 0x%08lx\n", hr);
		goto error;
	}

	// Retrieve the local firewall policy.
	hr = fwMgr->get_LocalPolicy(&fwPolicy);
	if (FAILED(hr))
	{
		printf("get_LocalPolicy failed: 0x%08lx\n", hr);
		goto error;
	}

	// Retrieve the firewall profile currently in effect.
	hr = fwPolicy->get_CurrentProfile(fwProfile);
	if (FAILED(hr))
	{
		printf("get_CurrentProfile failed: 0x%08lx\n", hr);
		goto error;
	}

error:
	// Release the local firewall policy.
	if (fwPolicy != NULL)
	{
		fwPolicy->Release();
	}

	// Release the firewall settings manager.
	if (fwMgr != NULL)
	{
		fwMgr->Release();
	}
	return hr;

}


void CWinFirewall::WindowsFirewallCleanup(IN INetFwProfile* fwProfile)
{
	// Release the firewall profile.
	if (fwProfile != NULL)
	{
		fwProfile->Release();
	}
}

HRESULT CWinFirewall::WindwsFirewallRemove(
	IN INetFwProfile* fwProfile,
	IN const wchar_t* fwProcessImageFileName,
	IN const wchar_t* fwName
)
{
	HRESULT hr = S_OK;
	BOOL fwAppEnabled;
	BSTR fwBstrProcessImageFileName = NULL;
	INetFwAuthorizedApplications* fwApps = NULL;

	_ASSERT(fwProfile != NULL);
	_ASSERT(fwProcessImageFileName != NULL);
	_ASSERT(fwName != NULL);

	// First check to see if the application is already authorized.
	hr = WindowsFirewallAppIsEnabled(
		fwProfile,
		fwProcessImageFileName,
		&fwAppEnabled
	);
	if (FAILED(hr))
	{
		printf("WindowsFirewallAppIsEnabled failed: 0x%08lx\n", hr);
		//goto error;
	}
	// Only add the application if it isn't already authorized.
	if (fwAppEnabled)
	{
		// Retrieve the authorized application collection.
		hr = fwProfile->get_AuthorizedApplications(&fwApps);
		if (FAILED(hr))
		{
			printf("get_AuthorizedApplications failed: 0x%08lx\n", hr);
			goto error;
		}

		fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
		if (fwBstrProcessImageFileName == NULL)
		{
			hr = E_OUTOFMEMORY;
			printf("SysAllocString failed: 0x%08lx\n", hr);
			goto error;
		}

		fwApps->Remove(fwBstrProcessImageFileName);
		if (FAILED(hr))
		{
			printf("Add failed: 0x%08lx\n", hr);
			goto error;
		}
	}

error:

	// Free the BSTRs.
	SysFreeString(fwBstrProcessImageFileName);
	// Release the authorized application collection.
	if (fwApps != NULL)
	{
		fwApps->Release();
	}

	return hr;
}

HRESULT CWinFirewall::WindowsFirewallAddApp(
	IN INetFwProfile* fwProfile,
	IN const wchar_t* fwProcessImageFileName,
	IN const wchar_t* fwName
)
{
	HRESULT hr = S_OK;
	BOOL fwAppEnabled;
	BSTR fwBstrName = NULL;
	BSTR fwBstrProcessImageFileName = NULL;
	INetFwAuthorizedApplication* fwApp = NULL;
	INetFwAuthorizedApplications* fwApps = NULL;

	_ASSERT(fwProfile != NULL);
	_ASSERT(fwProcessImageFileName != NULL);
	_ASSERT(fwName != NULL);

	// First check to see if the application is already authorized.
	hr = WindowsFirewallAppIsEnabled(
		fwProfile,
		fwProcessImageFileName,
		&fwAppEnabled
	);
	if (FAILED(hr))
	{
		printf("WindowsFirewallAppIsEnabled failed: 0x%08lx\n", hr);
		goto error;
	}

	// Only add the application if it isn't already authorized.
	if (!fwAppEnabled)
	{
		// Retrieve the authorized application collection.
		hr = fwProfile->get_AuthorizedApplications(&fwApps);
		if (FAILED(hr))
		{
			printf("get_AuthorizedApplications failed: 0x%08lx\n", hr);
			goto error;
		}

		// Create an instance of an authorized application.
		hr = CoCreateInstance(
			__uuidof(NetFwAuthorizedApplication),
			NULL,
			CLSCTX_INPROC_SERVER,
			__uuidof(INetFwAuthorizedApplication),
			(void**)&fwApp
		);
		if (FAILED(hr))
		{
			printf("CoCreateInstance failed: 0x%08lx\n", hr);
			goto error;
		}

		// Allocate a BSTR for the process image file name.
		fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
		if (fwBstrProcessImageFileName == NULL)
		{
			hr = E_OUTOFMEMORY;
			printf("SysAllocString failed: 0x%08lx\n", hr);
			goto error;
		}

		// Set the process image file name.
		hr = fwApp->put_ProcessImageFileName(fwBstrProcessImageFileName);
		if (FAILED(hr))
		{
			printf("put_ProcessImageFileName failed: 0x%08lx\n", hr);
			goto error;
		}

		// Allocate a BSTR for the application friendly name.
		fwBstrName = SysAllocString(fwName);
		if (SysStringLen(fwBstrName) == 0)
		{
			hr = E_OUTOFMEMORY;
			printf("SysAllocString failed: 0x%08lx\n", hr);
			goto error;
		}

		// Set the application friendly name.
		hr = fwApp->put_Name(fwBstrName);

		if (FAILED(hr))
		{
			printf("put_Name failed: 0x%08lx\n", hr);
			goto error;
		}

		// Add the application to the collection.
		hr = fwApps->Add(fwApp);
		if (FAILED(hr))
		{
			printf("Add failed: 0x%08lx\n", hr);
			goto error;
		}

		printf(
			"Authorized application %lS is now enabled in the firewall.\n",
			fwProcessImageFileName
		);
	}

error:

	// Free the BSTRs.
	SysFreeString(fwBstrName);
	SysFreeString(fwBstrProcessImageFileName);

	// Release the authorized application instance.
	if (fwApp != NULL)
	{
		fwApp->Release();
	}

	// Release the authorized application collection.
	if (fwApps != NULL)
	{
		fwApps->Release();
	}

	return hr;
}



void CWinFirewall::Check(HRESULT hr, const char* str, INetFwProfile* fwProfile, HRESULT comInit)
{
	if (FAILED(hr))
	{
		printf("%s: 0x%08lx\n", str, hr);
		//LOG_ERROR("%s: 0x%08lx\n", str, hr);
		cout << "str" << hr << endl;
		WindowsFirewallCleanup(fwProfile);
		if (SUCCEEDED(comInit))
		{
			CoUninitialize();
		}
	}
	else {
		//LOG_INFO("%s: 0x%08lx\n", "success", hr);
		cout << "success" << hr << endl;
	}
}


void CWinFirewall::AddFirewall(std::wstring filePath, std::wstring fileName)
{
	//LOG_INFO(L"AddFirewall filePath:%s, fileName:%s", filePath.c_str(), fileName.c_str());
	wcout << "AddFirewall filePath:" << filePath << ",fileName:" << fileName << endl;
	HRESULT hr = S_OK;
	HRESULT comInit = E_FAIL;
	INetFwProfile* fwProfile = NULL;

	// Initialize COM.
	comInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (comInit != RPC_E_CHANGED_MODE)
	{
		hr = comInit;
		Check(hr, "CoInitializeEx failed", fwProfile, comInit);
	}

	// 初始化防火墙文件
	hr = WindowsFirewallInitialize(&fwProfile);
	Check(hr, "WindowsFirewallInitialize  failed", fwProfile, comInit);

	// 添加到授权应用程序集合
	hr = WindowsFirewallAddApp(
		fwProfile,
		filePath.c_str(),
		fileName.c_str()
	);
	Check(hr, "WindowsFirewallAddApp failed", fwProfile, comInit);

	// Release the firewall profile.
	WindowsFirewallCleanup(fwProfile);
	// Uninitialize COM.
	if (SUCCEEDED(comInit))
	{
		CoUninitialize();
	}
	return;

}