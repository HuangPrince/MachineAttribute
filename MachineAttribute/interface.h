/************************************************************************
* ��Ȩ���� (C)2023, �����д����Ӿ��������޹�˾��
*
* �ļ����ƣ�interface.h
* ����ժҪ��
* ����˵����
* ��ǰ�汾��1.0.0
* �� �ߣ�	���»
* ������ڣ�2023 �� 11 �� 7 ��
*
* �޸ļ�¼ 1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
* �޸����ڣ�
* �� �� �ţ�
* �� �� �ˣ�
* �޸����ݣ�
************************************************************************/

#ifndef INTERFACE_H
#define INTERFACE_H

#pragma once
#include<iostream>
#include<string>
#include<windows.h>
#include<vector>
#include<fstream>

#ifdef	MACHINEATTRIBUTE_EXPORTS
#define MACHINEATTRIBUTE_API __declspec(dllexport)
#else
#define MACHINEATTRIBUTE_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	/// <summary>
	/// ��ȡ��ǰ�����ĸ�������
	/// </summary>
	/// <param name="strIniPath">�����TXT�ļ�·��</param>
	/// <param name="strIniName">�����TXT�ļ���</param>
	/// <returns>0����ȡ��Ϣ�ɹ���-1����ȡ��Ϣʧ��</returns>
	MACHINEATTRIBUTE_API int __stdcall GetMachineInfo(const std::string strTxtPath, const std::string strTxtName);

	/// <summary>
	/// ��ȡ��ǰ�����ĸ�������(�޲ΰ�)
	/// </summary>
	/// <returns></returns>
	MACHINEATTRIBUTE_API int __stdcall GetMachineInfoNo();


#ifdef __cplusplus
}
#endif


#endif //INTERFACE_H

