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

#pragma comment(lib,"Iphlpapi.lib") //��Ҫ���Iphlpapi.lib��
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
/// д��TXT�ļ�
/// </summary>
/// <param name="strTxtPath"></param>
/// <param name="strContent"></param>
/// <returns></returns>
int WriteTxt(const std::string strTxtPath, const std::string strContent)
{
	//�����ļ������
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
	char buffer[50];	//ȷ���������㹻������ת������ַ�������ֹ��
	sprintf(buffer, "%u", Input);
	std::string strValue(buffer);
	return strValue;
}

/// <summary>
/// ��ȡ��ǰʱ��
/// </summary>
int GetNowTime(std::string& strInfo)
{
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	strInfo += "************************************   ";
	strInfo += (ConvertString(systemTime.wYear) + "��");
	strInfo += (ConvertString(systemTime.wMonth) + "��");
	strInfo += (ConvertString(systemTime.wDay) + "��");
	strInfo += (ConvertString(systemTime.wHour) + "ʱ");
	strInfo += (ConvertString(systemTime.wMinute) + "��");
	strInfo += (ConvertString(systemTime.wSecond) + "��");
	strInfo += "   ************************************";
	strInfo += "\r\n\r\n";	
	return 0;
}


/// <summary>
/// ��ȡ�����ڴ��С
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
		sprintf_s(buffer, kMaxInfoBuffer, "�������ڴ棺 %.2f GB (%.2f GB ʣ��ʹ��)", decimal_total, decimal_avl);
		memory_info.append(buffer);
	}
	strInfo += memory_info;
	return 0;
}

/// <summary>
/// ��ȡCPU�ͺ�
/// </summary>
/// <param name="strInfo"></param>
/// <returns></returns>
int GetCPUInfo(std::string& strInfo)
{
#pragma region ��෽ʽ
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
	strInfo += ("CPU�ͺţ�" + strNewInfo);*/
#pragma endregion

#pragma region WMI��ʽ
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
		strInfo += "CPU�ͺţ�" + strRes;
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
/// ��ȡ����ϵͳ�汾
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
		os_name = "	δʶ�����ϵͳ�汾";
	}
	strInfo += "\r\n";
	strInfo += "����ϵͳ: " + os_name;
	return 0;
}


/// <summary>
/// ��ȡ������Ϣ
/// </summary>
/// <param name="strInfo"></param>
/// <returns></returns>
int GetNetInfo(std::string& strInfo)
{
	strInfo += "\r\n";
	strInfo += (string)"--------------------------------------------------------------------" + "\r\n";
	//PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//�õ��ṹ���С,����GetAdaptersInfo����
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	//��¼��������
	int netCardNum = 0;
	//��¼ÿ�������ϵ�IP��ַ����
	int IPnumPerNetCard = 0;
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//����������ص���ERROR_BUFFER_OVERFLOW
		//��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С
		//��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������
		//�ͷ�ԭ�����ڴ�ռ�
		delete pIpAdapterInfo;
		//���������ڴ�ռ������洢����������Ϣ
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	if (ERROR_SUCCESS == nRel)
	{
		//���������Ϣ
		 //�����ж�����,���ͨ��ѭ��ȥ�ж�
		while (pIpAdapterInfo)
		{
			strInfo += "�������ƣ�" + (string)pIpAdapterInfo->AdapterName + "\r\n";
			strInfo += "����������" + (string)pIpAdapterInfo->Description + "\r\n";
			switch (pIpAdapterInfo->Type)
			{
			case MIB_IF_TYPE_OTHER:
				//cout << "�������ͣ�" << "OTHER" << endl;
				strInfo += "�������ͣ�" + (string)"OTHER" + "\r\n";
				break;
			case MIB_IF_TYPE_ETHERNET:
				//cout << "�������ͣ�" << "ETHERNET" << endl;
				strInfo += "�������ͣ�" + (string)"ETHERNET" + "\r\n";
				break;
			case MIB_IF_TYPE_TOKENRING:
				//cout << "�������ͣ�" << "TOKENRING" << endl;
				strInfo += "�������ͣ�" + (string)"TOKENRING" + "\r\n";
				break;
			case MIB_IF_TYPE_FDDI:
				//cout << "�������ͣ�" << "FDDI" << endl;
				strInfo += "�������ͣ�" + (string)"FDDI" + "\r\n";
				break;
			case MIB_IF_TYPE_PPP:
				//printf("PPn");
				//cout << "�������ͣ�" << "PPP" << endl;
				strInfo += "�������ͣ�" + (string)"PPP" + "\r\n";
				break;
			case MIB_IF_TYPE_LOOPBACK:
				//cout << "�������ͣ�" << "LOOPBACK" << endl;
				strInfo += "�������ͣ�" + (string)"LOOPBACK" + "\r\n";
				break;
			case MIB_IF_TYPE_SLIP:
				//cout << "�������ͣ�" << "SLIP" << endl;
				strInfo += "�������ͣ�" + (string)"SLIP" + "\r\n";
				break;
			default:

				break;
			}
			strInfo += (string)"����IP��ַ���£�" + "\r\n";
			//���������ж�IP,���ͨ��ѭ��ȥ�ж�
			IP_ADDR_STRING* pIpAddrString = &(pIpAdapterInfo->IpAddressList);
			do
			{
				strInfo += "IP ��ַ��" + (string)pIpAddrString->IpAddress.String + "\r\n";
				strInfo += "������ַ��" + (string)pIpAddrString->IpMask.String + "\r\n";;
				strInfo += "���ص�ַ��" + (string)pIpAdapterInfo->GatewayList.IpAddress.String + "\r\n";;
				pIpAddrString = pIpAddrString->Next;
			} while (pIpAddrString);
			pIpAdapterInfo = pIpAdapterInfo->Next;
			strInfo += (string)"--------------------------------------------------------------------" + "\r\n";
		}

	}
	//�ͷ��ڴ�ռ�
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
/// ��ȡ����Ӳ����Ϣ
/// </summary>
/// <param name="strInfo"></param>
/// <returns></returns>
int GetDiskSpace(std::string& strInfo)
{
	strInfo += "\r\n" + (std::string)"Ӳ����Ϣ��" + "\r\n";
	for (char cDiskTop = 'C'; cDiskTop <= 'Z'; cDiskTop++)
	{
		//����ÿ������·��
		std::string strDiskPath = cDiskTop + (std::string)":\\";
		//����ULARGE_INTEGER�ṹ��洢������Ϣ
		ULARGE_INTEGER uiTotalSpace, uiFreeSpace;
		//����GetDiskFreeSpaceEx������ȡ�ռ���Ϣ
		if (GetDiskFreeSpaceEx(StringtoLPCWSTR(strDiskPath), NULL, &uiTotalSpace, &uiFreeSpace) != 0)
		{
			strInfo += strDiskPath + "��  ";
			strInfo += "�ܿռ䣺 " + std::to_string(uiTotalSpace.QuadPart / GBYTES) + " GB  ";
			strInfo += "���ÿռ�: " + std::to_string((uiTotalSpace.QuadPart / GBYTES - uiFreeSpace.QuadPart / GBYTES)) + " GB  ";
			strInfo += "ʣ��ռ䣺" + std::to_string(uiFreeSpace.QuadPart / GBYTES) + " GB" + "\r\n";
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
	strInfo += "\r\n" + (std::string)"���ʣ��ռ��̷�Ϊ��" + atMaxElement._Ptr->strName + "��    ���ʣ��ռ��̷��ռ�Ϊ��" + std::to_string(atMaxElement._Ptr->ulSpace) + " GB" + "\r\n";
	char cBuffer[MAX_PATH];
	GetModuleFileNameA(NULL, cBuffer, MAX_PATH);
	std::string strPath(cBuffer);
	std::string strPathDisk = strPath.substr(0, 3);
	if (strPathDisk != atMaxElement._Ptr->strName)
	{
		strInfo += "�������ǰ�洢�̷�Ϊ��" + strPathDisk + "��" + "    ���齫������洢�̷��ƶ�����" + atMaxElement._Ptr->strName + "��" + "\r\n";
	}
	return 0;
}


/// <summary>
/// ��ȡ�����Կ���Ϣ
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
		//wcout << "�Կ�����:  " << vtProp.bstrVal << endl;
		BSTR bstrN = vtProp.bstrVal;
		std::wstring wstrN(bstrN);
		std::string strResN(wstrN.begin(), wstrN.end());
		strInfo += "�Կ����ƣ�" + strResN + "\r\n";

		hr = pclsObj->Get(L"DriverVersion", 0, &vtProp, 0, 0);
		BSTR bstrX = vtProp.bstrVal;
		std::wstring wstrX(bstrX);
		std::string strResX(wstrX.begin(), wstrX.end());
		strInfo += "�Կ���������汾��" + strResX;
		//wcout << "�Կ���������汾:  " << vtProp.bstrVal << endl;

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
/// ��ȡ��ǰ�����ĸ�������
/// </summary>
/// <param name="strIniPath">�����TXT�ļ�·��</param>
/// <param name="strIniName">�����TXT�ļ���</param>
/// <returns>0����ȡ��Ϣ�ɹ���-1����ȡ��Ϣʧ��</returns>
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
	std::string strCompetePath = "AppRes\\images\\Description\\����.txt";
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
