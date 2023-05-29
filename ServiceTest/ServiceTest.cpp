#include<stdio.h>
#include<Windows.h>
SERVICE_STATUS ServiceStatus;  //服务状态码
SERVICE_STATUS_HANDLE hStatus;//服务句柄
SC_HANDLE SC_Manager;
SC_HANDLE SC_Service;
void WorkThing();//服务主要工作的事情
void UninstallService();//定义卸载服务的函数
void StopService();//定义停止服务的函数
void InstallService();//定义安装服务的函数
void StartDestService();//启动服务
VOID WINAPI ServiceHandler(DWORD dwControl);
void WINAPI ServiceMain(DWORD Args, LPSTR* lpSzArgv);
int main()
{
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = (LPWSTR)L"Test";//设置服务名称
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;//设置服务对应的函数
	ServiceTable[1].lpServiceName = NULL;//服务表的最后一项必须设置为空，表示当前服务表结束
	ServiceTable[1].lpServiceProc = NULL;
	StartServiceCtrlDispatcher(ServiceTable);//启动控制分派机制
	return 0;
}
void WINAPI ServiceMain(DWORD Args, LPSTR* lpSzArgv)
{
	//初始化服务状态
	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;//服务类型
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;//设置目前服务的状态：正在初始化
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;//设置当前服务接收的请求
	ServiceStatus.dwWin32ExitCode = 0;//服务用于报告启动或停止时发生错误的错误代码
	ServiceStatus.dwCheckPoint = 0;//服务在长时间启动、停止、暂停或继续操作期间定期递增以报告其进度的检查点值
	ServiceStatus.dwServiceType = 0;//服务类型
	ServiceStatus.dwWaitHint = 0;//将要进行 开始、停止、暂停或继续服务 操作所需的估计时间 (以毫秒为单位)
	//初始化结束之后，开始第二步操作，开始注册Handler函数(注册服务控制请求函数)
	hStatus = RegisterServiceCtrlHandler(L"Demo", (LPHANDLER_FUNCTION)ServiceHandler);//注册服务控制请求函数
	if (!hStatus)
	{
		printf("注册服务控制请求函数失败");
		system("Pause");
		return;
	}
	SetServiceStatus(hStatus, &ServiceStatus);//向SCM报告服务状态
	//是否设置成功
	if (GetLastError != NO_ERROR)
	{
		//如果失败，重新设置状态为停止
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwWaitHint = 0;
		SetServiceStatus(hStatus, &ServiceStatus);
		printf("设置服务状态失败");
		system("pause");
		return;
	}
	//设置成功之后继续运行，这个时候需要通知SCM将服务状态设置为Running状态
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;
	SetServiceStatus(hStatus, &ServiceStatus);//服务状态变更之后要重新设置  以通知SCM
	//启动服务
	StartDestService();
	//接下来开始执行服务程序的主要操作
	WorkThing();
}
//定义服务控制请求函数
VOID WINAPI ServiceHandler(DWORD dwControl)
{
	//实现服务控制请求函数，通过各类请求控制码决定服务做什么操作
	switch (dwControl)
	{
		//服务暂停
	case SERVICE_CONTROL_PAUSE:
		ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
		//服务继续运行
	case SERVICE_CONTROL_CONTINUE:
		ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
		//关闭服务(和停止服务走相同的额逻辑)
	case SERVICE_CONTROL_SHUTDOWN:
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwWaitHint = 0;
		SetServiceStatus(hStatus, &ServiceStatus);//设置服务状态
		//关闭服务和停止服务的时候需要先将服务停止
		StopService();
		//关闭和停止服务的时候需要同时将服务卸载
		UninstallService();
		return;
	default:
		break;
	}
	SetServiceStatus(hStatus, &ServiceStatus);//设置服务状态
	return;
}
//安装服务的函数
void InstallService()
{
	//首先获取当前进程的路径
	wchar_t PathName[MAX_PATH];
	wchar_t SysName[MAX_PATH];
	GetModuleFileName(NULL, PathName, MAX_PATH);
	GetSystemDirectory(SysName, MAX_PATH);
	wsprintf(SysName, L"%s\\ServiceTest.exe", SysName);
	if (!MoveFile(PathName, SysName))
	{
		printf(" Move File Error!\n");
		system("pause");
		return;
	}
	SC_Manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!SC_Manager)
	{
		printf("打开服务控制管理器失败");
		system("pause");
		return;
	}
	//c创建目标服务
	SC_Service = CreateService(SC_Manager, L"Demo", L"ServiceTest", SC_MANAGER_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, SysName, NULL, NULL, NULL, NULL, NULL);
	if (!SC_Service)
	{
		printf("创建服务失败");
	}
	printf("创建服务成功");
	CloseServiceHandle(SC_Service);
	CloseServiceHandle(SC_Manager);
}
//卸载服务函数
void UninstallService()
{
	SC_Manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!SC_Manager)
	{
		printf("打开服务控制管理器失败");
		system("pause");
		return;
	}
	//打开目标服务
	SC_Service = OpenService(SC_Manager, L"Demo", SC_MANAGER_ALL_ACCESS);
	if (!SC_Service)
	{
		printf("打开服务失败");
		CloseServiceHandle(SC_Manager);
		system("pause");
		return;
	}
	if (DeleteService(SC_Service))
	{
		printf("服务卸载成功");
	}
	CloseServiceHandle(SC_Service);
	CloseServiceHandle(SC_Manager);
	return;
}
//停止服务的函数
void StopService()
{
	//停止服务的主要思路是，发送控制吗到服务，将服务状态设置为暂停
	SC_Manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!SC_Manager)
	{
		printf("打开服务控制管理器失败");
		system("pause");
		return;
	}
	//打开目标服务
	SC_Service = OpenService(SC_Manager, L"Demo", SC_MANAGER_ALL_ACCESS);
	if (!SC_Service)
	{
		printf("打开服务失败");
		CloseServiceHandle(SC_Manager);
		system("pause");
		return;
	}
	if (ControlService(SC_Service, SERVICE_CONTROL_STOP, &ServiceStatus))
	{
		while (QueryServiceStatus(SC_Service, &ServiceStatus))
		{
			if (ServiceStatus.dwCurrentState == SERVICE_STOPPED)
				break;
		}
		CloseServiceHandle(SC_Service);
		CloseServiceHandle(SC_Manager);
		return;
	}
}
//启动服务
void StartDestService()
{
	SC_Manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!SC_Manager)
	{
		printf("打开服务控制管理器失败");
		system("pause");
		return;
	}
	//打开目标服务
	SC_Service = OpenService(SC_Manager, L"Demo", SC_MANAGER_ALL_ACCESS);
	if (!SC_Service)
	{
		printf("打开服务失败");
		CloseServiceHandle(SC_Manager);
		system("pause");
		return;
	}
	if (StartService(SC_Service, NULL, NULL))//启动服务，没有参数
	{
		//启动成功之后查看服务的状态
		while (QueryServiceStatus(SC_Service, &ServiceStatus))
		{
			if (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
				break;
		}
		printf("服务正常启动");
	}
	CloseServiceHandle(SC_Service);
	CloseServiceHandle(SC_Manager);
	return;
}
void WorkThing()
{
	return;
}
