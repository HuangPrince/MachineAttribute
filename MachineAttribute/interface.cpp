#include"pch.h"
#include"interface.h"
#include <winsock2.h>
#include<windows.h>
#include<string>
#include<fstream>
#include<iostream>
#include <Iphlpapi.h>
#include <comdef.h>
#include <DXGI.h>  
#include <vector>  
#include <Wbemidl.h>
#include<cstdint>
#include<algorithm>
#include<filesystem>
using namespace std;

#pragma comment(lib,"Iphlpapi.lib") //需要添加Iphlpapi.lib库
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib,"wbemuuid.lib")
#pragma warning(disable: 4996)

static const int kMaxInfoBuffer = 256;
#define _WIN32_DCOM
#define  GBYTES  1073741824  
#define  MBYTES  1048576  
#define  KBYTES  1024  
#define  DKBYTES 1024.0 

LPCWSTR StringtoLPCWSTR(std::string strInPut)
{
	size_t len = strInPut.length() + 1;
	const size_t newSize = 100;
	size_t convertedChars = 0;
	wchar_t* buffer = (wchar_t*)malloc(sizeof(wchar_t) * (strInPut.length() - 1));
	mbstowcs_s(&convertedChars, buffer, len, strInPut.c_str(), _TRUNCATE);
	return buffer;
}

/// <summary>
/// 写入TXT文件
/// </summary>
/// <param name="strTxtPath"></param>
/// <param name="strContent"></param>
/// <returns></returns>
int WriteTxt(const std::string strTxtPath, const std::string strContent)
{
	//创建文件输出流
	std::ofstream osFile(strTxtPath);
	if (osFile.is_open())
	{
		osFile << strContent << std::endl;
		osFile.close();
		return 0;
	}
	else
	{
		return -1;
	}
}


std::string ConvertString(WORD Input)
{
	char buffer[50];	//确保缓冲区足够大容纳转换后的字符串和终止符
	sprintf(buffer, "%u", Input);
	std::string strValue(buffer);
	return strValue;
}

/// <summary>
/// 获取当前时间
/// </summary>
int GetNowTime(std::string& strInfo)
{
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	strInfo += "************************************   ";
	strInfo += (ConvertString(systemTime.wYear) + "年");
	strInfo += (ConvertString(systemTime.wMonth) + "月");
	strInfo += (ConvertString(systemTime.wDay) + "日");
	strInfo += (ConvertString(systemTime.wHour) + "时");
	strInfo += (ConvertString(systemTime.wMinute) + "分");
	strInfo += (ConvertString(systemTime.wSecond) + "秒");
	strInfo += "   ************************************";
	strInfo += "\r\n\r\n";	
	return 0;
}


/// <summary>
/// 获取物理内存大小
/// </summary>
/// <param name="strInfo"></param>
/// <returns></returns>
int GetStorageSpace(std::string& strInfo)
{
	strInfo += "\r\n";
	std::string memory_info;
	MEMORYSTATUSEX statusex;
	statusex.dwLength = sizeof(statusex);
	if (GlobalMemoryStatusEx(&statusex))
	{
		unsigned long long total = 0, remain_total = 0, avl = 0, remain_avl = 0;
		double decimal_total = 0, decimal_avl = 0;
		remain_total = statusex.ullTotalPhys % GBYTES;
		total = statusex.ullTotalPhys / GBYTES;
		avl = statusex.ullAvailPhys / GBYTES;
		remain_avl = statusex.ullAvailPhys % GBYTES;
		if (remain_total > 0)
		{
			decimal_total = (remain_total / MBYTES) / DKBYTES;
		}
		if (remain_avl > 0)
		{
			decimal_avl = (remain_avl / MBYTES) / DKBYTES;
		}
		decimal_total += (double)total;
		decimal_avl += (double)avl;
		char  buffer[kMaxInfoBuffer];
		sprintf_s(buffer, kMaxInfoBuffer, "物理总内存： %.2f GB (%.2f GB 剩余使用)", decimal_total, decimal_avl);
		memory_info.append(buffer);
	}
	strInfo += memory_info;
	return 0;
}

/// <summary>
/// 获取CPU型号
/// </summary>
/// <param name="strInfo"></param>
/// <returns></returns>
int GetCPUInfo(std::string& strInfo)
{
#pragma region 汇编方式
	/*char cProStr[49];
	_asm
	{
		mov eax, 0x80000002
		cpuid
		mov dword ptr cProStr, eax
		mov dword ptr cProStr + 4, ebx
		mov dword ptr cProStr + 8, ecx
		mov dword ptr cProStr + 12, edx

		mov eax, 0x80000003
		cpuid
		mov dword ptr cProStr + 16, eax
		mov dword ptr cProStr + 20, ebx
		mov dword ptr cProStr + 24, ecx
		mov dword ptr cProStr + 28, edx

		mov eax, 0x80000004
		cpuid
		mov dword ptr cProStr + 32, eax
		mov dword ptr cProStr + 36, ebx
		mov dword ptr cProStr + 40, ecx
		mov dword ptr cProStr + 44, edx
	}
	std::string strNewInfo(cProStr);
	strInfo += ("CPU型号：" + strNewInfo);*/
#pragma endregion

#pragma region WMI方式
	HRESULT hres;
	//hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	hres = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	if (FAILED(hres))
	{
		/*cout << "Failed to initialize COM library. Error code = 0x"
			<< hex << hres << endl;*/
		return -1;                  // Program has failed.
	}

	//hres = CoInitializeSecurity(
	//	NULL,
	//	-1,                          // COM authentication
	//	NULL,                        // Authentication services
	//	NULL,                        // Reserved
	//	RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
	//	RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
	//	NULL,                        // Authentication info
	//	EOAC_NONE,                   // Additional capabilities 
	//	NULL                         // Reserved
	//);

	//if (FAILED(hres))
	//{
	//	/*cout << "Failed to initialize security. Error code = 0x"
	//		<< hex << hres << endl;*/
	//	CoUninitialize();
	//	return -1;                    // Program has failed.
	//}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator* pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&pLoc);

	if (FAILED(hres))
	{/*
		cout << "Failed to create IWbemLocator object."
			<< " Err code = 0x"
			<< hex << hres << endl;*/
		CoUninitialize();
		return -1;                 // Program has failed.
	}

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	IWbemServices* pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);

	if (FAILED(hres))
	{
		/*cout << "Could not connect. Error code = 0x"
			<< hex << hres << endl;*/
		pLoc->Release();
		CoUninitialize();
		return -1;                // Program has failed.
	}

	//cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		/*cout << "Could not set proxy blanket. Error code = 0x"
			<< hex << hres << endl;*/
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return -1;               // Program has failed.
	}

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----

	// For example, get the name of the operating system
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_Processor"),
		//bstr_t("SELECT * FROM Win64_Processor"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{/*
		cout << "ExecQuery(...) failed."
			<< " Error code = 0x"
			<< hex << hres << endl;*/
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return -1;               // Program has failed.
	}

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		// Get the value of the Name property
		hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		//wcout << " Name : " << vtProp.bstrVal << endl;

		//wcout << " CPU : " << vtProp.bstrVal << endl;
		BSTR bstr = vtProp.bstrVal;
		std::wstring wstr(bstr);
		std::string strRes(wstr.begin(), wstr.end());
		strInfo += "CPU型号：" + strRes;
		VariantClear(&vtProp);

		pclsObj->Release();
	}

	// Cleanup
	// ========

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return 0;
#pragma endregion
}


/// <summary>
/// 获取操作系统版本
/// </summary>
/// <param name="strInfo"></param>
/// <returns></returns>
int GetSystemVerion(std::string& strInfo)
{
	// get os name according to version number
	OSVERSIONINFO osver = { sizeof(OSVERSIONINFO) };
	GetVersionEx(&osver);
	std::string os_name;
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0)
	{
		os_name = "Windows 2000";
	}
	else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)
	{
		os_name = "Windows XP";
	}
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0)
	{
		os_name = "Windows 2003";
	}
	else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2)
	{
		os_name = "windows vista";
	}
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1)
	{
		os_name = "windows 7";
	}
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2)
	{
		os_name = "windows 10";
	}
	else
	{
		os_name = "	未识别操作系统版本";
	}
	strInfo += "\r\n";
	strInfo += "操作系统: " + os_name;
	return 0;
}


/// <summary>
/// 获取网络信息
/// </summary>
/// <param name="strInfo"></param>
/// <returns></returns>
int GetNetInfo(std::string& strInfo)
{
	strInfo += "\r\n";
	strInfo += (string)"--------------------------------------------------------------------" + "\r\n";
	//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//得到结构体大小,用于GetAdaptersInfo参数
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	//记录网卡数量
	int netCardNum = 0;
	//记录每张网卡上的IP地址数量
	int IPnumPerNetCard = 0;
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		//释放原来的内存空间
		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	if (ERROR_SUCCESS == nRel)
	{
		//输出网卡信息
		 //可能有多网卡,因此通过循环去判断
		while (pIpAdapterInfo)
		{
			strInfo += "网卡名称：" + (string)pIpAdapterInfo->AdapterName + "\r\n";
			strInfo += "网卡描述：" + (string)pIpAdapterInfo->Description + "\r\n";
			switch (pIpAdapterInfo->Type)
			{
			case MIB_IF_TYPE_OTHER:
				//cout << "网卡类型：" << "OTHER" << endl;
				strInfo += "网卡类型：" + (string)"OTHER" + "\r\n";
				break;
			case MIB_IF_TYPE_ETHERNET:
				//cout << "网卡类型：" << "ETHERNET" << endl;
				strInfo += "网卡类型：" + (string)"ETHERNET" + "\r\n";
				break;
			case MIB_IF_TYPE_TOKENRING:
				//cout << "网卡类型：" << "TOKENRING" << endl;
				strInfo += "网卡类型：" + (string)"TOKENRING" + "\r\n";
				break;
			case MIB_IF_TYPE_FDDI:
				//cout << "网卡类型：" << "FDDI" << endl;
				strInfo += "网卡类型：" + (string)"FDDI" + "\r\n";
				break;
			case MIB_IF_TYPE_PPP:
				//printf("PPn");
				//cout << "网卡类型：" << "PPP" << endl;
				strInfo += "网卡类型：" + (string)"PPP" + "\r\n";
				break;
			case MIB_IF_TYPE_LOOPBACK:
				//cout << "网卡类型：" << "LOOPBACK" << endl;
				strInfo += "网卡类型：" + (string)"LOOPBACK" + "\r\n";
				break;
			case MIB_IF_TYPE_SLIP:
				//cout << "网卡类型：" << "SLIP" << endl;
				strInfo += "网卡类型：" + (string)"SLIP" + "\r\n";
				break;
			default:

				break;
			}
			strInfo += (string)"网卡IP地址如下：" + "\r\n";
			//可能网卡有多IP,因此通过循环去判断
			IP_ADDR_STRING* pIpAddrString = &(pIpAdapterInfo->IpAddressList);
			do
			{
				strInfo += "IP 地址：" + (string)pIpAddrString->IpAddress.String + "\r\n";
				strInfo += "子网地址：" + (string)pIpAddrString->IpMask.String + "\r\n";;
				strInfo += "网关地址：" + (string)pIpAdapterInfo->GatewayList.IpAddress.String + "\r\n";;
				pIpAddrString = pIpAddrString->Next;
			} while (pIpAddrString);
			pIpAdapterInfo = pIpAdapterInfo->Next;
			strInfo += (string)"--------------------------------------------------------------------" + "\r\n";
		}

	}
	//释放内存空间
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}

	return 0;
}


struct DiskSpace
{
	std::string strName;
	ULONGLONG ulSpace;
};
std::vector<DiskSpace> vecDisk;
bool Compare(const DiskSpace& diskone, const DiskSpace& disktwo)
{
	return diskone.ulSpace < disktwo.ulSpace;
}
/// <summary>
/// 获取所有硬盘信息
/// </summary>
/// <param name="strInfo"></param>
/// <returns></returns>
int GetDiskSpace(std::string& strInfo)
{
	strInfo += "\r\n" + (std::string)"硬盘信息：" + "\r\n";
	for (char cDiskTop = 'C'; cDiskTop <= 'Z'; cDiskTop++)
	{
		//构建每个磁盘路径
		std::string strDiskPath = cDiskTop + (std::string)":\\";
		//定义ULARGE_INTEGER结构体存储磁盘信息
		ULARGE_INTEGER uiTotalSpace, uiFreeSpace;
		//调用GetDiskFreeSpaceEx函数获取空间信息
		if (GetDiskFreeSpaceEx(StringtoLPCWSTR(strDiskPath), NULL, &uiTotalSpace, &uiFreeSpace) != 0)
		{
			strInfo += strDiskPath + "盘  ";
			strInfo += "总空间： " + std::to_string(uiTotalSpace.QuadPart / GBYTES) + " GB  ";
			strInfo += "被用空间: " + std::to_string((uiTotalSpace.QuadPart / GBYTES - uiFreeSpace.QuadPart / GBYTES)) + " GB  ";
			strInfo += "剩余空间：" + std::to_string(uiFreeSpace.QuadPart / GBYTES) + " GB" + "\r\n";
			if (strDiskPath != "C:\\")
			{
				DiskSpace dsDisk;
				dsDisk.strName = strDiskPath;
				dsDisk.ulSpace = uiFreeSpace.QuadPart / GBYTES;
				vecDisk.push_back(dsDisk);
			}
		}
	}
	auto atMaxElement = std::max_element(vecDisk.begin(), vecDisk.end(), Compare);
	strInfo += "\r\n" + (std::string)"最大剩余空间盘符为：" + atMaxElement._Ptr->strName + "盘    最大剩余空间盘符空间为：" + std::to_string(atMaxElement._Ptr->ulSpace) + " GB" + "\r\n";
	char cBuffer[MAX_PATH];
	GetModuleFileNameA(NULL, cBuffer, MAX_PATH);
	std::string strPath(cBuffer);
	std::string strPathDisk = strPath.substr(0, 3);
	if (strPathDisk != atMaxElement._Ptr->strName)
	{
		strInfo += "该软件当前存储盘符为：" + strPathDisk + "盘" + "    建议将该软件存储盘符移动到：" + atMaxElement._Ptr->strName + "盘" + "\r\n";
	}
	return 0;
}


/// <summary>
/// 获取所有显卡信息
/// </summary>
/// <param name="strInfo"></param>
/// <returns></returns>
int GetGraphicsInfo(std::string& strInfo)
{
	strInfo += "\r\n";
	HRESULT hres;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	//hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	hres = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	if (FAILED(hres))
	{
		return -1;                  // Program has failed.
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------

	//hres = CoInitializeSecurity(
	//	NULL,
	//	-1,                          // COM authentication
	//	NULL,                        // Authentication services
	//	NULL,                        // Reserved
	//	RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
	//	RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
	//	NULL,                        // Authentication info
	//	EOAC_NONE,                   // Additional capabilities 
	//	NULL                         // Reserved
	//);


	//if (FAILED(hres))
	//{
	//	CoUninitialize();
	//	return -1;                    // Program has failed.
	//}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator* pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&pLoc);

	if (FAILED(hres))
	{
		//cout << "Failed to create IWbemLocator object."
		//	<< " Err code = 0x"
		//	<< hex << hres << endl;
		CoUninitialize();
		return -1;                 // Program has failed.
	}

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	IWbemServices* pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);

	if (FAILED(hres))
	{
		//cout << "Could not connect. Error code = 0x"
		//	<< hex << hres << endl;
		pLoc->Release();
		CoUninitialize();
		return -1;                // Program has failed.
	}

	//cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		//cout << "Could not set proxy blanket. Error code = 0x"
		//	<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return -1;               // Program has failed.
	}

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----

	// For example, get the name of the operating system
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_VideoController"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		//cout << "Query for operating system name failed."
		//	<< " Error code = 0x"
		//	<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return -1;               // Program has failed.
	}

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		// Get the value of the Name property
		hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		//wcout << "显卡名称:  " << vtProp.bstrVal << endl;
		BSTR bstrN = vtProp.bstrVal;
		std::wstring wstrN(bstrN);
		std::string strResN(wstrN.begin(), wstrN.end());
		strInfo += "显卡名称：" + strResN + "\r\n";

		hr = pclsObj->Get(L"DriverVersion", 0, &vtProp, 0, 0);
		BSTR bstrX = vtProp.bstrVal;
		std::wstring wstrX(bstrX);
		std::string strResX(wstrX.begin(), wstrX.end());
		strInfo += "显卡驱动程序版本：" + strResX;
		//wcout << "显卡驱动程序版本:  " << vtProp.bstrVal << endl;

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	// Cleanup
	// ========
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();
	return 0;
}




/// <summary>
/// 获取当前机器的各种属性
/// </summary>
/// <param name="strIniPath">输出的TXT文件路径</param>
/// <param name="strIniName">输出的TXT文件名</param>
/// <returns>0：获取信息成功；-1：获取信息失败</returns>
int __stdcall GetMachineInfo(const std::string strTxtPath, const std::string strTxtName)
{
	std::string strCompetePath = strTxtPath + strTxtName;

	std::string strDiskResult = "";
	GetNowTime(strDiskResult);
	GetCPUInfo(strDiskResult);
	GetSystemVerion(strDiskResult);
	GetStorageSpace(strDiskResult);
	GetDiskSpace(strDiskResult);
	GetGraphicsInfo(strDiskResult);
	GetNetInfo(strDiskResult);
	WriteTxt(strCompetePath, strDiskResult);
	return 0;
}

int __stdcall GetMachineInfoNo()
{
	std::string strCompetePath = "AppRes\\images\\Description\\关于.txt";
	std::string strDiskResult = "";
	GetNowTime(strDiskResult);
	GetCPUInfo(strDiskResult);
	GetSystemVerion(strDiskResult);
	GetStorageSpace(strDiskResult);
	GetDiskSpace(strDiskResult);
	GetGraphicsInfo(strDiskResult);
	GetNetInfo(strDiskResult);
	WriteTxt(strCompetePath, strDiskResult);
	return 0;
}
