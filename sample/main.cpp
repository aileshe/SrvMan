#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "SrvMan.h"
using namespace std;
using namespace sc; 

int main(int argc, char **argv)
{
	// ��������
	const wchar_t* service_name = L"MyTestService";

	// �жϷ����Ƿ����
	if (sc_has(service_name))
		cout << "�������!" << endl;
	else
		cout << "���񲻴���!" << endl;

	// ��������
	/*if (sc_run(service_name))
		cout << "���������ɹ�!" << endl;
	else
		cout << "��������ʧ��!" << endl;}*/

	// ��ͣ����
	/*
	if (sc_stop(service_name))
		cout << "������ͣ�ɹ�!" << endl;
	else
		cout << "������ͣʧ��!" << endl;
	*/

	// ��������
	/*
	PWSTR szPath,         // �������·��
	PWSTR pszServiceName, // ��������
	PWSTR pszDisplayName, // ��ʾ����
	DWORD dwStartType,    // ��������  SERVICE_AUTO_START,SERVICE_DEMAND_START...
	PWSTR pszDependencies,// List of service dependencies - "dep1\0dep2\0\0", һ����� L""
	PWSTR pszAccount,     // ��¼�û�, L"NT AUTHORITY\\LocalService" or L"NT AUTHORITY\\SYSTEM" or L"NT AUTHORITY\\NETWORK SERVICE"
	PWSTR pszPassword     // �û�����, һ����� NULL
	*/
	/*if (sc_create(
		L"C:/Program Files/MyTestService/MyTestService.exe",
		service_name,
		service_name,
		SERVICE_AUTO_START,
		L"",
		L"NT AUTHORITY\\SYSTEM",
		NULL
	))
		cout << "��������ɹ�!" << endl;
	else
		cout << "��������ʧ��!" << endl;
	*/


	// ɾ������
	/*if (sc_remove(service_name))
		cout << "ɾ������ɹ�!" << endl;
	else
		cout << "ɾ������ʧ��!" << endl;*/
		




	getchar();
	return 0;
}