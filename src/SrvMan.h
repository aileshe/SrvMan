#pragma once

namespace sc 
{
// Start Type
#define SERVICE_BOOT_START             0x00000000
#define SERVICE_SYSTEM_START           0x00000001
#define SERVICE_AUTO_START             0x00000002
#define SERVICE_DEMAND_START           0x00000003
#define SERVICE_DISABLED               0x00000004
typedef const wchar_t* LPCWSTR;
typedef const wchar_t* PWSTR;
typedef unsigned long  DWORD;

	class SrvMan
	{
	public:
		/* 工厂方法 */
		static SrvMan* Get();

		/* 启动服务 */
		virtual bool Run(LPCWSTR service_name) = 0;

		/* 暂停服务 */
		virtual bool Stop(LPCWSTR service_name) = 0;

		/* 创建服务 */
		virtual bool Create(PWSTR szPath, // 服务程序路径
			PWSTR pszServiceName, // 服务名称
			PWSTR pszDisplayName, // 显示名称
			DWORD dwStartType,    // 启动类型  SERVICE_AUTO_START,SERVICE_DEMAND_START...
			PWSTR pszDependencies,// List of service dependencies - "dep1\0dep2\0\0", 一般填空 L""
			PWSTR pszAccount,     // 登录用户, L"NT AUTHORITY\\LocalService" or L"NT AUTHORITY\\SYSTEM" or L"NT AUTHORITY\\NETWORK SERVICE"
			PWSTR pszPassword     // 用户密码, 一般填空 NULL
		) = 0;

		/* 删除服务 */
		virtual bool Delete(LPCWSTR service_name) = 0;

		/* 判断服务是否存在 */
		virtual bool Has(LPCWSTR service_name) = 0;



		virtual ~SrvMan();

	protected:
		SrvMan();
	};

	/* 启动服务 */
	#define sc_run SrvMan::Get()->Run
	/* 暂停服务 */
	#define sc_stop SrvMan::Get()->Stop
	/* 判断服务是否存在 */
	#define sc_has SrvMan::Get()->Has
	/* 删除服务 */
	#define sc_remove SrvMan::Get()->Delete
	/* 创建服务 */
	#define sc_create SrvMan::Get()->Create

}



