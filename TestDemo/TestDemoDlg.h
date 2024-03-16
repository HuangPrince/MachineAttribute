
// TestDemoDlg.h: 头文件
//

#pragma once
#include<string>

#ifdef _DEBUG
#define MACHINEDLL_NAME "MachineAttributed.dll"
#else
#define MACHINEDLL_NAME "MachineAttribute.dll"
#endif

//typedef int (*PFnMachineInfo)(const std::string strTxtPath, const std::string strTxtName);
typedef int (*PFnMachineInfo)();


// CTestDemoDlg 对话框
class CTestDemoDlg : public CDialogEx
{
	// 构造
public:
	CTestDemoDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTDEMO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditabout();
	


public:
	//动态库句柄
	HINSTANCE m_hMachDll;
	CString m_strRet;
	PFnMachineInfo m_pfnMachineInfo;
	bool m_bLoadOK;
};
