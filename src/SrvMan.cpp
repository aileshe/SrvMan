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
		SERVICE_STATUS ssSvcStatus = {}; // ����״̬

	public:
		/* ��ʼ�� */
		bool Init()
		{
			/* �򿪷��������� */
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

		/* �������� */
		bool Run(LPCWSTR service_name)
		{
			if (schSCManager == NULL)
			{
				if (!Init())
					return false;
			}

			/* ��ò��������� ��fopen���� ģ: ����|��ѯ״̬|��ͣ */
			schService = ::OpenService(schSCManager, service_name,
				SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
			if (schService == NULL)
			{
				//cout << "Open service error!" << endl;
				Cleanup();
				return false;
			}

			/* ��÷���״̬ */
			if (::QueryServiceStatus(schService, &ssSvcStatus) == FALSE)
			{
				//cout << "Get Service state error!" << endl;
				Cleanup();
				return false;
			}

			if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
			{
				// ��������
				if (::StartService(schService, NULL, NULL) == FALSE)
				{
					//cout << "start service error!" << endl;
					Cleanup();
					return false;
				}
				// �ȴ���������
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

		/* ��ͣ���� */
		bool Stop(LPCWSTR service_name)
		{
			if (schSCManager == NULL)
			{
				if (!Init())
					return false;
			}

			/* ��ò��������� ��fopen���� ģ: ����|��ѯ״̬|��ͣ */
			schService = ::OpenService(schSCManager, service_name,
				SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
			if (schService == NULL)
			{
				//cout << "Open service error!" << endl;
				Cleanup();
				return false;
			}

			/* ��÷���״̬ */
			if (::QueryServiceStatus(schService, &ssSvcStatus) == FALSE)
			{
				//cout << "Get Service state error!" << endl;
				Cleanup();
				return false;
			}
			if (ssSvcStatus.dwCurrentState == SERVICE_RUNNING)
			{
				// ֹͣ����
				if (::ControlService(schService,
					SERVICE_CONTROL_STOP, &ssSvcStatus) == FALSE)
				{
					//cout << "stop service error!" << endl;
					Cleanup();
					return false;
				}
				// �ȴ�����ֹͣ
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

		/* �������� */
		bool Create(PWSTR szPath, // �������·��
			PWSTR pszServiceName, // ��������
			PWSTR pszDisplayName, // ��ʾ����
			DWORD dwStartType,    // ��������  SERVICE_AUTO_START,SERVICE_DEMAND_START...
			PWSTR pszDependencies,// List of service dependencies - "dep1\0dep2\0\0", һ����� L""
			PWSTR pszAccount,     // ��¼�û�, L"NT AUTHORITY\\LocalService" or L"NT AUTHORITY\\SYSTEM" or L"NT AUTHORITY\\NETWORK SERVICE"
			PWSTR pszPassword     // �û�����, һ����� NULL
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

		/* ɾ������ */
		bool Delete(LPCWSTR service_name)
		{
			if (schSCManager == NULL)
			{
				if (!Init())
					return false;
			}

			/* ��ò��������� ��fopen���� ģ: ����|��ѯ״̬|��ͣ */
			schService = ::OpenService(schSCManager, service_name,
				SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
			if (schService == NULL)
			{
				//cout << "Open service error!" << endl;
				Cleanup();
				return false;
			}

			/* ��÷���״̬ */
			if (::QueryServiceStatus(schService, &ssSvcStatus) == FALSE)
			{
				//cout << "Get Service state error!" << endl;
				Cleanup();
				return false;
			}

			if (ssSvcStatus.dwCurrentState == SERVICE_RUNNING)
			{
				// ֹͣ����
				if (::ControlService(schService,
					SERVICE_CONTROL_STOP, &ssSvcStatus) == FALSE)
				{
					//cout << "stop service error!" << endl;
					Cleanup();
					return false;
				}
				// �ȴ�����ֹͣ
				while (::QueryServiceStatus(schService, &ssSvcStatus))
				{
					::Sleep(ssSvcStatus.dwWaitHint);
					if (ssSvcStatus.dwCurrentState != SERVICE_STOP_PENDING)
						break;
				}
				if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
				{
					//cout << "stop success!" << endl;
					// ��ʼɾ������
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
			// ��ʼɾ������
			if (!::DeleteService(schService))
			{
				printf("DeleteService failed w/err 0x%08lx\n", GetLastError());
				Cleanup();
				return false;
			}

			Cleanup();
			return true;
		}

		/* �жϷ����Ƿ���� */
		bool Has(LPCWSTR service_name)
		{
			if (schSCManager == NULL)
			{
				if (!Init())
					return false;
			}

			/* ��ò��������� ��fopen���� ģ: ����|��ѯ״̬|��ͣ */
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

		/* �ռ����� */
		void Cleanup()
		{
			// �ռ��ͷ�
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

	/* �������� */
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
