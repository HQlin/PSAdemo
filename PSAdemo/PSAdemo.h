
// PSAdemo.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#include "CalgorithmRegion.h"
#include "HalconBmp.h"
#include "HalconTool.h"
// CPSAdemoApp:
// 有关此类的实现，请参阅 PSAdemo.cpp
//

class CPSAdemoApp : public CWinApp
{
public:
	CPSAdemoApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CPSAdemoApp theApp;


//1.全局变量声明
extern HalconBmp *g_pBmp;								//读图工具
extern HalconTool g_ToolDown;							//显示图像工具
extern bool isRunPic;									//是否跑图
extern int g_SaveImage;									//识别是否保存图像(D:/图像备份/)
extern CalgorithmRegion *g_pCalgorithmRegion;			//轮廓算法工具