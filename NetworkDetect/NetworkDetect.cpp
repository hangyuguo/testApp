// NetworkDetect.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "netlistmgr.h"

void networkDetect();
int detectNetwork2();

class netInfo;

int main()
{
    std::cout << "Hello World!\n";
	while(1){
		//networkDetect();
		detectNetwork2();
		Sleep(2000);
	}
	
}


void networkDetect() {
	// 通过NLA接口获取网络状态
	HRESULT Result = CoInitialize(NULL);
	IUnknown* pUnknown = NULL;
	Result = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_IUnknown, (void**)&pUnknown);
	if (SUCCEEDED(Result)) {
		INetworkListManager* pNetworkListManager = NULL;
		Result = pUnknown->QueryInterface(IID_INetworkListManager, (void**)&pNetworkListManager);
		if (SUCCEEDED(Result)) {
			VARIANT_BOOL IsConnect = VARIANT_FALSE;
			Result = pNetworkListManager->get_IsConnectedToInternet(&IsConnect);

			NLM_CONNECTIVITY pConnect = NLM_CONNECTIVITY_DISCONNECTED;
			Result = pNetworkListManager->GetConnectivity(&pConnect);

			std::cout << "GetConnectivity result:" << Result << std::endl;

			pNetworkListManager->Release();
		}
		pUnknown->Release();
	}
	CoUninitialize();
}


class CNetworkListManagerEvent : public INetworkListManagerEvents
{
public:
	CNetworkListManagerEvent() : m_ref(1)
	{

	}

	~CNetworkListManagerEvent()
	{

	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
	{
		HRESULT Result = S_OK;
		if (IsEqualIID(riid, IID_IUnknown))
		{
			*ppvObject = (IUnknown*)this;
		}
		else if (IsEqualIID(riid, IID_INetworkListManagerEvents))
		{
			*ppvObject = (INetworkListManagerEvents*)this;
		}
		else
		{
			Result = E_NOINTERFACE;
		}

		return Result;
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{
		return (ULONG)InterlockedIncrement(&m_ref);
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		LONG Result = InterlockedDecrement(&m_ref);
		if (Result == 0)
			delete this;
		return (ULONG)Result;
	}

	virtual HRESULT STDMETHODCALLTYPE ConnectivityChanged(
		/* [in] */ NLM_CONNECTIVITY newConnectivity)
	{
		return S_OK;
	}

private:

	LONG m_ref;
};

int detectNetwork2()
{
	CoInitialize(NULL);

	//
	//  通过NLA接口获取网络状态
	//
	IUnknown* pUnknown = NULL;

	HRESULT Result = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_IUnknown, (void**)&pUnknown);
	if (SUCCEEDED(Result))
	{
		INetworkListManager* pNetworkListManager = NULL;
		Result = pUnknown->QueryInterface(IID_INetworkListManager, (void**)&pNetworkListManager);
		if (SUCCEEDED(Result))
		{
			VARIANT_BOOL IsConnect = VARIANT_FALSE;
			Result = pNetworkListManager->get_IsConnectedToInternet(&IsConnect);
			if (SUCCEEDED(Result))
			{
				printf("IsConnect Result %s\n", IsConnect == VARIANT_TRUE ? "TRUE" : "FALSE");
			}

			IConnectionPointContainer* pCPContainer = NULL;
			Result = pNetworkListManager->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPContainer);
			if (SUCCEEDED(Result))
			{
				IConnectionPoint* pConnectPoint = NULL;
				Result = pCPContainer->FindConnectionPoint(IID_INetworkListManagerEvents, &pConnectPoint);
				if (SUCCEEDED(Result))
				{
					DWORD Cookie = NULL;
					CNetworkListManagerEvent* NetEvent = new CNetworkListManagerEvent;
					Result = pConnectPoint->Advise((IUnknown*)NetEvent, &Cookie);
					if (SUCCEEDED(Result))
					{
						printf("Loop Message\n");
						MSG msg;
						while (GetMessage(&msg, NULL, 0, 0))
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);

							if (msg.message == WM_QUIT)
							{
								break;
							}
						}

						pConnectPoint->Unadvise(Cookie);

						pConnectPoint->Release();
					}
				}

				pCPContainer->Release();
			}

			pNetworkListManager->Release();
		}

		pUnknown->Release();
	}

	CoUninitialize();
	return 1;
}

//-------------------------------
#include <Windows.h>
#include <netlistmgr.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <functional>
#include <vector>

namespace
{
	class NetworkListManagerEvent : public INetworkListManagerEvents
	{
	public:
		NetworkListManagerEvent(std::function<void(bool)> netChangedHandler) : m_ref(1), netChangedHandler_(netChangedHandler)
		{

		}
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
		{
			HRESULT Result = S_OK;
			if (IsEqualIID(riid, IID_IUnknown))
			{
				*ppvObject = (IUnknown*)this;
			}
			else if (IsEqualIID(riid, IID_INetworkListManagerEvents))
			{
				*ppvObject = (INetworkListManagerEvents*)this;
			}
			else
			{
				Result = E_NOINTERFACE;
			}
			return Result;
		}

		ULONG STDMETHODCALLTYPE AddRef()
		{
			return (ULONG)InterlockedIncrement(&m_ref);
		}

		ULONG STDMETHODCALLTYPE Release()
		{
			LONG Result = InterlockedDecrement(&m_ref);
			if (Result == 0)
			{
				delete this;
			}
			return (ULONG)Result;
		}

		virtual HRESULT STDMETHODCALLTYPE ConnectivityChanged(NLM_CONNECTIVITY newConnectivity)
		{
			if (newConnectivity == NLM_CONNECTIVITY_DISCONNECTED)
			{
				netChangedHandler_(false);
			}
			else if ((newConnectivity & NLM_CONNECTIVITY_IPV4_INTERNET) || (newConnectivity & NLM_CONNECTIVITY_IPV6_INTERNET))
			{
				netChangedHandler_(true);
			}
			return S_OK;
		}
	private:
		LONG m_ref;
		std::function<void(bool)> netChangedHandler_;
	};
	class NetInfoManager
	{
	public:
		NetInfoManager()
		{
			CoInitialize(NULL);
			HRESULT ret = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_IUnknown, (void**)&pUnknown_);
			if (FAILED(ret))
			{
				return;
			}
			ret = pUnknown_->QueryInterface(IID_INetworkListManager, (void**)&pNetworkListManager_);
			if (FAILED(ret))
			{
				return;
			}
			__StartListenNetEvent();
		}

		~NetInfoManager()
		{
			__StopListenNetEvent();
			pNetworkListManager_->Release();
			pUnknown_->Release();
			CoUninitialize();
		}

		bool IsConnected()
		{
			bool isConnected = false;
			VARIANT_BOOL isConnect = VARIANT_FALSE;
			HRESULT ret = pNetworkListManager_->get_IsConnected(&isConnect);
			if (SUCCEEDED(ret))
			{
				isConnected = (VARIANT_TRUE == isConnect);
			}
			return isConnected;
		}

		bool IsInternetConnected()
		{
			bool isConnected = false;
			VARIANT_BOOL isConnect = VARIANT_FALSE;
			HRESULT ret = pNetworkListManager_->get_IsConnectedToInternet(&isConnect);
			if (SUCCEEDED(ret))
			{
				isConnected = (VARIANT_TRUE == isConnect);
			}
			return isConnected;
		}

		void AddListener(std::function<void(bool)> handler)
		{
			std::lock_guard<std::mutex> lk(listenerMutex);
			eventHandlers_.push_back(handler);
		}

		static NetInfoManager* GetInstance()
		{
			static NetInfoManager inst;
			return &inst;
		}

		bool __StartListenNetEvent()
		{
			if (netEventThread_)
			{
				return true;
			}
			netEventThread_ = std::make_unique<std::thread>(std::thread([=]()
			{
				CoInitialize(NULL);
				IUnknown* pUnknown = nullptr;
				INetworkListManager* pNetworkListManager = nullptr;
				HRESULT ret = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_IUnknown, (void**)&pUnknown);
				if (FAILED(ret))
				{
					return;
				}
				ret = pUnknown->QueryInterface(IID_INetworkListManager, (void**)&pNetworkListManager);
				if (FAILED(ret))
				{
					return;
				}
				IConnectionPointContainer* pCPContainer = nullptr;
				ret = pNetworkListManager->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPContainer);
				if (FAILED(ret))
				{
					return;
				}
				IConnectionPoint* pConnectPoint = nullptr;
				ret = pCPContainer->FindConnectionPoint(IID_INetworkListManagerEvents, &pConnectPoint);
				if (FAILED(ret))
				{
					return;
				}

				DWORD cookie = 0;
				NetworkListManagerEvent* netEvent = new NetworkListManagerEvent([=](bool connected) {
					std::lock_guard<std::mutex> lk(listenerMutex);
					for (const auto& hander : eventHandlers_)
					{
						hander(connected);
					}
				});
				ret = pConnectPoint->Advise((IUnknown*)netEvent, &cookie);
				if (FAILED(ret))
				{
					return;
				}
				MSG msg;
				int nRet;
				while (nRet = ::GetMessage(&msg, 0, 0, 0) != 0)
				{
					if (-1 == nRet)
					{
						break;
					}
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				pConnectPoint->Unadvise(cookie);
				pConnectPoint->Release();
				pCPContainer->Release();
				pNetworkListManager->Release();
				pUnknown->Release();
				CoUninitialize();
			}));
			return true;
		}

		void __StopListenNetEvent()
		{
			if (netEventThread_)
			{
				int tryQuitCount = 3;
				auto netEventThreadId = GetThreadId(netEventThread_->native_handle());
				while (tryQuitCount--)
				{
					::PostThreadMessage(netEventThreadId, WM_QUIT, 0, 0);
					DWORD ret = ::WaitForSingleObject(netEventThread_->native_handle(), 1000);
					if (WAIT_TIMEOUT != ::WaitForSingleObject(netEventThread_->native_handle(), 1000))
					{
						break;
					}
				}
				if (netEventThread_->joinable())
				{
					netEventThread_->join();
				}
				std::cout << "stop." << std::endl;
			}
		}
	private:
		std::mutex listenerMutex;
		static NetInfoManager* inst_;
		std::vector<std::function<void(bool)>> eventHandlers_;
		IUnknown* pUnknown_ = nullptr;
		INetworkListManager* pNetworkListManager_ = nullptr;
		std::unique_ptr<std::thread> netEventThread_;
	};
}
NetInfoManager* NetInfoManager::inst_ = nullptr;

class netInfo {
	netInfo() = default;
	~netInfo() = default;
public:
	bool IsNetConnected()
	{
		return NetInfoManager::GetInstance()->IsConnected();
		return true;
	}

	bool IsInternetConnected()
	{
		return NetInfoManager::GetInstance()->IsInternetConnected();
		return true;
	}

	void ListenNetEvent(std::function<void(bool)> eventHandler)
	{
		NetInfoManager::GetInstance()->AddListener(eventHandler);
	}
};
