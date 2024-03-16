/************************************************************************
* 版权所有 (C)2023, 深圳市大族视觉技术有限公司。
*
* 文件名称：interface.h
* 内容摘要：
* 其它说明：
* 当前版本：1.0.0
* 作 者：	黄炜禄
* 完成日期：2023 年 11 月 7 日
*
* 修改记录 1：// 修改历史记录，包括修改日期、修改者及修改内容
* 修改日期：
* 版 本 号：
* 修 改 人：
* 修改内容：
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
	/// 获取当前机器的各种属性
	/// </summary>
	/// <param name="strIniPath">输出的TXT文件路径</param>
	/// <param name="strIniName">输出的TXT文件名</param>
	/// <returns>0：获取信息成功；-1：获取信息失败</returns>
	MACHINEATTRIBUTE_API int __stdcall GetMachineInfo(const std::string strTxtPath, const std::string strTxtName);

	/// <summary>
	/// 获取当前机器的各种属性(无参版)
	/// </summary>
	/// <returns></returns>
	MACHINEATTRIBUTE_API int __stdcall GetMachineInfoNo();


#ifdef __cplusplus
}
#endif


#endif //INTERFACE_H

