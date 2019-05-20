# SrvMan
A simple windows service manage class in C++.
简单的windows service管理 C++封装类

## 下载安装:
```
git clone https://github.com/aileshe/SrvMan.git
```

## 使用 Sample:
```
#include <iostream>
#include "SrvMan.h"
using namespace std;
using namespace sc; 

int main(int argc, char **argv)
{

	// 服务名称
	const wchar_t* service_name = L"MyTestService";

	// 判断服务是否存在
	if (sc_has(service_name))
		cout << "服务存在!" << endl;
	else
		cout << "服务不存在!" << endl;

	// 启动服务
	if (sc_run(service_name))
		cout << "服务启动成功!" << endl;
	else
		cout << "服务启动失败!" << endl;

	// 暂停服务
	if (sc_stop(service_name))
		cout << "服务暂停成功!" << endl;
	else
		cout << "服务暂停失败!" << endl;


	// 创建服务
	/*
	PWSTR szPath,         // 服务程序路径
	PWSTR pszServiceName, // 服务名称
	PWSTR pszDisplayName, // 显示名称
	DWORD dwStartType,    // 启动类型  SERVICE_AUTO_START,SERVICE_DEMAND_START...
	PWSTR pszDependencies,// List of service dependencies - "dep1\0dep2\0\0", 一般填空 L""
	PWSTR pszAccount,     // 登录用户, L"NT AUTHORITY\\LocalService" or L"NT AUTHORITY\\SYSTEM" or L"NT AUTHORITY\\NETWORK SERVICE"
	PWSTR pszPassword     // 用户密码, 一般填空 NULL
	*/
	if (sc_create(
		L"C:/Program Files/MyTestService/MyTestService.exe",
		service_name,
		service_name,
		SERVICE_AUTO_START,
		L"",
		L"NT AUTHORITY\\SYSTEM",
		NULL
	))
		cout << "创建服务成功!" << endl;
	else
		cout << "创建服务失败!" << endl;


	// 删除服务
	if (sc_remove(service_name))
		cout << "删除服务成功!" << endl;
	else
		cout << "删除服务失败!" << endl;


	getchar();
	return 0;
}

```
## 联系方式
Author: Dejan

QQ: 673008865
