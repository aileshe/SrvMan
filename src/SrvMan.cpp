#include "SrvMan.h"
#include <Windows.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "advapi32.lib")

namespace sc
{
	class CSrvMan : public SrvMan
	{
	private:
		SC_HANDLE schSCManager = NULL;
		SC_HANDLE schService = NULL;
		SERVICE_STATUS ssSvcStatus = {}; // 服务状态

	public:
		/* 初始化 */
		bool Init()
		{
			/* 打开服务管理对象 */
			schSCManager = ::OpenSCManager(NULL,
				NULL, GENERIC_EXECUTE);
			if (schSCManager == NULL)
			{
				//cout << "open SCManager error!" << endl;
				Cleanup();
				return false;
			}
			return true;
		}

		/* 启动服务 */
		bool Run(LPCWSTR service_name)
		{
			if (schSCManager == NULL)
			{
				if (!Init())
					return false;
			}

			/* 获得操作服务句柄 与fopen相似 模: 启动|查询状态|暂停 */
			schService = ::OpenService(schSCManager, service_name,
				SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
			if (schService == NULL)
			{
				//cout << "Open service error!" << endl;
				Cleanup();
				return false;
			}

			/* 获得服务状态 */
			if (::QueryServiceStatus(schService, &ssSvcStatus) == FALSE)
			{
				//cout << "Get Service state error!" << endl;
				Cleanup();
				return false;
			}

			if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
			{
				// 启动服务
				if (::StartService(schService, NULL, NULL) == FALSE)
				{
					//cout << "start service error!" << endl;
					Cleanup();
					return false;
				}
				// 等待服务启动
				while (::QueryServiceStatus(schService, &ssSvcStatus) == TRUE)
				{
					::Sleep(ssSvcStatus.dwWaitHint);
					if (ssSvcStatus.dwCurrentState == SERVICE_RUNNING)
					{
						//cout << "start success!" << endl;
						Cleanup();
						return true;
					}
				}
			}

			Cleanup();
			return true;
		}

		/* 暂停服务 */
		bool Stop(LPCWSTR service_name)
		{
			if (schSCManager == NULL)
			{
				if (!Init())
					return false;
			}

			/* 获得操作服务句柄 与fopen相似 模: 启动|查询状态|暂停 */
			schService = ::OpenService(schSCManager, service_name,
				SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
			if (schService == NULL)
			{
				//cout << "Open service error!" << endl;
				Cleanup();
				return false;
			}

			/* 获得服务状态 */
			if (::QueryServiceStatus(schService, &ssSvcStatus) == FALSE)
			{
				//cout << "Get Service state error!" << endl;
				Cleanup();
				return false;
			}
			if (ssSvcStatus.dwCurrentState == SERVICE_RUNNING)
			{
				// 停止服务
				if (::ControlService(schService,
					SERVICE_CONTROL_STOP, &ssSvcStatus) == FALSE)
				{
					//cout << "stop service error!" << endl;
					Cleanup();
					return false;
				}
				// 等待服务停止
				while (::QueryServiceStatus(schService, &ssSvcStatus) == TRUE)
				{
					::Sleep(ssSvcStatus.dwWaitHint);
					if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
					{
						//cout << "stop success!" << endl;
						Cleanup();
						return true;
					}
				}
			}
			Cleanup();
			return true;
		}

		/* 创建服务 */
		bool Create(PWSTR szPath, // 服务程序路径
			PWSTR pszServiceName, // 服务名称
			PWSTR pszDisplayName, // 显示名称
			DWORD dwStartType,    // 启动类型  SERVICE_AUTO_START,SERVICE_DEMAND_START...
			PWSTR pszDependencies,// List of service dependencies - "dep1\0dep2\0\0", 一般填空 L""
			PWSTR pszAccount,     // 登录用户, L"NT AUTHORITY\\LocalService" or L"NT AUTHORITY\\SYSTEM" or L"NT AUTHORITY\\NETWORK SERVICE"
			PWSTR pszPassword     // 用户密码, 一般填空 NULL
		)
		{
			// Open the local default service control manager database
			schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT |
				SC_MANAGER_CREATE_SERVICE);
			if (schSCManager == NULL)
			{
				//wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
				Cleanup();
				return false;
			}

			// Install the service into SCM by calling CreateService
			schService = CreateService(
				schSCManager,                   // SCManager database
				pszServiceName,                 // Name of service
				pszDisplayName,                 // Name to display
				SERVICE_QUERY_STATUS,           // Desired access
				SERVICE_WIN32_OWN_PROCESS,      // Service type
				dwStartType,                    // Service start type
				SERVICE_ERROR_NORMAL,           // Error control type
				szPath,                         // Service's binary
				NULL,                           // No load ordering group
				NULL,                           // No tag identifier
				pszDependencies,                // Dependencies
				pszAccount,                     // Service running account
				pszPassword                     // Password of the account
			);
			if (schService == NULL)
			{
				//wprintf(L"CreateService failed w/err 0x%08lx\n", GetLastError());
				Cleanup();
				return false;
			}

			Cleanup();
			return true;
		}

		/* 删除服务 */
		bool Delete(LPCWSTR service_name)
		{
			if (schSCManager == NULL)
			{
				if (!Init())
					return false;
			}

			/* 获得操作服务句柄 与fopen相似 模: 启动|查询状态|暂停 */
			schService = ::OpenService(schSCManager, service_name,
				SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
			if (schService == NULL)
			{
				//cout << "Open service error!" << endl;
				Cleanup();
				return false;
			}

			/* 获得服务状态 */
			if (::QueryServiceStatus(schService, &ssSvcStatus) == FALSE)
			{
				//cout << "Get Service state error!" << endl;
				Cleanup();
				return false;
			}

			if (ssSvcStatus.dwCurrentState == SERVICE_RUNNING)
			{
				// 停止服务
				if (::ControlService(schService,
					SERVICE_CONTROL_STOP, &ssSvcStatus) == FALSE)
				{
					//cout << "stop service error!" << endl;
					Cleanup();
					return false;
				}
				// 等待服务停止
				while (::QueryServiceStatus(schService, &ssSvcStatus))
				{
					::Sleep(ssSvcStatus.dwWaitHint);
					if (ssSvcStatus.dwCurrentState != SERVICE_STOP_PENDING)
						break;
				}
				if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
				{
					//cout << "stop success!" << endl;
					// 开始删除服务
					if (!::DeleteService(schService))
					{
						printf("DeleteService failed w/err 0x%08lx\n", GetLastError());
						Cleanup();
						return false;
					}

					Cleanup();
					return true;
				}
				Cleanup();
				return false;
			}
			// 开始删除服务
			if (!::DeleteService(schService))
			{
				printf("DeleteService failed w/err 0x%08lx\n", GetLastError());
				Cleanup();
				return false;
			}

			Cleanup();
			return true;
		}

		/* 判断服务是否存在 */
		bool Has(LPCWSTR service_name)
		{
			if (schSCManager == NULL)
			{
				if (!Init())
					return false;
			}

			/* 获得操作服务句柄 与fopen相似 模: 启动|查询状态|暂停 */
			schService = ::OpenService(schSCManager, service_name,
				SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
			if (schService == NULL)
			{
				//cout << "Open service error!" << endl;
				Cleanup();
				return false;
			}

			Cleanup();
			return true;
		}

		/* 空间清理 */
		void Cleanup()
		{
			// 空间释放
			if (schSCManager)
			{
				::CloseServiceHandle(schSCManager);
				schSCManager = NULL;
			}
			if (schService)
			{
				::CloseServiceHandle(schService);
				schService = NULL;
			}
			ssSvcStatus = {};
		}

	};

	/* 工厂方法 */
	SrvMan* SrvMan::Get()
	{
		static SrvMan* wsm = 0;
		if (0 == wsm)
		{
			static CSrvMan c_wsm;
			wsm = &c_wsm;
		}
		return wsm;
	}

	SrvMan::SrvMan()
	{
	}


	SrvMan::~SrvMan()
	{
	}
}
